
#include <stdio.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <string.h> 

#include "log.h"

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif

#define BUFF_SIZE 		512
#define ARGUMENT_COUT_MAX 	256
#define DELIMITER 		";"


#define FIFO_NAME conf->fifo ? conf->fifo : "/tmp/bootsplash.fifo"

static int fd_fifo = 0;
static Settings *conf;

static void (*messageCallback)(int *,char *);
static void (*exitCallback)(void);

void helpPrint() 
{
    bootLogger(LOG_INFO,
	   "HELP: [ percent=<value> ] : percent the progressbar should be set\n");
    bootLogger(LOG_INFO,
	   "HELP:                       to can be in range of 0 to 100\n");
    bootLogger(LOG_INFO,
	   "HELP: [ message=<value> ] : writes the status message to bootsplash\n");
    bootLogger(LOG_INFO,
	   "HELP: [ quit            ] : end the bootsplash daemon\n");
    bootLogger(LOG_INFO,
	   "HELP: [ help            ] : write this message to syslog\n");
    bootLogger(LOG_INFO,
	   "HELP: paraeters are separated by ';' character e.g.: \n");
    bootLogger(LOG_INFO,
	   "HELP: \"precent=100;message=hello world\" settings are applied\n");
    bootLogger(LOG_INFO, "HELP: on FiFo close by client\n");

}

static void run_Command(int argc, char **argv)
{

    int i, percent;
    char *cmd;
    char *parameter = NULL;
    for (i = 0; i < argc; ++i) {
	cmd = strtok(argv[i], "=");
        bootLogger(LOG_DEBUG, "found parameter: \"%s\"\n",  cmd); 
	if (!strncmp(cmd, "help", 4)) {
	    helpPrint();
	} else if (!strncmp(cmd, "quit", 4)) {
	    if(exitCallback) exitCallback();
	} else if (!strncmp(cmd, "message", 7)) {

	    if (!(parameter = strtok(NULL, "="))) {
		bootLogger(LOG_WARNING, "no value for message given\n");
		break;
	    }

	    if(messageCallback) messageCallback(NULL,parameter);

	} else if (!strncmp(cmd, "percent", 7)) {
	    if (!(parameter = strtok(NULL, "="))) {
		bootLogger(LOG_WARNING, "no value for percent given\n");
		break;
	    }
	    percent = atoi(parameter);
	    if(messageCallback) messageCallback(&percent,NULL);

	}

    }


}


int readFifo(char *buffer, size_t n)
{

    return read(fd_fifo, buffer, n);
}



int openComm(Settings *settings)
{

    conf = settings;

    bootLogger(LOG_DEBUG,"FiFo file set to: %s\n", FIFO_NAME);

    unlink(FIFO_NAME);
/*We create a FIFO*/

    if ((mkfifo(FIFO_NAME, O_RDWR | O_NONBLOCK)) == -1) {
	bootLogger(LOG_ERR, "Can't creat a fifo: %s\n",strerror(errno));
	return errno;
    }
/*We open the fifo for read and write*/
    if ((fd_fifo = open(FIFO_NAME, O_RDONLY | O_NONBLOCK)) == -1) {
	bootLogger(LOG_ERR, "Can't open a fifo: %s\n",strerror(errno));
	return errno;
    }

       bootLogger(LOG_INFO, "fifo opened\n");

	return 0;

}

void registerMessageCallbackComm(void (*callback)(int *,char *)) { 
    messageCallback = callback;
}

void registerExitCallbackComm(void (*callback)(void)) { 
    exitCallback = callback;
}



void startComm() {
    char buffer[BUFF_SIZE];
    int read_bytes;
    int read_argc;
    char *read_argv[ARGUMENT_COUT_MAX];
    char *ptr;

	if ((read_bytes = readFifo(buffer, 511))) {
	    if (read_bytes != -1) {
		buffer[read_bytes] = 0x0;

		read_argc = 0;

		ptr = strtok(buffer, DELIMITER);

		while (ptr != NULL) {

		    if ((read_argc + 1) < ARGUMENT_COUT_MAX) {
			read_argv[read_argc] = ptr;
			++read_argc;
		    } else {
			bootLogger(LOG_WARNING, "too much parameters (%s)\n",
			       ptr);
		    }

		    ptr = strtok(NULL, DELIMITER);

		}


		run_Command(read_argc, read_argv);


	    }

	}

	usleep(500000);

}






void closeComm()
{
    bootLogger(LOG_INFO, "closing fifo\n");
    close(fd_fifo);
    unlink(FIFO_NAME);
    fd_fifo = 0;
}
