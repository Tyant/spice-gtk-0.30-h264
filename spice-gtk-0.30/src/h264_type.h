#ifndef h264_type_h
#define h264_type_h

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include "libswscale/swscale.h"


#define H264_ENCODER_ERROR -1
#define H264_NOT_GET_PIC -2
#define H264_NOT_GET_SLICE -3
#define H264_DECODER_ERROR -4

#define H264_ENCODER_OK 0
#define H264_DECODER_OK 0

#define H264_OK 1
#define H264_ERROR 0

#define H264_VERSION 2015.1.1 



#define H264_ENV_INIT(version) do{                            \
            /*LOG version*/                                      \
            av_register_all();                                  \
            avcodec_register_all();                             \
                                      }while(0)


#define GET_FORMAT_BITS(fmt,bits) do {              \
   switch (fmt)                                      \
   {                                                 \
      case AV_PIX_FMT_BGR24:                         \
      case AV_PIX_FMT_RGB24:                         \
         bits = 3 * 8;                                  \
         break;                                      \
      case AV_PIX_FMT_ARGB:                          \
      case AV_PIX_FMT_RGBA:                          \
      case AV_PIX_FMT_ABGR:                          \
      case AV_PIX_FMT_BGRA:                          \
         bits = 4 * 8;                                  \
         break;                                      \
      case AV_PIX_FMT_RGB565:                        \
      case AV_PIX_FMT_RGB555:                        \
      case AV_PIX_FMT_BGR565:                        \
      case AV_PIX_FMT_BGR555:                        \
         bits = 2 * 8;                                  \
         break;                                      \
      default:                                       \
         bits = 0;                                  \
   }                                                 \
}while(0)

#define GET_FORMAT_BYTES(fmt,bytes) do {              \
   switch (fmt)                                      \
   {                                                 \
      case AV_PIX_FMT_BGR24:                         \
      case AV_PIX_FMT_RGB24:                         \
         bytes = 3;                                  \
         break;                                      \
      case AV_PIX_FMT_ARGB:                          \
      case AV_PIX_FMT_RGBA:                          \
      case AV_PIX_FMT_ABGR:                          \
      case AV_PIX_FMT_BGRA:                          \
         bytes = 4;                                  \
         break;                                      \
      case AV_PIX_FMT_RGB565:                        \
      case AV_PIX_FMT_RGB555:                        \
      case AV_PIX_FMT_BGR565:                        \
      case AV_PIX_FMT_BGR555:                        \
         bytes = 2;                                  \
         break;                                      \
      default:                                       \
         bytes = 0;                                  \
   }                                                 \
}while(0)



typedef struct _H264StreamInfo
{
    int bit_rate;
    int rc_max_rate;
    int rc_min_rate;
    int rc_buffer_size;
    int width;
    int height;
    AVRational time_base;
    int gop_size;
    int max_b_frames;
    int thread_count;
    int profile;
    enum AVPixelFormat pix_fmt;
    enum AVPixelFormat s_pix_fmt;
    int qmin;
    int qmax;
    float qcompress;  

}H264StreamInfo;

#endif
