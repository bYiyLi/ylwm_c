//
// Created by 乂 on 2021/6/6.
//

#ifndef YLWM_CONFIG_H
#define YLWM_CONFIG_H
#include <X11/keysymdef.h>
#define Enter 65293
#define ShiftMask		(1<<0)
#define LockMask		(1<<1)
#define ControlMask		(1<<2)
#define AltMask		(1<<3)
#define Mod2Mask		(1<<4)
#define Mod3Mask		(1<<5)
#define Mod4Mask		(1<<6)
#define Mod5Mask		(1<<7)
//25   11001 8+1+16

static char *termcmd[] = { "st", NULL };
static char *rofi_drun[] = { "rofi", "-show","drun" };
static char *rofi_windowcd[] = { "rofi", "-show","windowcd" };
static char *rofi_combi[] = { "rofi", "-show","combi" };
static char *rofi_keys[] = { "rofi", "-show","keys" };
static Key keys[] = {
//        { MOD1,      0x0062, spawn,      browcmd }, /* 0x0062 = XK_b */
//        { MOD1,      0xff0d, spawn,      termcmd }, /* 0xff0d = XK_Enter */
//        { MOD1,      0x0020, spawn,      menucmd }, /* 0x0020 = XK_space */
        { AltMask|Mod2Mask,     XK_r , spawn, rofi_drun },    /* 0x0066 = XK_f */
        { AltMask|Mod2Mask,     XK_w , spawn, rofi_windowcd },    /* 0x0066 = XK_f */
        { AltMask|Mod2Mask,     XK_c , spawn, rofi_combi },    /* 0x0066 = XK_f */
        { AltMask|Mod2Mask,     XK_e , spawn, rofi_keys },    /* 0x0066 = XK_f */
        { ShiftMask|AltMask|Mod2Mask,      XK_c, kill_client, NULL },    /* 0x0071 = XK_q */
        { ShiftMask|AltMask|Mod2Mask,      XK_q, kill_ylwm, NULL },    /* 0x0071 = XK_q */
        { ShiftMask|AltMask|Mod2Mask, XK_Return, spawn,    termcmd }     /* 0x0071 = XK_q */
};

#endif //YLWM_CONFIG_H
