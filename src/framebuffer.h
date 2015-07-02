/*
 * Author: Thomas 'tseiman' Schmidt <t.schmidt@sh-home.de>
 * 
 * This file is part of the GnublinBootSplash project a simple
 * userpace boot splash screen for the embedded GNUBLIN board
 * available from http://gnublin.embedded-projects.net
 * and the EA DOGXL160 display drivven over SPI bus
 *
 * this project should come with the GNU Public License
 * in hope that it is somehow usefull and an example how
 * (not) to do things
 * 
 * written Fri 21 Jun 2013 12:57:55 AM CEST
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif


int setup_splash(int argc, char **argv, Settings *settings);

void display_progress(int percentage, char *text);

void destroy_splash(void);
