#ifndef h264_decoder_h
#define h264_decoder_h
#include <math.h>
#include <assert.h>

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include "libswscale/swscale.h"


#include "h264_type.h"




typedef struct _H264Decoder
{
   AVCodecContext *codec_context;
   struct SwsContext *sws_context;
   AVFrame *yuvframe;
   AVFrame *rgbframe;
}H264Decoder;


void  h264_decoder_env_init();

int h264_decoder_init(H264Decoder * decoder, H264StreamInfo * info);
void h264_decoder_uninit(H264Decoder * decoder);

H264Decoder* h264_decoder_new();
void h264_decoder_release(H264Decoder * decoder);

int h264_decode(H264Decoder * decoder,char * rgb,int *rgb_len, char *slice ,int slice_len,int *width,int *height,int *hpp);
#endif
