

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif


// void bootLoggerLog(int loglevel, const char *format, ...);

static Settings *conf = NULL;

void openBootLog(Settings * settings)
{
    conf = settings;

    if (conf->daemonize) {
	openlog(PACKAGE, LOG_PID | LOG_CONS, LOG_USER);
    }

    bootLogger(LOG_DEBUG,"Set loglevel to (%d)\n",conf->verbose);

}


void bootLoggerLog(int loglevel, const char *format, ...)
{
    char buffer[512];
    va_list arglist;
    va_start(arglist, format);
    vsnprintf( buffer, 512, format,arglist );
    va_end(arglist);


/* printf("conf->verbose=%d, loglevel=%d\n",conf->verbose, loglevel); */

    if (conf->verbose >= loglevel) {
	if (conf->daemonize) {
		syslog(loglevel,"%s",buffer);
	} else {
	    printf("%s",buffer);
	}
    }


}

void closeBootLog()
{

    if (conf->daemonize) {
	closelog();
   }
}
