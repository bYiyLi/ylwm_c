#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <inttypes.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>
#include <stdlib.h>
typedef int BOOL;
#define ROOT_EVENT_MASK (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |                                       \
                         XCB_EVENT_MASK_BUTTON_PRESS |                                                \
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | /* when the user adds a screen (e.g. video \
                                                            * projector), the root window gets a      \
                                                            * ConfigureNotify */                      \
                         XCB_EVENT_MASK_POINTER_MOTION |                                              \
                         XCB_EVENT_MASK_PROPERTY_CHANGE |                                             \
                         XCB_EVENT_MASK_FOCUS_CHANGE |                                                \
                         XCB_EVENT_MASK_ENTER_WINDOW)
#define TRUE 1
#define FALSE 0
xcb_connection_t *connection;
BOOL is_run=FALSE;

BOOL init_connection(){
    connection = xcb_connect (NULL,NULL);
    switch (xcb_connection_has_error(connection)) {
        case XCB_CONN_ERROR:{
            fprintf(stderr,"=> xcb connection errors because of socket, pipe and other stream errors.\n");
            fprintf(stderr,"=> 由于套接字，管道和其他流错误，导致xcb连接错误。\n");
            return FALSE;
        }
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:{
            fprintf(stderr,"=> xcb connection shutdown because of extension not supported.\n");
            fprintf(stderr,"=> 由于不支持扩展，xcb连接关闭。\n");
            return FALSE;
        }
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:{
            fprintf(stderr,"=> malloc(), calloc() and realloc() error upon failure, for eg ENOMEM.\n");
            fprintf(stderr,"=> 失败时出现malloc（），calloc（）和realloc（）错误，例如ENOMEM。\n");
            return FALSE;
        }
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:{
            fprintf(stderr,"=> Connection closed, exceeding request length that server accepts.\n");
            fprintf(stderr,"=> 连接已关闭，超出了服务器接受的请求长度。\n");
            return FALSE;
        }
        case XCB_CONN_CLOSED_INVALID_SCREEN:{
            fprintf(stderr,"=> Connection closed because the server does not have a screen matching the display.\n");
            fprintf(stderr,"=> 由于服务器没有与显示匹配的屏幕，因此连接已关闭。\n");
            return FALSE;
        }
        case XCB_CONN_CLOSED_PARSE_ERR:{
            fprintf(stderr,"=> Connection closed, error during parsing display string.\n");
            fprintf(stderr,"=> 连接已关闭，解析显示字符串时出错。\n");
//            return FALSE;
        }
    };
    return TRUE;
};
const xcb_setup_t *setup_t;

typedef struct yl_wm_window yl_wm_window_item;
struct yl_wm_window{
    xcb_window_t *window;
    xcb_get_window_attributes_cookie_t * window_attribute_cookie;
    yl_wm_window_item * up;
    yl_wm_window_item * down;
}*yl_wm_window;

typedef struct  yl_wm_screen yl_wm_screen_node;
 __attribute__((unused)) struct  yl_wm_screen{
    int num;
    yl_wm_screen_node *parent_node;
    yl_wm_screen_node *brother_node;
    xcb_screen_t *screen;
    char * name;
     yl_wm_window_item * windows_root;
} *yl_wm_root_screen_node;

BOOL check_other_wm(){
    //Assigning to 'xcb_setup_t *' (aka 'struct xcb_setup_t *') from 'const struct xcb_setup_t *' discards qualifiers
    setup_t = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup_t);
    if (iter.rem <= 0){
        fprintf(stderr,"当前没有Screen\n");
//        return FALSE;
    }
    xcb_window_t root = iter.data->root;
    xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(connection,root, XCB_CW_EVENT_MASK, (uint32_t[]){ROOT_EVENT_MASK});
    xcb_generic_error_t *error = xcb_request_check(connection, cookie);
    if (error != NULL) {
        fprintf(stderr,"Another window manager seems to be running (X error %d)\n", error->error_code);
        return FALSE;
    }
    return TRUE;
}

/*
 * Go through all existing windows (if the window manager is restarted) and manage them
 *
 */
void init_screen_windows(yl_wm_screen_node *screen_node) {
    xcb_query_tree_reply_t *reply;
    int i, len;
    xcb_window_t *children;
    xcb_get_window_attributes_cookie_t *cookies;
    /* Get the tree of windows whose parent is the root window (= all) */
    if ((reply = xcb_query_tree_reply(connection, xcb_query_tree(connection, screen_node->screen->root), 0)) == NULL)
        return;

    len = xcb_query_tree_children_length(reply);
    if (len<=0){
        return;
    }
    cookies = malloc(len * sizeof(*cookies));
    screen_node->windows_root = malloc(sizeof(yl_wm_window_item));
    yl_wm_window_item *now=screen_node->windows_root;
    /* Request the window attributes for every window */
    children = xcb_query_tree_children(reply);
    for (i = 1; i < len; ++i){
        cookies[i] = xcb_get_window_attributes(connection, children[i]);
        yl_wm_window_item * new= malloc(sizeof(yl_wm_window_item));
        new->window=&children[i];
        new->window_attribute_cookie=&cookies[i];

    }
    free(reply);
}

void init_screen_linked(){
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup_t);
    yl_wm_root_screen_node = malloc(sizeof(yl_wm_screen_node));
    yl_wm_root_screen_node->brother_node=yl_wm_root_screen_node;
    yl_wm_root_screen_node->parent_node=yl_wm_root_screen_node;
    yl_wm_root_screen_node->screen=iter.data;
    yl_wm_root_screen_node->name="yl_wm";
    yl_wm_screen_node * now_node=yl_wm_root_screen_node;
    yl_wm_root_screen_node->num=0;
    init_screen_windows(yl_wm_root_screen_node);
    for(int n=1;iter.rem!=0;n++){
        xcb_screen_next(&iter);
        if (iter.rem==0){
            break;
        }
        yl_wm_screen_node * new_node= malloc(sizeof(yl_wm_screen_node));
        new_node->name="y_wm";
        new_node->screen=iter.data;
        new_node->parent_node=now_node;
        new_node->brother_node=now_node->brother_node;
        now_node->brother_node->parent_node=new_node;
        now_node->brother_node=new_node;
        now_node->num=n;
        init_screen_windows(new_node);
    }


}

int main() {
    is_run=init_connection() && check_other_wm();
    if (is_run){
        printf("check ok\n");
        init_screen_linked();



    }
    printf("connection: %p",connection);

    xcb_disconnect(connection);
    fflush(stderr);
    fflush(stdout);
    return 0;
}
