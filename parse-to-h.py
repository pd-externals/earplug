#! /usr/bin/env python3
#
# convert KEMAR head-related impulse response measurement data set to C header
# file 3 dimensional array
#
# usage: parse-to-h.py [DATAFILE]
#
# Chikashi Miyama 2021
# adapted from original parse-to-h.pl Perl script by Hans-Christoph Steiner 2009
#

import sys

datafile = "earplug_data.txt"
if len(sys.argv) > 1:
    datafile = sys.argv[1]

src = open(datafile, "r")
dest = open("earplug_data.h", "w")

lines = src.readlines()

dest.write("/* default impulse responses as embedded binary data */\n")
dest.write("t_float earplug_impulses[368][2][128] = {\n")
for line in lines:
    if line[0] == "*":
        continue
    if line == '\n':
        continue
    else:
        irs = line.split(' ')
        irs.pop()

        count = 0
        leftSamples = []
        rightSamples = []
        for sample in irs:
            if count % 2 == 0:
                leftSamples.append(sample)
            else:
                rightSamples.append(sample)
            count += 1

        dest.write('{\n{')
        dest.write(', '.join(leftSamples))
        dest.write('},\n')

        dest.write('{')
        dest.write(', '.join(rightSamples))
        dest.write('}\n},\n')
dest.write('};\n')
src.close()
dest.close()
