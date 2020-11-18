# earplug~

RT binaural filter (HRTF) based on KEMAR impulse measurement

Pei Xiang, summer 2004  
Revised in fall 2006 by Jorge Castellanos  
Revised in spring 2009 by Hans-Christoph Steiner to compile in the data file  
Brought to GitHub in fall 2020 by Dan Wilcox

## Build

This is a pure C external and can be compiled simply with:

    make

## Usage

To load this external in Pd, include the directory containing the following files
in your Pd search paths:

* earplug~.pd_*: compiled external
* earplug~-help.pd: help file
* earplug_data.txt: KEMAR data set

### HRIR Data Set

By default, earplug~ will load the HRTF data set found in earplug_data.txt
located in the same directory as the external or the calling patch. If this file
is not found, earplug~ will use an internal copy compiled into the object. This
data set basically describes the "ear model" that is used by the convolution
algorithm to filter the audio.

To use your own ear model, you need an HRTF data set prepared as a 3D array of
368x2x128 float values. See the format of earplug_data.txt as basis for making
your own file and is based on the compact set of measurements of the KEMAR
Dummy-Head Microphone from https://sound.media.mit.edu/resources/KEMAR.html.

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
>> or other externals which work with spcacialisation
>> of sounds,
>> preferably for headphones.
>>
>> thanks
>> marc
