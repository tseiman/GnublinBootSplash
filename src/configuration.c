/*
 * Author: Thomas 'tseiman' Schmidt <t.schmidt@sh-home.de>
 * 
 * This file is part of the GnublinBootSplash project a simple
 * userpace boot splash screen for the embedded GNUBLIN board
 * available from http://gnublin.embedded-projects.net
 * and the EA DOGXL160 display drivven over SPI bus
 *
 * This module handles all DirectFB interfaces and all
 * graphic routines
 *
 * this project should come with the GNU Public License
 * in hope that it is somehow usefull and an example how
 * (not) to do things
 * 
 * written Fri 21 Jun 2013 01:12:55 AM CEST
 */

#include <glib.h>
#include <malloc.h>
#include <stdio.h>
#include <syslog.h>

#include "log.h"

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif


#define exitAndPrintError(msg) 							\
    if (error) { 								\
	if (error->message) {							\
	    printf ("%s::%s(%d)> ERROR, %s: %s\n", __FILE__, __func__,		\
		 __LINE__, msg, error->message);				\
	} else {								\
	    printf ("%s::%s(%d)> ERROR, %s\n", __FILE__, __func__,		\
		 __LINE__, msg);						\
	}									\
	return error->code;							\
    }										\
    printf("%s::%s(%d)> ERROR, %s\n", __FILE__, __func__, __LINE__, msg);	\
    return -1;


#define printWarning(msg) 							\
    if (error->message) {							\
        printf ("%s::%s(%d)> WARN, %s: %s\n", __FILE__, __func__,		\
		 __LINE__, msg, error->message);				\
     } else {									\
	    printf ("%s::%s(%d)> WARN, %s\n", __FILE__, __func__,		\
		 __LINE__, msg);						\
     }										

#define dumpConfStr(name,val) printf("config->%s=%s\n",name, val); 
#define dumpConfInt(name,val) printf("config->%s=%d\n",name, val); 

#define resetError(err)  							\
	if(err) g_error_free(err);						\
 	err = NULL;

static Settings *conf = NULL;
static GKeyFile *keyfile = NULL;

static void toString()
{

   
    dumpConfStr("fb_device", conf->fb_device);
    dumpConfInt("status_text_xpos", conf->status_text_xpos);
    dumpConfStr("background_image", conf->background_image);
    dumpConfStr("progressbar_image", conf->progressbar_image);
    dumpConfInt("progressbar_image_xpos", conf->progressbar_image_xpos);
    dumpConfInt("progressbar_image_ypos", conf->progressbar_image_ypos);
    dumpConfStr("status_text_font", conf->status_text_font);
    dumpConfInt("status_text_fontsize", conf->status_text_fontsize);
    dumpConfInt("status_text_xpos", conf->status_text_xpos);
    dumpConfInt("status_text_ypos", conf->status_text_ypos);
    dumpConfInt("verbose", conf->verbose);
    dumpConfStr("fifo", conf->fifo);
    dumpConfStr("lock", conf->lock);
    dumpConfInt("daemonize", conf->daemonize);
    dumpConfInt("forceunlock", conf->forceunlock);

}


int loadConfig(char *config_file)
{

    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;
    int *pos;
    
  

    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    /* Load the GKeyFile from keyfile.conf or return. */
    if (!g_key_file_load_from_file(keyfile, config_file, flags, &error)) {
	exitAndPrintError("reading config file");

    }
    conf = g_slice_new(Settings);

    conf->fb_device = NULL;
    conf->background_image = NULL;
    conf->progressbar_image = NULL;
    conf->status_text_font = NULL;


/* reading device name, this can be NULL */
resetError(error);

  conf->fb_device =
	g_key_file_get_string(keyfile, "Framebuffer", "Device",  &error);



/* reading background image information */
resetError(error);

    conf->background_image =
	g_key_file_get_string(keyfile, "Background", "Image",  &error);
    if (!conf->background_image) {
	exitAndPrintError("reading background image");
    }

/* reading progress bar image information */
resetError(error);

    conf->progressbar_image =
	g_key_file_get_string(keyfile, "ProgressBar", "Image",  &error);
    if (!conf->progressbar_image) {
	exitAndPrintError("reading progressbar image");
    }


/* reading progressbar position */
resetError(error);

    pos =
	g_key_file_get_integer_list(keyfile, "ProgressBar", "Pos", &length,
				     &error);

    if (length != 2) {
	exitAndPrintError("reading progress bar image position");
    }

    conf->progressbar_image_xpos = pos[0];
    conf->progressbar_image_ypos = pos[1];

/* reading status text font file name */
resetError(error);

    conf->status_text_font =
	g_key_file_get_string(keyfile, "StatusText", "Font",  &error);
    if (!conf->status_text_font) {
	exitAndPrintError("reading status text font");
    }

/* reading status text font size */
resetError(error);

    conf->status_text_fontsize =
	g_key_file_get_integer(keyfile, "StatusText", "FontSize",  &error);

    if (!conf->status_text_fontsize) {
	exitAndPrintError("reading status text font size");
    }


/* reading status text position */
resetError(error);


    pos =
	g_key_file_get_integer_list(keyfile, "StatusText", "Pos", &length,
				     &error);

    if (length != 2) {
	exitAndPrintError("reading status text position");
    }

    conf->status_text_xpos = pos[0];
    conf->status_text_ypos = pos[1];


/* reading bootsplash flags */

 resetError(error);
    conf->verbose =
	g_key_file_get_integer(keyfile, "GnublinBootSplash", "Verbose",  &error);
	if(error) { printWarning("GnublinBootSplash/Verbose:") };

resetError(error);

    conf->daemonize =
	g_key_file_get_boolean(keyfile, "GnublinBootSplash", "Daemonize", &error); 
	if (error) { printWarning("GnublinBootSplash/Daemonize:") };


	if (error) {
		conf->daemonize = TRUE;
	}

resetError(error);

   conf->fifo =
	g_key_file_get_string(keyfile, "GnublinBootSplash", "Fifo",  &error);


resetError(error);

   conf->lock =
	g_key_file_get_string(keyfile, "GnublinBootSplash", "LockFile",  &error);


resetError(error);

    conf->forceunlock =
	g_key_file_get_boolean(keyfile, "GnublinBootSplash", "ForceUnlock", &error); 
	if (error) { printWarning("GnublinBootSplash/ForceUnlock:") };

	if (error) {
		conf->forceunlock = FALSE;
	}

/* setting oop like toString function */

    conf->toString = toString;

    return 0;

}

Settings *getConf()
{
    return conf;
}



void destroyConfig()
{
    if(!conf) return;

    if(conf->fb_device) free(conf->fb_device);
    if(conf->background_image) free(conf->background_image);
    if(conf->progressbar_image) free(conf->progressbar_image);
    if(conf->status_text_font) free(conf->status_text_font);
    if(conf->fifo) free(conf->fifo);
    if(conf->lock) free(conf->lock);


    if (conf)
	g_slice_free(Settings, conf);
    if (keyfile)
	g_key_file_free(keyfile);
}
