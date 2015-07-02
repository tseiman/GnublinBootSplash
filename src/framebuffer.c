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
 * written Fri 21 Jun 2013 01:05:34 AM CEST
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <directfb.h>

#include "log.h"


#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif





/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal (#x, err);                         \
               }








/* prototypes */
void display_progress(int percentage, char *text);


/* config struct */
static Settings *conf = NULL;

/*
typedef struct {
    char *background_image;
    char *progressbar_image;
    int progressbar_image_xpos;
    int progressbar_image_ypos;
    char *status_text_font;
    int status_text_fontsize;
    int status_text_xpos;
    int status_text_ypos;
    void (*toString)(void);
} Settings;

*/


/* DFB surface storing the background */
static IDirectFBSurface *background = NULL;
/* background size */
static int back_width;
static int back_height;

/* DFB surface storing the progressbar image */
static IDirectFBSurface *progress_img = NULL;
/* progressbar size */
static int progress_img_width;
static int progress_img_height;

static IDirectFB *dfb = NULL;
static IDirectFBSurface *primary = NULL;
static IDirectFBSurface *sub_progress = NULL;
static IDirectFBSurface *sub_text = NULL;

static IDirectFBFont *font = NULL;

static DFBResult err;

/* 
 * sets up all DFB and loads background and progress bar image
 *
 * @param argc passed from main() to DFB give number of comand line arguments
 * @param argv points to array of character array where parameters are stored, passed from main routine
 * @return 0 if ok otherwise a minus value
 *
 */
int setup_splash(int argc, char **argv, Settings * settings)
{
    DFBSurfaceDescription dsc;
    DFBSurfaceDescription back_dsc;
    DFBRectangle rect;
    IDirectFBImageProvider *provider;
    DFBFontDescription font_dsc;
    conf = settings;
    int width;
    int height;


    DFBCHECK(DirectFBInit(&argc, &argv));
    DFBCHECK(DirectFBSetOption("font-format", "A1"));
    DFBCHECK(DirectFBSetOption("no-sighandler", NULL));
    if(conf->verbose < 3) DFBCHECK(DirectFBSetOption("quiet",NULL));
    if(conf->fb_device) DFBCHECK(DirectFBSetOption("fbdev", conf->fb_device));

    /* create the super interface */
    DFBCHECK(DirectFBCreate(&dfb));

    DFBCHECK(dfb->SetCooperativeLevel(dfb, DFSCL_FULLSCREEN));


    /*  create the primary surface  */
    dsc.flags = DSDESC_CAPS;
    dsc.caps = DSCAPS_PRIMARY;

  /* dsc.caps |= DSCAPS_TRIPLE; */

    if (dfb->CreateSurface(dfb, &dsc, &primary) != DFB_OK) {
	dsc.caps = (dsc.caps & ~DSCAPS_TRIPLE) | DSCAPS_DOUBLE;
	DFBCHECK(dfb->CreateSurface(dfb, &dsc, &primary));
    }

    /* load size of background image */

    DFBCHECK(dfb->CreateImageProvider
	     (dfb, conf->background_image, &provider));
    DFBCHECK(provider->GetSurfaceDescription(provider, &back_dsc));
    back_width = back_dsc.width;
    back_height = back_dsc.height;

    if (!back_width || !back_height)
	return -1;


    /*  create the background surface  */
    DFBCHECK(dfb->CreateSurface(dfb, &back_dsc, &background));
    provider->RenderTo(provider, background, NULL);
    provider->Release(provider);

    /*  create subsurface for the progres bar */
    primary->GetSize(primary, &width, &height);
    rect.x = 0;
    rect.y = 0;
    rect.w = back_width;
    rect.h = back_height;
    primary->GetSubSurface(primary, &rect, &sub_progress);


   /* primary->Blit(primary, background, NULL, 0, 0);
    primary->Flip(primary, NULL, 0);

    primary->SetClip(primary, NULL);
*/

    /*  create subsurface for the status text */
    primary->GetSubSurface(primary, &rect, &sub_text);




    primary->Blit(primary, background, NULL, 0, 0);
    primary->Flip(primary, NULL, 0);

    primary->SetClip(primary, NULL);


    /* Create and set font */

    font_dsc.flags = DFDESC_HEIGHT;
    font_dsc.height = conf->status_text_fontsize;
    DFBCHECK(dfb->
	     CreateFont(dfb, conf->status_text_font, &font_dsc, &font));
    DFBCHECK(primary->SetFont(primary, font));
/*

 DFBCHECK(background->SetColor(background, 0xff, 0x00, 0x00, 0xff));
    DFBCHECK(background->DrawString
	     (background, "oooooooooooooo", -1, STATUS_TEXT_POS_X, STATUS_TEXT_POS_Y,
	      DSTF_LEFT));
DFBCHECK (background->SetBlittingFlags (background, DSBLIT_BLEND_ALPHACHANNEL));
    DFBCHECK(background->Blit(background, background, NULL, 0, 0));

sleep(3);
*/

    DFBCHECK(dfb->CreateImageProvider
	     (dfb, conf->progressbar_image, &provider));
    DFBCHECK(provider->GetSurfaceDescription(provider, &dsc));

    progress_img_width = dsc.width;
    progress_img_height = dsc.height;

    if (!progress_img_width || !progress_img_height)
	return -1;

    DFBCHECK(dfb->CreateSurface(dfb, &dsc, &progress_img));

    provider->RenderTo(provider, progress_img, NULL);

    provider->Release(provider);



    display_progress(0,NULL);

    return 0;
}






/*
 * draws the percentage image and crops it depending to the given 
 * percentage parameter.
 * 
 * @param percentage value from 0 to 100
 */
void display_progress(int percentage, char *text)
{

    if (percentage > 100 || percentage < 0)
	return;

    int crop_width = ((float) progress_img_width / 100) * percentage;


    DFBRegion region;
    primary->Blit(primary, background, NULL, 0, 0);

    sub_progress->SetBlittingFlags(sub_progress, DSBLIT_BLEND_ALPHACHANNEL);
    region.x1 = 0;
    region.y1 = 0;
    region.x2 = crop_width + conf->progressbar_image_xpos;
    region.y2 = back_height;

    sub_progress->SetClip(sub_progress, &region);


    DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0xff));

	if(text) {
    DFBCHECK(primary->DrawString
	     (primary, text, -1, conf->status_text_xpos,
	      conf->status_text_ypos, DSTF_LEFT));
}

    sub_progress->Blit(sub_progress, progress_img, NULL, conf->progressbar_image_xpos,
	      conf->progressbar_image_ypos);

    primary->Flip(primary, NULL, 0);
}


/* 
 * deletes all DFB resources and cleans DFB up 
 */
void destroy_splash()
{


    if (font)
	font->Release(font);
    if (progress_img)
	progress_img->Release(progress_img);
    if (background)
	background->Release(background);
    if (sub_progress)
	sub_progress->Release(sub_progress);
    if (sub_text)
	sub_text->Release(sub_text);
    if (primary)
	primary->Release(primary);
    if (dfb)
	dfb->Release(dfb);




}


/* --- EOF --- */
