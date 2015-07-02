
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdbool.h>

#include "comm.h"
#include "framebuffer.h"
#include "log.h"


#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif

#define LOCK_FILE_NAME		conf->lock ? conf->lock :  "/var/lock/bootsplash.lock"

static Settings *conf = NULL;

// static int run = FALSE;

static char *message = NULL;
static int percent = 0;



/* this function is derived from http://wordptr.com/2012/11/25/linux-daemon-lock-file-a-bug-retrospective/ */

static int set_lockfile()
{
    char pidtext[64];
    struct stat sts;
    int lfp;
    int result = EEXIST;

    if(conf->forceunlock) { 
    	bootLogger(LOG_WARNING, "WARNING: removing lockfile %s, if there is one.\n", LOCK_FILE_NAME);
    	bootLogger(LOG_WARNING, "WARNING: please note that there is no lock file checking and the process might run twice\n");
	unlink(LOCK_FILE_NAME);
    }

    if (stat(LOCK_FILE_NAME, &sts) != 0 && errno == ENOENT) {
	lfp = open(LOCK_FILE_NAME, O_WRONLY | O_CREAT | O_EXCL, 0640);
	if (lfp > -1) {
	    if (ftruncate(lfp, 0) == 0) {

		snprintf(pidtext, sizeof(pidtext), "%ld\n",
			 (long) getpid());
		int r = write(lfp, pidtext, strlen(pidtext));
		if (r > 0) {
		    bootLogger(LOG_INFO, "created lock file: %s\n",
			   LOCK_FILE_NAME);
		    result = 0;
		} else {
		    return errno;
		}
	    } else {
		return errno;

	    }
	} else {
	    return errno;
	}

    }
    return result;
}


/* this function is derived from http://wordptr.com/2012/11/25/linux-daemon-lock-file-a-bug-retrospective/ */

static void remove_lockfile()
{
    bootLogger(LOG_INFO, "removing lock file: %s\n", LOCK_FILE_NAME);
    unlink(LOCK_FILE_NAME);
}


void callbackExit() 
{ 
	destroy_splash();
        closeComm();
	remove_lockfile();
	bootLogger(LOG_INFO, "%s", conf->daemonize ? "stop daemon\n" :"stop process\n" );
	closeBootLog();
	destroyConfig();
	exit(0);
}



void sig_handler(int signo)
{
    if ((signo == SIGINT) || (signo == SIGTERM)) {
        bootLogger(LOG_INFO, "catched SIGINT or SIGTERM\n");

	callbackExit();
    }
}



void callbackSetDisplayProgress(int *progress, char *msg) 
{
    bootLogger(LOG_DEBUG, "called with message=%s, progress=%d \n", msg, progress ? *progress : 0);

    if(progress)  percent = *progress;
    if(msg)  message = msg;

    display_progress(percent, message ? message : "");
    bootLogger(LOG_DEBUG, "message=%s, percent=%d \n", message ? message : "" , percent);

}

void daemonRun(Settings * settings, int argc, char **argv)
{
    int ret = 0;
    pid_t pid = 0;
    pid_t sid = 0;



    conf = settings;

    openBootLog(conf);
    if (conf->daemonize) {
	pid = fork();

/* Indication of fork() failure */
	if (pid < 0) {
	    bootLogger(LOG_ERR, "fork failed: %s \n", strerror(errno));
	    destroyConfig();
	    exit(1);		/* Return failure in exit status */
	}
	if (pid > 0) {		/* PARENT PROCESS. Need to kill it. */
	    bootLogger(LOG_INFO, "process_id of child process %d \n", pid);
    	    closeBootLog();
/*	    destroyConfig(); - we have just one instance which was not modified - no need to free here */
	    exit(0);		/* return success in exit status */
	}
	umask(0);		/* unmask the file mode */

	sid = setsid();		/* set new session */
	if (sid < 0) {		/* Return failure */
	    bootLogger(LOG_ERR, "Could not set session ID: %s \n",
		   strerror(errno));
	    closeBootLog();
	    destroyConfig();
	    exit(1);
	}



     }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
	bootLogger(LOG_WARNING, "can't catch SIGINT: %s \n", strerror(errno));
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
	bootLogger(LOG_WARNING, "can't catch SIGTERM: %s \n", strerror(errno));

    if (conf->daemonize) {
	if (chdir("/"))
	    bootLogger(LOG_WARNING, "cannot change dir to '/': %s \n", strerror(errno));	/* Change the current working directory to root. */
    }

    if ((ret = set_lockfile())) {
	bootLogger(LOG_ERR, "Child exiting cause lockfile (%s) problem: %s\n", LOCK_FILE_NAME,
	       strerror(ret));
	closeBootLog();
	destroyConfig();
	exit(1);
    }

    bootLogger(LOG_INFO, "%s", conf->daemonize ? "start daemon child\n" :"start process\n"  );


    if (conf->daemonize) {


	close(STDIN_FILENO);	/* Close stdin. stdout and stderr */
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

    }

    if (!conf->daemonize) {
	bootLogger(LOG_INFO, "started not as daemon, logging to STDOUT\n");
    }

    message = "";		/* &defaultMessage; */


    openComm(conf);


     bootLogger(LOG_DEBUG, "Setting up splash ...\n");
    if (setup_splash(argc, argv, getConf())) {
	destroy_splash();
	bootLogger(LOG_ERR, "Was not able to setup splash, exiting\n");
	destroyConfig();
	closeBootLog();
	exit(2);
    }

     bootLogger(LOG_DEBUG, "splash OK\n");

    registerMessageCallbackComm(&callbackSetDisplayProgress);
    registerExitCallbackComm(&callbackExit);


//    run = TRUE;

    while (true) {
	startComm();
    }
    



/*    destroy_splash();
    closeComm();
    remove_lockfile();
    bootLogger(LOG_INFO, "%s", conf->daemonize ? "stop daemon\n" :"stop process\n" );
    closeBootLog();
*/
 /*   exit(0); */

   // return 0; 

}
