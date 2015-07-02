
#include <stdio.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <string.h> 
#include <systemd/sd-bus.h>
#include <stdbool.h>

#include "log.h"

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif

#define BUFF_SIZE 		512
#define ARGUMENT_COUT_MAX 	256
#define DELIMITER 		";"

#define BUS_PATH "kernel:path=/sys/fs/kdbus/0-system/bus"


#define io_sdbus_error_ret(message, err)  if (r < 0) { bootLogger(LOG_ERR, message ": %s\n",strerror(-err)); return err; }

static sd_bus_slot *slot = NULL;
static sd_bus *bus = NULL;
static Settings *conf;

static void (*messageCallback)(int *,char *);
static void (*exitCallback)(void);



void registerMessageCallbackComm(void (*callback)(int *,char *)) { 
    messageCallback = callback;
}

void registerExitCallbackComm(void (*callback)(void)) { 
    exitCallback = callback;
}




int openComm(Settings *settings)
{
    int r;
    conf = settings;


    bootLogger(LOG_DEBUG, "connect to bus\n");
    
    r = sd_bus_new(&bus);
    io_sdbus_error_ret("Failed to allocate bus", r);
    
    r = sd_bus_negotiate_creds(bus,true,_SD_BUS_CREDS_ALL);
    io_sdbus_error_ret("Failed to set bus creds", r);
    
    r = sd_bus_negotiate_timestamp(bus, true);
    io_sdbus_error_ret("Failed to negotiate timestamp", r);
    
    r = sd_bus_negotiate_fds(bus,true);
    io_sdbus_error_ret("Failed to negotiate filedesciptor passing", r);
    
    r = sd_bus_set_address(bus, BUS_PATH);
    io_sdbus_error_ret("Failed to set bus address", r);
    
    r = sd_bus_start(bus);
    io_sdbus_error_ret("Failed to start bus", r);
    
    r = sd_bus_add_match(bus, NULL, "", NULL, NULL);
    io_sdbus_error_ret("Failed to add gerneric match to bus", r);
    
    
    
    return 0;
}


void startComm() {
    const char *path, *result, *unit;
    uint32_t id;
    sd_bus_message *m = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    int percent = 0;
    double float_percent = 0.0;
    char * message;
    int len  = 0;
    int r;
    
    /* Process requests */
    r = sd_bus_process(bus, &m);
    
    if (r < 0) {
            bootLogger(LOG_ERR, "Failed to process bus: %s\n", strerror(-r));
//                        goto finish;
    }
    if(m) {
        if (sd_bus_message_is_signal(m, "org.freedesktop.systemd1.Manager", "JobRemoved") > 0) {
            r = sd_bus_message_read(m, "uoss", &id,&path, &unit,&result);

	    if (r < 0) bootLogger(LOG_ERR, "Failed to decode Message: %s\n", strerror(-r));


    	    r = sd_bus_get_property(bus, 
    				"org.freedesktop.systemd1", 
    				"/org/freedesktop/systemd1", 
    				"org.freedesktop.systemd1.Manager", 
    				"Progress", &error, &m,"d");                       

	    if (r < 0) bootLogger(LOG_ERR, "Failed to get progress property: %s\n", strerror(-r));


    	    /* Parse the response message */
    	    r = sd_bus_message_read_basic(m,  'd', (void *) &float_percent );
    	    if (r < 0) bootLogger(LOG_ERR, "Failed to decode property: %s\n", strerror(-r));

	    
	    percent = float_percent * 100;
	    
	    len = strnlen(unit,conf->status_text_maxlen);
	    message = (char *)malloc((len + 20) *(sizeof(char)));
	    if(message == 0) {
    		bootLogger(LOG_ERR, "cant allocate any memory: %s\n", strerror(errno));
    		return;
	    }
	    memset(message,0,len + 20);
	    strncpy(message,unit,len);
	    if(len > conf->status_text_maxlen - 3) strncat(message,"...",3);
	    strncat(message,", ",2);
	    strncat(message,result,10);
	    bootLogger(LOG_DEBUG,"Calling messageCallback with message=%s, percent=%d, %f\n",message,percent, float_percent);
	    messageCallback(&percent, message);
            free(message);
	}

        return;
    }

    if (r > 0) /* we processed a request, try to process another one, right-away */
            return;

    /* Wait for the next request to process */
    r = sd_bus_wait(bus, (uint64_t) -1);
    if (r < 0) {
        bootLogger(LOG_ERR, "Failed to process bus: %s\n", strerror(-r));
//                        goto finish;
    }

}


void closeComm()
{
    bootLogger(LOG_DEBUG, "disconnect from bus\n");
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);
}
