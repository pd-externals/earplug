# Makefile for earplug~

lib.name = earplug~

cflags = -I./conv
class.sources = earplug~.c conv/convolution.c conv/fft-dif.c conv/ift-dif.c

datafiles = earplug~-help.pd earplug~-meta.pd earplug_data_src.txt \
            parse-to-h.pl README.txt LICENSE.txt

PDLIBBUILDER_DIR=.
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
