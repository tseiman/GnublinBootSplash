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


#ifndef FALSE
#define FALSE (0)
#define TRUE (!FALSE)
#endif

typedef struct {
    char *fb_device;
    char *background_image;
    char *progressbar_image;
    int progressbar_image_xpos;
    int progressbar_image_ypos;
    char *status_text_font;
    int status_text_fontsize;
    int status_text_xpos;
    int status_text_ypos;
    int verbose;
    char *fifo;
    char *lock;
    int daemonize;
    int forceunlock;
    void (*toString)(void);
} Settings;


int loadConfig(char *config_file);

Settings *getConf(void);


void destroyConfig(void);
