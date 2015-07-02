# INCLUDES = @DIRECTFB_CFLAGS@ @GLIB_CFLAGS@

LDADD = @DIRECTFB_LIBS@ @GLIB_LIBS@ @SYSTEMD_LIBS@

DATADIR = @datarootdir@

AM_CPPFLAGS = \
	-DDATADIR=\"$(DATADIR)/bootsplash/\"	\
	-DPACKAGE=\"$(PACKAGE)\" \
	 @DIRECTFB_CFLAGS@ @GLIB_CFLAGS@ @SYSTEMD_CFLAGS@


bin_PROGRAMS = bootsplash

bootsplash_SOURCES = \
	bootsplash.c \
	framebuffer.h \
	framebuffer.c \
	configuration.h \
	configuration.c \
	daemon.c \
	daemon.h \
	comm.h \
	log.c \
	log.h

if SDBUS_IO
bootsplash_SOURCES += io-sdbus.c
else
bootsplash_SOURCES += io-fifo.c
endif
