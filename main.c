


#include "main.h"
#include "config.h"


int
main(int argc, char **argv){
    atexit(cleanup);
    install_sig_handlers();
    init();
    setup();
}

void sigcatch(const int sig){sigcode = sig;}

void
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
    sa.sa_handler = sigcatch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart if interrupted by handler */
    if ( sigaction(SIGINT, &sa, NULL) == -1
         || sigaction(SIGHUP, &sa, NULL) == -1
         || sigaction(SIGTERM, &sa, NULL) == -1)
        exit(-1);
}

bool init(){



}

bool setup(){

}

void cleanup(){
    printf("\n== exit ylwm ==\n");
    if(!conn){
        return;
    }
    xcb_set_input_focus(conn, XCB_NONE,XCB_INPUT_FOCUS_POINTER_ROOT,
                            XCB_CURRENT_TIME);
    xcb_flush(conn);
    xcb_disconnect(conn);
}