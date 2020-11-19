# Makefile for earplug~

lib.name = earplug~

class.sources = earplug~.c

datafiles = earplug~-help.pd earplug~-meta.pd earplug_data_src.txt \
            parse-to-h.pl README.txt LICENSE.txt

include Makefile.pdlibbuilder