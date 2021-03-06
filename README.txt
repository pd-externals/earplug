# earplug~

Realtime binaural filter (HRTF) based on KEMAR impulse measurement

Pei Xiang, summer 2004  
Revised in fall 2006 by Jorge Castellanos  
Revised in spring 2009 by Hans-Christoph Steiner to compile in the data file  
Updated in 2020-2021 by Dan Wilcox & Chikashi Miyama

GPL v2

## Description

earplug~ is a binaural filter based on KEMAR impulse measurement which allows
you to spatialize a sound in realtime. It basically takes the KEMAR dummy head
data set, and interpolates 368 locations where HRTF measurement exists in a
spherical surface. You get azimuth 0 - 360 degrees and elevation -40 - 90
degrees.

## Build

This is a pure C external and can be compiled simply with:

    make

## Usage

To load this external in Pd, include the directory containing the following
files in your Pd search paths:

* earplug~.pd_*: compiled external
* earplug~-help.pd: help file
* earplug_data.txt: KEMAR data set (optional, see below)

### HRIR Data Set

On creation, earplug~ will try to load an HRTF impulse response data set found
in a "earplug_data.txt" file located in the same directory as the calling
patch. If this file is not found, earplug~ will use an default data set compiled
into the object. This impulse response set basically describes the "ear model"
that is used by the convolution algorithm to filter the audio.

To use your own ear model, you need an HRTF data set prepared as a 3D array of
368x2x128 float values. See the format of earplug_data.txt as a basis for
making your own file and is based on the compact set of measurements of the
KEMAR dummy head microphone from:

    https://sound.media.mit.edu/resources/KEMAR.html

To embed a custom data set, use the parse-to-h.py Python script with your IR
data which will overwrite the default binary data in earplug_data.h:

    ./parse-to-h.py custom_data.txt

To disable compiling the default data set into the external, define
EARPLUG_NO_DATA_EMBED:

    make CFLAGS="-DEARPLUG_DATA_NO_EMBED"

Note: If the default data set is not embedded, an "earplug_data.txt" file is
required, otherwise the external will output silence.

## Background

http://markmail.org/message/sxfauaymftshbgwz  
http://lists.puredata.info/pipermail/pd-list/2005-02/025764.html

On Jan 31, 2005, at 5:05 PM, Pei Xiang wrote:

> hey, marc,
> actually i've just written one external that handles this, [earplug~]
> http://crca.ucsd.edu/~pxiang/research.htm it basically takes the KEMAR data
> set, and interpolates 366 locations where HRTF measurement exists in a
> spherical surface. you get azimuth control 0-360 and elevation -40 - 90.
>
> now i'm still trying to clean it up a little bit, but probably have to use by
> copy the data.txt file into the default search dir of Pd, for the moment.
>
> cheers, Pei 
>
>
> --- metafor <metafor at gmx.net> wrote:
>
>> just wondering, is there a puredata object or
>> abstraction
>> existing for the "head  related transfer function"?
>>
>> or other externals which work with spacialisation
>> of sounds,
>> preferably for headphones.
>>
>> thanks
>> marc
