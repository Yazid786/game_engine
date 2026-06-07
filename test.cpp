#include "engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"


funcdef Image png_to_bin_data(Arena* asset_arena,string path)
{
    int width;
    int height;
    int channels;

    printf("%s\n",path);
    bytes img_data = os_load_entire_file(asset_arena,path);
    printf("%u\n",img_data,img_data.raw);
    u8* img = stbi_load_from_memory(img_data.raw,img_data.len,&width,&height,&channels,0);
    if (!img)
    {
        printf("fuck you\n");
    }
    printf("width: %u\n",width);
    printf("Height: %u\n",height);
    printf("channel: %u\n",channels);

    bytes buffer = alloc_slice(asset_arena,u8,sizeof(width)+sizeof(height)+sizeof(channels)+(u64)width*height*channels);
    memcpy(buffer.raw,&width,sizeof(width));
    memcpy(buffer.raw+sizeof(width),&height,sizeof(height));
    memcpy(buffer.raw+sizeof(width)+sizeof(height),&channels,sizeof(channels));
    memcpy(buffer.raw+sizeof(width)+sizeof(height)+sizeof(channels),img,(u64)width*height*channels);
   
    for (int i = 0; i< buffer.len; i++)
    {
        printf("buffer: %u\n",buffer.raw[i]);
    }
    stbi_image_free(img);
    return {width,height,channels,buffer};
}