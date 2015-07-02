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


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "daemon.h"

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif


static void printHelp()
{
    printf("\n");
    printf("GnublinBootSplash help:\n\n");
    printf
	(" [--help              , -h         ] : Optional, prints this help message\n\n");
    printf
	(" [--verbose=<level>   , -v <level> ] : Optional, prints out some debug information\n");
    printf
	("                                       if there are any. Per default set to max.\n");
    printf
	("                                       can have value from 0 (no logging) to\n");
    printf
	("                                       7 maximum logging (yes syslog has 7 categories...)\n\n");
    printf
	(" [--configfile=<file> , -c <file>  ] : Optional, gives path and filename of\n");
    printf
	("                                       configuration file.\n\n");
    printf
	(" [--dontdaemonize     , -d         ] : don't daemonize, for testing especially\n");
    printf
        ("                                       on non embedded hardware.\n\n");
    printf
	(" [--forceunlock       , -f         ] : force to remove lockfile before start\n\n");
    printf
	(" [--dfb:<opts>                     ] : Optional, DirectFB parameters, see DFB\n");
    printf
	("                                       manual for more information.\n\n");
    printf("\n");
    exit(0);
}

int main(int argc, char **argv)
{


    int ret, c;
    char *config = NULL;
    int verbose = -1;
    int dontdaemonize = -1;
    int forceunlock = -1;
   

    opterr = 0;

    while (1) {
	static struct option long_options[] = {
	    {"help"		, no_argument		, 0, 'h'},
	    {"dontdaemonize"	, no_argument		, 0, 'd'},
	    {"verbose"		, required_argument	, 0, 'v'},
	    {"configfile"	, required_argument	, 0, 'c'},
	    {"forceunlock"	, no_argument		, 0, 'f'},
	    { 0			, 0			, 0,  0}
	};
	/* getopt_long stores the option index here. */
	int option_index = 0;

	c = getopt_long(argc, argv, "hdv:c:f", long_options, &option_index);

	/* Detect the end of the options. */
	if (c == -1)
	    break;

	switch (c) {
	case 0:
	    printHelp();
	    break;

	case 'h':
	    printHelp();
	    break;
	case 'v':
	    verbose = atoi(optarg);
	    if(verbose > 7) verbose = 7; 
	    if(verbose < 0) verbose = 0; 
	    break;
	case 'c':
	    config = optarg;
	    break;
	case 'd':
	    dontdaemonize = FALSE;
	    break;
	case 'f':
	    forceunlock = TRUE;
	    break;
	case '?':
	    if (strncmp(argv[optind - 1], "--dfb:", 6)) {
		printf("\nERROR unknown parameter: %s\n",
		       argv[optind - 1]);
		printHelp();
	    }
	    break;

	default:
	    break;
	    /*    abort(); */
	}
    }




    if ((ret = loadConfig(config ? config : "/etc/gnublinbootsplash.cfg"))) {
	destroyConfig();
	return ret;
    }

    if (verbose >= 0)
	getConf()->verbose = verbose;

 
    if(dontdaemonize != -1) getConf()->daemonize = dontdaemonize;
    if(forceunlock != -1) getConf()->forceunlock = forceunlock;


/*	getConf()->toString(); */
   daemonRun(getConf(), argc, argv);


 destroyConfig();

    return 0;
}
