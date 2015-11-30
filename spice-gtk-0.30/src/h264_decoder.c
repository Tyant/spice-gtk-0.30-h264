#include "h264_decoder.h"

void  h264_decoder_env_init()
{
    H264_ENV_INIT(H264_VERSION);
}

H264Decoder* h264_decoder_new()
{
    H264Decoder *decoder = NULL;
    decoder = (H264Decoder *)malloc(sizeof(H264Decoder));
    if(decoder)
    {
        memset(decoder,0,sizeof(H264Decoder));
    }
    return decoder;
}

void h264_decoder_release(H264Decoder * decoder)
{
   if(decoder != NULL)
   {
      free(decoder);
      decoder = NULL;
   }
}


int h264_decoder_init(H264Decoder * decoder, H264StreamInfo * info)
{
    assert(decoder != NULL);
    assert(info != NULL);
    AVCodec *codec = NULL;
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) 
    {
        goto last_err; 
    }
    decoder->codec_context = avcodec_alloc_context3(codec);
    if(!decoder->codec_context)
    {
        goto last_err;
    }
    if (codec->capabilities&CODEC_CAP_TRUNCATED)
        decoder->codec_context->flags |= CODEC_FLAG_TRUNCATED;

    if (avcodec_open2(decoder->codec_context, codec, NULL) < 0) 
    {
        goto codec_open_err;
    }
    decoder->yuvframe = av_frame_alloc();
    if(!decoder->yuvframe)
    {
        goto do_err;
    
    }
    decoder->rgbframe = av_frame_alloc();
    if(!decoder->rgbframe)
    {
        goto do_err;
    }

    decoder->rgbframe->format = info->pix_fmt;

    return H264_OK;
do_err:
    if (decoder->yuvframe)
    {
        av_frame_free(&decoder->yuvframe);
    }
    if (decoder->rgbframe)
    {
        av_frame_free(&decoder->rgbframe);
    }

    if(!decoder->codec_context)
    {
        avcodec_close(decoder->codec_context);
    }
codec_open_err:
    
    if(!decoder->codec_context)
    {
        av_free(decoder->codec_context);
    }
last_err:
    decoder->yuvframe = NULL;
    decoder->rgbframe = NULL;
    decoder->sws_context = NULL;
    decoder->codec_context = NULL;
    
    return H264_ERROR;
}

void h264_decoder_uninit(H264Decoder * decoder)
{
    if(decoder->sws_context)
    {
        sws_freeContext(decoder->sws_context);
        decoder->sws_context = NULL;
    }
    
    if(decoder->codec_context)
    {
        avcodec_close(decoder->codec_context);
        av_free(decoder->codec_context);
        decoder->codec_context = NULL;
    }
    
    if (decoder->yuvframe)
    {
        av_frame_free(&decoder->yuvframe);
        decoder->yuvframe = NULL;
    }
    
    if (decoder->rgbframe)
    {
        av_frame_free(&decoder->rgbframe);
        decoder->rgbframe = NULL;
    }
}

int h264_decode(H264Decoder * decoder,char * rgb,int *rgb_len, char *slice ,int slice_len,int *width,int *height,int *hpp)
{
    int ret = 0;
    int got = 0;
    int bytes = 0;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = slice;
    pkt.size = slice_len;
    
    ret = avcodec_decode_video2(decoder->codec_context, decoder->yuvframe, &got, &pkt);
    if(ret < 0)
    {
        return  H264_DECODER_ERROR;
    }
    if (got)
    {
        //printf("decoding length = %d, package length = %d\n", ret, slice_len);
    
        if (!decoder->sws_context)
        {
            decoder->sws_context = sws_getContext(decoder->codec_context->width, decoder->codec_context->height, 
                                    decoder->codec_context->pix_fmt,decoder->codec_context->width, decoder->codec_context->height,
                                    decoder->rgbframe->format , SWS_BICUBIC, NULL, NULL, NULL);
        }
        /*
        if(!rgbframe->data[0])
        {
            av_image_alloc(decoder->rgbframe->data, decoder->rgbframe->linesize, decoder->codec_context->width,
                                    decoder->codec_context->height, AV_PIX_FMT_BGR24, 24);
        }
        */
        decoder->rgbframe->width = decoder->codec_context->width;
        decoder->rgbframe->height = decoder->codec_context->height;

        avpicture_fill((AVPicture*)decoder->rgbframe, rgb, decoder->rgbframe->format, decoder->codec_context->width, 
                decoder->codec_context->height);

        sws_scale(decoder->sws_context, (const uint8_t * const*)decoder->yuvframe->data, decoder->yuvframe->linesize, 0,
                            decoder->codec_context->height, decoder->rgbframe->data, decoder->rgbframe->linesize);
        GET_FORMAT_BITS(decoder->rgbframe->format,bytes) ; 
        *rgb_len = decoder->rgbframe->width * decoder->rgbframe->height * bytes;
        *width = decoder->rgbframe->width;
        *height = decoder->rgbframe->height;
        *hpp = bytes;

        return H264_DECODER_OK; 

    }
    return H264_NOT_GET_PIC;
}
