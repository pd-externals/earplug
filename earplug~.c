/* RT binaural filter: earplug~        */
/* based on KEMAR impulse measurement  */
/* Pei Xiang, summer 2004              */
/* Revised in fall 2006 by Jorge Castellanos */
/* Revised in spring 2009 by Hans-Christoph Steiner to compile in the data file */
/* Updated in 2020-2021 by Dan Wilcox & Chikashi Miyama */

#include "m_pd.h"
#include <math.h>
#include <string.h>
#include <errno.h>

/* impulse response data */
#ifdef EARPLUG_DATA_NO_EMBED
t_float earplug_impulses[368][2][128] = {{{0.0f}}};
#else
#include "earplug_data.h"
#endif

#define VERSION "0.3.0"

/* these pragmas only apply to Microsoft's compiler */
#ifdef _MSC_VER
#pragma warning( disable : 4244 ) /* uncast float/int conversion etc. */
#pragma warning( disable : 4305 ) /* uncast const double to float */
#endif

/* elevation degree:       -40  -30  -20  -10   0   10  20  30  40  50  60  70  80  90 */
/* index array:              0    1    2    3   4    5   6   7   8   9  10  11  12  13 */
/* impulse response number: 29   31   37   37  37   37  37  31  29  23  19  13   7   1 */ 
/* 0 degree response index:  0   29   60   97 134  171 208 245 276 305 328 347 360 367 */

static t_class *earplug_class;

typedef struct _earplug
{
    t_object x_obj; 
    t_outlet *left_channel;
    t_outlet *right_channel;

    t_float azi;
    t_float ele;
    unsigned ch_L;
    unsigned ch_R;
     
    t_float azimScale[13];
    unsigned int azimOffset[13];

    t_float ir[2][128];
    t_float convBuffer[128];
    t_float (*impulses)[2][128];     /* a 3D array of 368x2x128 */
    t_float f;                       /* dummy float for dsp */
    int bufferPin;
} t_earplug;

static t_int *earplug_perform(t_int *w)
{
    t_earplug *x = (t_earplug *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *right_out = (t_float *)(w[3]);
    t_float *left_out = (t_float *)(w[4]);
    int blocksize = (int)(w[5]);
    unsigned i;


    if (x->ele < 8.0) /* if elevation is less than 80 degrees... */
    { 
        /* a quantized version of the elevation */
        int elevInt = (int)floor(x->ele);
        /* used as the index to the array of scaling factors for the azimuth
           (adding 4 because the lowest elevation is -4, so it starts at 0) */
        unsigned elevGridIndex = elevInt + 4;
        unsigned azimIntUp = (unsigned)(x->azi * x->azimScale[elevGridIndex+1]);
        float azimFracUp = azimIntUp + 1.0 - x->azi * x->azimScale[elevGridIndex+1];
        float azimFracUpInv = 1.0 - azimFracUp;
        float elevFracUp = x->ele - elevInt * 1.0;
        unsigned azimIntDown = (unsigned)(x->azi * x->azimScale[elevGridIndex]);
        float azimFracDown = azimIntDown + 1.0 - x->azi * x->azimScale[elevGridIndex];
        float azimFracDownInv = 1.0 - azimFracDown;
        float elevFracDown = 1.0 - elevFracUp;
        unsigned lowerIdx = x->azimOffset[elevGridIndex] + azimIntDown;
        unsigned upperIdx = x->azimOffset[elevGridIndex + 1] + azimIntUp;
        
        for (i = 0; i < 128; i++)
        {
            /* elevFracDown: interpolate the lower two HRIRs and multiply them by their "fraction"
                 elevFracUp: interpolate the upper two HRIRs and multiply them by their "fraction" */
            x->ir[x->ch_L][i] = elevFracDown *
                                        (azimFracDown * x->impulses[lowerIdx][0][i] + 
                                        azimFracDownInv * x->impulses[lowerIdx + 1][0][i]) +
                                        elevFracUp *
                                        (azimFracUp * x->impulses[upperIdx][0][i] + 
                                        azimFracUpInv * x->impulses[upperIdx + 1][0][i]);

            x->ir[x->ch_R][i] = elevFracDown *
                                        (azimFracDown * x->impulses[lowerIdx][1][i] +
                                        azimFracDownInv * x->impulses[lowerIdx + 1][1][i]) +
                                        elevFracUp *
                                        (azimFracUp * x->impulses[upperIdx][1][i] +
                                        azimFracUpInv * x->impulses[upperIdx + 1][1][i]);
        }
    }
    else
    {
        /* if elevation is 80 degrees or more the interpolation requires only
           three points (because there's only one HRIR at 90 deg) */

        /* scale the azimuth to 12 (the number of HRIRs at 80 deg) discreet points */
        unsigned azimIntDown = (unsigned)(x->azi * 0.033333);
        float azimFracDown = azimIntDown + 1.0 - x->azi * 0.033333;
        float elevFracUp = x->ele - 8.0;
        float elevFracDown = 9.0 - x->ele;
        for (i = 0; i < 128; i++)
        {
            /* elevFracDown: these two lines interpolate the lower two HRIRs
                 elevFracUp: multiply the 90 degree HRIR with its corresponding fraction */
            x->ir[x->ch_L][i] = elevFracDown *
                                        (azimFracDown * x->impulses[360+azimIntDown][0][i] +
                                        (1.0 - azimFracDown) * x->impulses[361+azimIntDown][0][i])
                                        + elevFracUp * x->impulses[367][0][i];
            x->ir[x->ch_R][i] = elevFracDown *
                                        (azimFracDown * x->impulses[360+azimIntDown][1][i]  +
                                        (1.0 - azimFracDown) * x->impulses[361+azimIntDown][1][i])
                                        + elevFracUp * x->impulses[367][1][i]; 
        }
    }

    float inSample;
    float convSum[2]; /* to accumulate the sum during convolution */

    /* convolve the interpolated HRIRs (left and right) with the input signal */
    while (blocksize--)
    {
        convSum[0] = 0; 
        convSum[1] = 0; 

        inSample = *(in++);

        x->convBuffer[x->bufferPin] = inSample;
        for (i = 0; i < 128; i++)
        { 
            convSum[0] += x->ir[0][i] * x->convBuffer[(x->bufferPin - i) &127];
            convSum[1] += x->ir[1][i] * x->convBuffer[(x->bufferPin - i) &127];
        }   

        x->bufferPin = (x->bufferPin + 1) & 127;

        *left_out++ = convSum[0];
        *right_out++ = convSum[1];
    }
    return w + 6;
}

static void earplug_azimuth(t_earplug *x, float value) {
    if (value < 0 || value > 360)
        value = 0;
    if (value <= 180){
        x->ch_L = 0;
        x->ch_R = 1;
    }
    else{ 
        x->ch_L = 1;
        x->ch_R = 0;
        value = 360.0 - value;
    }
    x->azi = value;
}

static void earplug_elevation(t_earplug *x, float value) {

    if (value < -40)
        value = -40;
    if (value > 90)
        value = 90;
    /* divided by 10 since each elevation is 10 degrees apart */
    x->ele = value * 0.1;
}

static void earplug_dsp(t_earplug *x, t_signal **sp)
{
    /* callback, params, userdata, in_samples,   out_L,        out_R,        blocksize */
    dsp_add(earplug_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

static void *earplug_new(t_floatarg azimArg, t_floatarg elevArg)
{
    t_earplug *x = (t_earplug *)pd_new(earplug_class);
    x->left_channel = outlet_new(&x->x_obj, gensym("signal"));
    x->right_channel = outlet_new(&x->x_obj, gensym("signal"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("azimuth"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("elevation"));

    x->azi = azimArg;
    x->ele = elevArg;
    x->ch_L = 0;
    x->ch_R = 1;

    int i, j;
    FILE *fp;
    t_symbol *canvasdir = canvas_getdir(canvas_getcurrent());
    char buff[MAXPDSTRING], *bufptr;
    int filedesc;

    filedesc = open_via_path(canvasdir->s_name, "earplug_data.txt", "", buff, &bufptr, MAXPDSTRING, 0);
    if (filedesc >= 0) /* if there was no error opening the text file... */
    {
        int ret;
        fp = fdopen(filedesc, "r");
        for (i = 0; i < 368; i++) 
        {
            do {ret = fgetc(fp);}
            while (ret != 10 && ret != EOF);
            if (ret != EOF)
            {
                for (j = 0; j < 128; j++)
                {
                    ret = fscanf(fp, "%f %f ", &earplug_impulses[i][0][j],
                                               &earplug_impulses[i][1][j]);
                    if (ret == EOF) {break;}
                }
            }
            if (ret == EOF)
            {
                pd_error(x, "earplug~: could not load %s/earplug_data.txt, check format?", buff);
                break;
            }
        }
        fclose(fp);
        if (ret != EOF) {logpost(x, 3, "earplug~: loaded %s/earplug_data.txt", buff);}
    }
    x->impulses = earplug_impulses;

    for (i = 0; i < 128; i++)
         x->convBuffer[i] = 0.f; 
    x->bufferPin = 0;

    /* this is the scaling factor for the azimuth so that it
       corresponds to an HRTF in the KEMAR database */
    x->azimScale[0] = x->azimScale[8] = 0.153846153;   /* -40 and 40 degree */
    x->azimScale[1] = x->azimScale[7] = 0.166666666;   /* -30 and 30 degree */
    x->azimScale[2] = x->azimScale[3] = x->azimScale[4]
                    = x->azimScale[5] = x->azimScale[6] = 0.2; /* -20 to 20 degree */
    x->azimScale[9] = 0.125;        /* 50 degree */
    x->azimScale[10] = 0.1;         /* 60 degree */
    x->azimScale[11] = 0.066666666; /* 70 degree */
    x->azimScale[12] = 0.033333333; /* 80 degree */

    x->azimOffset[0] = 0; 
    x->azimOffset[1] = 29;
    x->azimOffset[2] = 60;
    x->azimOffset[3] = 97;
    x->azimOffset[4] = 134;
    x->azimOffset[5] = 171;
    x->azimOffset[6] = 208;
    x->azimOffset[7] = 245;
    x->azimOffset[8] = 276;
    x->azimOffset[9] = 305;
    x->azimOffset[10] = 328;
    x->azimOffset[11] = 347;
    x->azimOffset[12] = 360;

    return x;
}

void earplug_tilde_setup(void)
{
    earplug_class = class_new(gensym("earplug~"), (t_newmethod)earplug_new, 0,
        sizeof(t_earplug), CLASS_DEFAULT, A_DEFFLOAT, A_DEFFLOAT, 0);

    CLASS_MAINSIGNALIN(earplug_class, t_earplug, f);

    class_addmethod(earplug_class, (t_method)earplug_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod(earplug_class, (t_method)earplug_azimuth, gensym("azimuth"), A_FLOAT, 0);
    class_addmethod(earplug_class, (t_method)earplug_elevation, gensym("elevation"), A_FLOAT, 0);

    post("earplug~ %s: binaural filter with measured responses", VERSION);
    post("    elevation: -40 to 90 degrees, azimuth: 360 degrees");
    post("    do not use a blocksize > 8192");
}
