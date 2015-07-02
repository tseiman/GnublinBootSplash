
#include <string.h>

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "configuration.h"
#endif

/* sick makro shit ;-) */
#define bootLogger(loglevel, format, args...) 				\
	bootLoggerLog(loglevel,"%s::%s(%d)>" format,  __FILE__,	\
		 __func__, __LINE__, ##args )

void openBootLog(Settings * settings);
void bootLoggerLog(int loglevel, const char *format, ...);
void closeBootLog(void);



