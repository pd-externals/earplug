# Makefile for earplug~

lib.name = earplug~

# remove this flag when you don't want to embed the IR data in the exteral itself
cflags = -D EMBED_DEFAULT_IR

class.sources = earplug~.c

datafiles = earplug~-help.pd earplug~-meta.pd earplug_data_src.txt \
            parse-to-h.pl README.txt LICENSE.txt

PDLIBBUILDER_DIR=.
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
