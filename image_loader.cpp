#include "engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"


funcdef Image 
png_to_binary_data(Arena *arena, string path) 
{   
    int width;
    int height;
    int channels;
    
    Temp t = {};
    defer(temp_end(t));

    string cpath=string_to_cstring(scratch(&t),path);
    u8* data=stbi_load((char*)cpath.raw,&width,&height,&channels,0);  //0 to keep original or 4 to force RGBA

    if (!data){
        return Image{};
    }
    
    u64 buffer_size=(u64)width*height*channels;
    Image img={};

    img.data=alloc_slice(arena,u8,buffer_size);
    memcpy(img.data.raw,data,buffer_size);
    img.width=width;
    img.height=height;
    img.channels=channels;
    
    stbi_image_free(data);
    
    return img;
    
}

