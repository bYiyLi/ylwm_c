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
#define XK_a                             0x0061  /* U+0061 LATIN SMALL LETTER A */
#define XK_b                             0x0062  /* U+0062 LATIN SMALL LETTER B */
#define XK_c                             0x0063  /* U+0063 LATIN SMALL LETTER C */
#define XK_d                             0x0064  /* U+0064 LATIN SMALL LETTER D */
#define XK_e                             0x0065  /* U+0065 LATIN SMALL LETTER E */
#define XK_f                             0x0066  /* U+0066 LATIN SMALL LETTER F */
#define XK_g                             0x0067  /* U+0067 LATIN SMALL LETTER G */
#define XK_h                             0x0068  /* U+0068 LATIN SMALL LETTER H */
#define XK_i                             0x0069  /* U+0069 LATIN SMALL LETTER I */
#define XK_j                             0x006a  /* U+006A LATIN SMALL LETTER J */
#define XK_k                             0x006b  /* U+006B LATIN SMALL LETTER K */
#define XK_l                             0x006c  /* U+006C LATIN SMALL LETTER L */
#define XK_m                             0x006d  /* U+006D LATIN SMALL LETTER M */
#define XK_n                             0x006e  /* U+006E LATIN SMALL LETTER N */
#define XK_o                             0x006f  /* U+006F LATIN SMALL LETTER O */
#define XK_p                             0x0070  /* U+0070 LATIN SMALL LETTER P */
#define XK_q                             0x0071  /* U+0071 LATIN SMALL LETTER Q */
#define XK_r                             0x0072  /* U+0072 LATIN SMALL LETTER R */
#define XK_s                             0x0073  /* U+0073 LATIN SMALL LETTER S */
#define XK_t                             0x0074  /* U+0074 LATIN SMALL LETTER T */
#define XK_u                             0x0075  /* U+0075 LATIN SMALL LETTER U */
#define XK_v                             0x0076  /* U+0076 LATIN SMALL LETTER V */
#define XK_w                             0x0077  /* U+0077 LATIN SMALL LETTER W */
#define XK_x                             0x0078  /* U+0078 LATIN SMALL LETTER X */
#define XK_y                             0x0079  /* U+0079 LATIN SMALL LETTER Y */
#define XK_z                             0x007a  /* U+007A LATIN SMALL LETTER Z */

#endif //YLWM_MAIN_H
