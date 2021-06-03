# Makefile for earplug~

lib.name = earplug~

class.sources = earplug~.c

# set this to disable embedding the default impulse response data set
#cflags = -DEARPLUG_DATA_NO_EMBED

datafiles = earplug~-help.pd earplug~-meta.pd earplug_data.h earplug_data.txt \
            parse-to-h.py README.txt LICENSE.txt

PDLIBBUILDER_DIR=.
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
