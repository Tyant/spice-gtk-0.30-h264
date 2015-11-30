#include <time.h>

#include "config.h"

#include "spice-client.h"
#include "spice-common.h"
#include "spice-channel-priv.h"

#include "channel-display-priv.h"

#define DEFAULT_MAX_FRAME_SIZE 1920 * 1080 * 4

void stream_h264_init(display_stream* st)
{
    SPICE_DEBUG("%s", __FUNCTION__);
    h264_decoder_env_init();

    st->h264_info = (H264StreamInfo*)malloc(sizeof(H264StreamInfo));
    if(st->h264_info == NULL)
    {
        printf("allocation memory for struct H264StreamInfo failed!\n");
        return;
    }    
    memset(st->h264_info, 0, sizeof(H264StreamInfo));
    st->h264_info->pix_fmt = AV_PIX_FMT_BGRA;                              //PIX_FMT_BGR24;

    st->h264_decoder = h264_decoder_new();
    if(st->h264_decoder == NULL)
    {
        printf("allocation memory for struct H264Decoder failed!\n");
        return;
    }
 
    int ret=  h264_decoder_init(st->h264_decoder, st->h264_info);
    if(H264_ERROR == ret)      
    {
        printf("init decoder failed!\n");
        return;
    } 
}

void stream_h264_data(display_stream* st, SpiceRect* rc)
{
    int width = 0;
    int height = 0;
    int hpp = 0;
    int ret = 0;
    
    char* rgb_frame = NULL;
    int rgb_frame_size = 0;

    char* h264_frame = NULL;
    int* h264_frame_size = NULL;
    
    SPICE_DEBUG("%s", __FUNCTION__);

    if(st->msg_data->psize <= 0)
    {
       printf("msg_data' s psize <= 0\n");
       return;   
    }

    h264_frame = (char*)(st->msg_data->data + 12);
    h264_frame_size =(int*)(st->msg_data->data + 8);   

    rgb_frame = (char*)g_malloc0(DEFAULT_MAX_FRAME_SIZE);
    ret = h264_decode(st->h264_decoder, rgb_frame, &rgb_frame_size, h264_frame, *h264_frame_size, &width, &height, &hpp);
    if(ret == 0)
    {    
      if(NULL != st->out_frame)
        g_free(st->out_frame);

     	unsigned ack_width = abs(rc->right - rc->left);
    	unsigned ack_height = abs(rc->top - rc->bottom);

      if(ack_width % 2 == 1)
     	{
         unsigned ack_row = 4 * ack_width;
     		unsigned row = 4 * width;
     
     		char* tmp_pic = (char*)g_malloc0(ack_row * ack_height);
     		for(unsigned i = 0;i < ack_height; i++)
     		{
     			memcpy(tmp_pic + i * ack_row, rgb_frame + i * row, ack_row);
     		}
     
     		st->out_frame = tmp_pic;
     		g_free(rgb_frame);       
    	}
     	else
     	{
     		 st->out_frame = (uint8_t*)rgb_frame;
      }
      
     } 
     else
     {
         g_free(rgb_frame);
     }
}

void stream_h264_cleanup(display_stream* st)
{
    SPICE_DEBUG("%s", __FUNCTION__);
    
    if(st->h264_decoder != NULL)
   {
       h264_decoder_uninit(st->h264_decoder);
       h264_decoder_release(st->h264_decoder);
       st->h264_decoder = NULL;
   }

   if(st->h264_info != NULL)
   {
      free(st->h264_info);
      st->h264_info = NULL;
   }
   
   if(st->out_frame != NULL)
   {
      g_free(st->out_frame);                                                                                                                         
      st->out_frame = NULL;
   }
}
