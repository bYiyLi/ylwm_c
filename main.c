#include <wait.h>
#include "main.h"
#include "config.h"
int
main(int argc, char **argv){
    atexit(cleanup);
    install_sig_handlers();
    puts("run ==");
    fflush(stderr);
    fflush(stdout);
    if (init()&&setup()) {
        printf("\n screen_num:%d \n",screen_num);
        while (xcb_flush(conn)&&!run()&&is_run) {
            fflush(stderr);
            fflush(stdout);
        }
    }
}

static void kill_client(xcb_generic_event_t *event) {

}

void kill_ylwm(){
    is_run=false;
    puts("kill wm");
}

static void sig_catch(const int sig){sig_code = sig;}

static void
install_sig_handlers(void)
{
    struct sigaction sa;
    struct sigaction sa_chld;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    //could not initialize signal handler
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        exit(-1);
    sa.sa_handler = sig_catch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart if interrupted by handler */
    if ( sigaction(SIGINT, &sa, NULL) == -1
         || sigaction(SIGHUP, &sa, NULL) == -1
         || sigaction(SIGTERM, &sa, NULL) == -1)
        exit(-1);
}

static bool init(void){
    puts("ylwm run fun init");
    conn = xcb_connect(NULL, &screen_num);
    switch (xcb_connection_has_error(conn)) {
        case XCB_CONN_ERROR:{
            die("xcb connection errors because of socket, pipe and other stream errors.");
            return false;
        }
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:{
            die("xcb connection shutdown because of extension not supported");
            return false;
        }
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:{
            die("malloc(), calloc() and realloc() error upon failure, for eg ENOMEM");
            return false;
        }
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:{
            die("Connection closed, exceeding request length that server accepts.");
            return false;
        }
        case XCB_CONN_CLOSED_PARSE_ERR:{
            die("Connection closed, error during parsing display string.");
            return false;
        }
        case XCB_CONN_CLOSED_INVALID_SCREEN:{
            die("Connection closed because the server does not have a screen matching the display.");
            return false;
        }
        case XCB_CONN_CLOSED_FDPASSING_FAILED:{
            die("Connection closed because some FD passing operation failed");
            return false;
        }
    };
    if (!(xcb_setup = xcb_get_setup(conn))){
        return false;
    }
    int index = 0;
    for (xcb_screen_iterator_t screen_iterator_t = xcb_setup_roots_iterator(xcb_setup);
        screen_iterator_t.rem!=0 ;
        xcb_screen_next(&screen_iterator_t),index++) {
        puts("ylwm has xcb_screen");
        ylwm_screen_list = realloc((void *)ylwm_screen_list,(++screen_list_size) * sizeof(ylwm_screen));
        ylwm_screen_list[screen_list_size-1].current_screen = screen_iterator_t.data;
        ylwm_screen_list[screen_list_size-1].index = index;
        printf("now screen index:%d , %d ++++ the screen_list_size is %d\n",index,screen_iterator_t.index,screen_list_size);
        init_setup(ylwm_screen_list[screen_list_size-1]);
    }
    return true;
}

static void init_setup(ylwm_screen current_screen){
    values[0] =
            XCB_EVENT_MASK_KEY_PRESS |
            XCB_EVENT_MASK_KEYMAP_STATE|
            XCB_EVENT_MASK_KEY_RELEASE |
            XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
            | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
            | XCB_EVENT_MASK_PROPERTY_CHANGE
            | XCB_EVENT_MASK_BUTTON_PRESS
    ;
    xcb_generic_error_t *error = xcb_request_check(conn,
                                                   xcb_change_window_attributes_checked(conn,
                                                                                        current_screen.current_screen->root,
                                                                                        XCB_CW_EVENT_MASK,
                                                                                        values));
    xcb_flush(conn);
    if (error){
        fprintf(stderr,"%s\n","xcb_request_check:failed.");
        free(error);
        return;
    }
    xcb_ungrab_key(conn, XCB_GRAB_ANY, current_screen.current_screen->root, XCB_MOD_MASK_ANY);
    int key_table_size = sizeof(keys) / sizeof(*keys);
    for (int i = 0; i < key_table_size; ++i) {
        xcb_keycode_t *keycode = xcb_get_keycodes(keys[i].keysym);
        if (keycode != NULL) {
            xcb_grab_key(conn, 1, current_screen.current_screen->root, keys[i].mod, *keycode,
                         XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC );
        }
    }
    xcb_flush(conn);
    xcb_grab_button(conn, 0, current_screen.current_screen->root, XCB_EVENT_MASK_BUTTON_PRESS |
                                        XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
                    XCB_GRAB_MODE_ASYNC, current_screen.current_screen->root, XCB_NONE, 1, ShiftMask);
    xcb_grab_button(conn, 0, current_screen.current_screen->root, XCB_EVENT_MASK_BUTTON_PRESS |
                                        XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
                    XCB_GRAB_MODE_ASYNC, current_screen.current_screen->root, XCB_NONE, 3, AltMask);
    xcb_flush(conn);
}

static xcb_keycode_t *xcb_get_keycodes(xcb_keysym_t keysym) {
    xcb_key_symbols_t *key_sym = xcb_key_symbols_alloc(conn);
    xcb_keycode_t *keycode;
    keycode = (!(key_sym) ? NULL : xcb_key_symbols_get_keycode(key_sym, keysym));
    xcb_key_symbols_free(key_sym);
    return keycode;
}

static bool setup(){
    return true;
}

static void cleanup(){
    printf("\n== exit ylwm ==\n");
    if(conn){
        xcb_set_input_focus(conn, XCB_NONE,XCB_INPUT_FOCUS_POINTER_ROOT,
                            XCB_CURRENT_TIME);
        xcb_flush(conn);
        xcb_disconnect(conn);
    }
//    if (xcb_setup){
//        free((void *)xcb_setup);
//    }
    fflush(stderr);
    fflush(stdout);
}
static int die(char *err_str) {
    size_t n = 0;
    char *p = err_str;
    while ((* (p++)) != 0) {
        ++n;
    }
    ssize_t o = write(STDERR_FILENO, err_str, n);
    int ret = 1;
    if (o < 0) {
        ret = -1;
    }
    return ret;
}

static void handleMotionNotify(xcb_generic_event_t *ev){
    puts("handleMotionNotify\n");
}
static void handleEnterNotify(xcb_generic_event_t *ev){
    puts("handleEnterNotify\n");
}
static void handleDestroyNotify(xcb_generic_event_t *ev){
    puts("handleDestroyNotify\n");
}
static void handleButtonPress(xcb_generic_event_t *ev){
    puts("handleButtonPress\n");
}
static void handleButtonRelease(xcb_generic_event_t *ev){
    puts("handleButtonRelease\n");
}
void
print_modifiers (uint32_t mask)
{
    const char *MODIFIERS[] = {
            "Shift", "Lock", "Ctrl", "Alt",
            "Mod2", "Mod3", "Mod4", "Mod5",
            "Button1", "Button2", "Button3", "Button4", "Button5"
    };

    printf ("Modifier mask: ");
    for (const char **modifier = MODIFIERS ; mask; mask >>= 1, ++modifier) {
        if (mask & 1) {
            printf (*modifier);
        }
    }
    printf ("\n");
}


static void handleKeyPress(xcb_generic_event_t *ev){
    puts("handleKeyPress\n");
    xcb_key_press_event_t *e = ( xcb_key_press_event_t *) ev;
    xcb_keysym_t keysym = xcb_get_keysym(e->detail);
    printf("keysym:%c   %d ,mod:%d\n",keysym,keysym,e->state);
    print_modifiers(e->state);
    int key_table_size = sizeof(keys) / sizeof(*keys);
    for (int i = 0; i < key_table_size; ++i) {
        if ((keys[i].keysym == keysym) && (keys[i].mod == e->state)) {
            keys[i].func(keys[i].com);
        }
    }
}

static xcb_keysym_t xcb_get_keysym(xcb_keycode_t keycode) {
    xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(conn);
    xcb_keysym_t keysym;
    keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keycode, 0));
    xcb_key_symbols_free(keysyms);
    return keysym;
}
static void handleMapRequest(xcb_generic_event_t *ev){
    puts("handleMapRequest\n");
    xcb_map_request_event_t *e = (xcb_map_request_event_t *) ev;
    xcb_map_window(conn, e->window);
//    uint32_t vals[5];
//    vals[0] = (scre->width_in_pixels / 2) - (WINDOW_WIDTH / 2);
//    vals[1] = (scre->height_in_pixels / 2) - (WINDOW_HEIGHT / 2);
//    vals[2] = WINDOW_WIDTH;
//    vals[3] = WINDOW_HEIGHT;
//    vals[4] = BORDER_WIDTH;
//    xcb_configure_window(dpy, e->window, XCB_CONFIG_WINDOW_X |
//                                         XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
//                                         XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_BORDER_WIDTH, vals);
    xcb_flush(conn);
    values[0] = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
    xcb_change_window_attributes_checked(conn, e->window,
                                         XCB_CW_EVENT_MASK, values);
//    setFocus(e->window);
}
static void handleFocusIn(xcb_generic_event_t *ev){
    puts("handleFocusIn\n");
}
static void handleFocusOut(xcb_generic_event_t *ev){
    puts("handleFocusOut\n");
}

static int run(){
    int ret = xcb_connection_has_error(conn);
    if (ret == 0) {
        xcb_generic_event_t *ev = xcb_wait_for_event(conn);
        if (ev != NULL) {
            printf("ev->response_type:%d\n",ev->response_type);
            handler_func_t *handler;
            for (handler = handler_fun_list; handler->func != NULL; handler++) {
                if ((ev->response_type & ~0x80) == handler->request) {
                    handler->func(ev);
                }
            }
            free(ev);
        }
    }
    xcb_flush(conn);
    return ret;
}
static void spawn(char **com) {
    puts(com[0]);
    if (fork() == 0) {
        setsid();
        if (fork() != 0) {
            _exit(0);
        }
        execvp((char*)com[0], (char**)com);
        _exit(0);
    }
    wait(NULL);
}
