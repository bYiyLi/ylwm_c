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
//typedef int BOOL;
#define ROOT_EVENT_MASK (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |                                       \
                         XCB_EVENT_MASK_BUTTON_PRESS |                                                \
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | /* when the user adds a screen (e.g. video \
                                                            * projector), the root window gets a      \
                                                            * ConfigureNotify */                      \
                         XCB_EVENT_MASK_POINTER_MOTION |                                              \
                         XCB_EVENT_MASK_PROPERTY_CHANGE |                                             \
                         XCB_EVENT_MASK_FOCUS_CHANGE |                                                \
                         XCB_EVENT_MASK_ENTER_WINDOW)


typedef enum {
    TRUE=1,
    FALSE=0
}BOOL;

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

typedef struct{
    xcb_window_t *window;
    xcb_get_window_attributes_cookie_t * window_attribute_cookie;
}yl_wm_window;

typedef struct {

}yl_wm_page;

int yl_wm_screen_size=0;
typedef struct  yl_wm_screen{
    xcb_screen_t *screen;
    int page_size;
    int page_now;
    yl_wm_page * yl_wm_page;
} yl_wm_screen_item;


yl_wm_screen_item * root_screen_node;

BOOL init_page(yl_wm_screen_item screen_node);
BOOL init_windows(yl_wm_page page,xcb_window_t root);
BOOL init_screen_linked();


BOOL check_other_wm(){
    //Assigning to 'xcb_setup_t *' (aka 'struct xcb_setup_t *') from 'const struct xcb_setup_t *' discards qualifiers
    setup_t = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup_t);
    if (iter.rem <= 0){
        fprintf(stderr,"当前没有Screen\n");
        return FALSE;
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
BOOL init_page(yl_wm_screen_item screen_node){
    screen_node.page_size=1;
    screen_node.page_now=1;
    screen_node.yl_wm_page= malloc(sizeof(yl_wm_page));
    return init_windows(*screen_node.yl_wm_page,screen_node.screen->root);
}
BOOL init_windows(yl_wm_page page,xcb_window_t root) {
    xcb_query_tree_reply_t *reply;
    int i, len;
    xcb_window_t *children;
    xcb_get_window_attributes_cookie_t *cookies;
    /* Get the tree of windows whose parent is the root window (= all) */
    if ((reply = xcb_query_tree_reply(connection, xcb_query_tree(connection, root), NULL)) == NULL)
        return FALSE;
    len = xcb_query_tree_children_length(reply);
    printf("\n init_windows xcb_query_tree_children_length :%d\n",len);
    if (len<=0){
        return TRUE;
    }
    cookies = malloc(len * sizeof(*cookies));
    /* Request the window attributes for every window */
    children = xcb_query_tree_children(reply);
    for (i = 0; i < len; ++i){
        printf("\nchildren %d:",children[i]);

//        cookies[i] = xcb_get_window_attributes(connection, children[i]);
//        yl_wm_window * new= malloc(sizeof(yl_wm_window));
//        new->window=&children[i];
//        new->window_attribute_cookie=&cookies[i];
    }
    free(reply);
    return TRUE;
}

BOOL init_screen_linked(){
    printf("==============================");
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup_t);
    do {
        yl_wm_screen_size++;
        if ((root_screen_node=realloc(root_screen_node,yl_wm_screen_size*sizeof(yl_wm_screen_item)))==NULL){
            fprintf(stderr,"err Dealloc root_screen_node\n");
            return FALSE;
        }
        root_screen_node[yl_wm_screen_size-1].screen=iter.data;

        if (!init_page(root_screen_node[yl_wm_screen_size-1])){
            fprintf(stderr,"init page err\n");
            return FALSE;
        };
        xcb_screen_next(&iter);
    } while (iter.rem!=0);
    return TRUE;
}

int main() {
    is_run=init_connection() && check_other_wm();
    if (is_run){
        printf("check ok\n");
        init_screen_linked();



    }
    printf("\nconnection: %p\n",connection);
    printf("\nroot_screen_node: %p\n",root_screen_node);
    printf("\nyl_wm_screen_size: %d\n",yl_wm_screen_size);
    xcb_disconnect(connection);
    free(root_screen_node);
    fflush(stderr);
    fflush(stdout);
    return 0;
}
