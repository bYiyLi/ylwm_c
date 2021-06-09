//
// Created by 乂 on 2021/6/6.
//

#ifndef YLWM_MAIN_H
#define YLWM_MAIN_H
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <xcb/xcb_keysyms.h>

#include <unistd.h>
typedef struct {
    unsigned int mod;
    xcb_keysym_t keysym;
    void (*func)(char **com);
    char **com;
} Key;
xcb_connection_t *conn = NULL;             // Connection to X server.
const struct xcb_setup_t * xcb_setup = NULL;
xcb_generic_event_t *current_ev;
int sig_code = 0;                           // Signal code. Non-zero if we've been interruped by a signal.
int screen_num = -1;
bool is_run = true;
typedef struct ylwm_screen{
    xcb_screen_t *current_screen;
    int index;
}ylwm_screen;
uint32_t values[3];
static int screen_list_size = 0;
static ylwm_screen *ylwm_screen_list;
typedef struct {
    uint32_t request;
    void (*func)(xcb_generic_event_t *ev);
} handler_func_t;
//===========================================================
void kill_ylwm();
static void kill_client();
static void spawn(char **com);

//===============================================
static void register_existing_windows(ylwm_screen screen);
static void register_events(xcb_window_t window);
//==========================================================
static char * window_name(xcb_window_t window);

static xcb_keycode_t *xcb_get_keycodes(xcb_keysym_t keysym);

static xcb_keysym_t xcb_get_keysym(xcb_keycode_t keycode);

static void install_sig_handlers(void);

static bool init(void);

static void init_setup(ylwm_screen current_screen);

static bool setup(void);

static void cleanup(void);

static int die(char *err_str);

static int run();

static void handleMotionNotify(xcb_generic_event_t *ev);
static void handleEnterNotify(xcb_generic_event_t *ev);
static void handleDestroyNotify(xcb_generic_event_t *ev);
static void handleButtonPress(xcb_generic_event_t *ev);
static void handleButtonRelease(xcb_generic_event_t *ev);
static void handleKeyPress(xcb_generic_event_t *ev);
static void handleMapRequest(xcb_generic_event_t *ev);
static void handleFocusIn(xcb_generic_event_t *ev);
static void handleFocusOut(xcb_generic_event_t *ev);

static handler_func_t handler_fun_list[] = {
        { XCB_MOTION_NOTIFY,  handleMotionNotify },
        { XCB_ENTER_NOTIFY,   handleEnterNotify },
        { XCB_DESTROY_NOTIFY, handleDestroyNotify },
        { XCB_BUTTON_PRESS,   handleButtonPress },
        { XCB_BUTTON_RELEASE, handleButtonRelease },
        { XCB_KEY_PRESS,      handleKeyPress },
        { XCB_MAP_REQUEST,    handleMapRequest },
        { XCB_FOCUS_IN,       handleFocusIn },
        { XCB_FOCUS_OUT,      handleFocusOut },
        { XCB_NONE,           NULL }
};


#define XK_Return                        0xff0d  /* Return, enter */
#define XK_q                             0x0071  /* U+0071 LATIN SMALL LETTER Q */
#define XK_c                             0x0063  /* U+0063 LATIN SMALL LETTER C */


#endif //YLWM_MAIN_H
