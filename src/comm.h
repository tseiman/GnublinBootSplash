
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif


int openComm(Settings * settings);
void registerMessageCallbackComm(void (*callback)(int *,char *));
void registerExitCallbackComm(void (*callback)(void));
void startComm(void);
void closeComm(void);

