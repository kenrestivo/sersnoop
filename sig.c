/* $Id$ 
	sig.c
	signal handling abstractions
*/

#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <kenmacros.h>

int shutdown;

/************************
    TESTHANDLER
    simple handler for the old-skool style signal callback
    i've abstracted this out of the POSIX handler,
    so i only have to change it in one place if i need to use both
***********************/
void 
testHandler(int sig)
{

    switch (sig){
        case SIGINT:
        case SIGTERM:
            fprintf(stderr,
                "testhandler: this is an SNMP message-- "
                "Somebody Nuked My Program.\n");
            fprintf(stderr, "testhandler: now i am exiting\n");
            shutdown = 1;
            break;
        case SIGHUP:
            fprintf(stderr, "testhandler: hup!\n");
            shutdown = 1;
            break;
        default:
            break;
    }

} /* END TESTHANDLER */



/************************
    SIGNALSETUP
    utility to set up signal handlers.
    run this once at program start
    it's here so as not to pollute main.
    no args, no return
***********************/
void 
signalSetup(void)
{
    struct sigaction action;
    struct sigaction oldact;
    sigset_t set;
    sigset_t saveSet;
    int i;
    /* TODO: make this a va_arglist */
    int sigs[] = {SIGINT, SIGHUP, SIGTERM};

    /* block all sigs so we're not disturbed while we're setting this up */
    SYSCALL(sigfillset(&set) );

    SYSCALL(sigprocmask(SIG_SETMASK, &set, &saveSet))


    /* set up signal stuff */
        /* TODO: only nocldstop/info are posix, 
            others are solaris and bsd!  do i need them?
        */
    action.sa_flags = (SA_NOCLDSTOP | SA_RESTART);

    /* if verbose is set, do this the looong posix way, otherwise the oldskool way */
    action.sa_handler = testHandler;
    /* now set what signals for the HANDLER to block */
    SYSCALL(sigfillset(&action.sa_mask));

    /* now set ths signals for the hanler to HANDLE */
    for(i = 0; i < sizeof(sigs)/sizeof(sigs[0]); i++){
        /* if this is zero, i fuxored up */
        assert(sigs[i] != 0);
        SYSCALL(sigaction(sigs[i], &action, &oldact) );
    }

    /* now that we are all set, go back and fix the sigmask */
    SYSCALL(sigprocmask(SIG_SETMASK,  &saveSet, NULL));

} /* END SIGNALSETUP */



/* EOF */
