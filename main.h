//
// Created by 乂 on 2021/6/6.
//

#ifndef YLWM_MAIN_H
#define YLWM_MAIN_H
#include <xcb/xcb.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

xcb_connection_t *conn = NULL;             // Connection to X server.
int sigcode = 0;                           // Signal code. Non-zero if we've been interruped by a signal.

void
install_sig_handlers(void);

bool init();

bool setup();

void cleanup();





#endif //YLWM_MAIN_H
