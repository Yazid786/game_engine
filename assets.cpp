#include "engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#if DEBUG_BUILD

typedef u64 Asset_TimeStamp;
struct Asset_Record {
	OS_FileKind     kind;
	Asset_TimeStamp last_edit;
};

funcdef string asset__get_path(Asset a) 
{
	switch (a) {
		case Asset::Sprite_Shader : return S("./shaders/sprite.glsl");
		case Asset::Screen_Shader : return S("./shaders/screen.glsl");
		default: return S("");
	}
}

#endif

global struct 
{
#if DEBUG_BUILD
	Asset_Record records[(u64) Asset::Count];
#endif

	list<Sprite> sprites;
} asset_ctx;


funcdef void
assets_init()
{

#if DEBUG_BUILD
	// fill out asset record ( debug info )
	u64 asset_count = (u64) Asset::Count;
	for (u64 i=0; i<asset_count; ++i) {
		Asset asset = (Asset) i;

		string path = asset__get_path(asset);
		if (!path.len)
			continue;

		OS_FileData data = os_file_data(path);
		assert(data.flags & File_Exists);

		Asset_Record rec = {
			data.kind,
			data.timestamp,
		};

		asset_ctx.records[i] = rec;
	}
#endif
}

funcdef slice<string>
asset_fetch_shader_source(Arena *arena, Asset shader)
{
#if DEBUG_BUILD
    assert(shader > Asset::Shader_Begin && shader < Asset::Shader_End);

    string path = asset__get_path(shader);
    string full_file = string_from_bytes(os_load_entire_file(arena, path));

    s64 v_index = -1;
    s64 f_index = -1;

    for (u64 i = 0; i + 4 <= full_file.len; i++) {
        if (v_index < 0 &&
            memcmp(full_file.raw + i, "#vs\n", 4) == 0) {
            v_index = i;
        }

        if (f_index < 0 &&
            memcmp(full_file.raw + i, "#fs\n", 4) == 0) {
            f_index = i;
        }

        if (v_index >= 0 && f_index >= 0)
            break;
    }

    if (v_index < 0 || f_index < 0) {
        return {};
    }

    s64 i0 = Min(v_index, f_index);
    s64 i1 = Max(v_index, f_index);

    u64 v_slice = (i0 == f_index) ? 1 : 0;


	slice<string> result = alloc_slice(arena, string, 2);
    result[v_slice] = full_file.range(i0 + 4, i1);
    result[1-v_slice] = full_file.range(i1 + 4, full_file.len);
	return result;
#else
# error "asset_fetch_shader_source implementation missing"
#endif
}

//////////////
// ~reehan @NOTE: image to binary

funcdef Image 
asset_fetch_image(Arena *arena, Asset texture) 
{
#if DEBUG_BUILD
    assert(texture > Asset::Texture_Begin && texture < Asset::Texture_End);

    int width;
    int height;
    int channels;
    
    Temp t = temp_begin(scratch(&arena, 1));
    defer(temp_end(t));

	string path = asset__get_path(texture);
    string cpath = string_to_cstring(t.arena,path);

    u8 *data = stbi_load((char*)cpath.raw,&width,&height,&channels,0);
	defer(stbi_image_free(data));

    if (!data){
        return Image{};
    }
    
    u64 buffer_size= (u64) width * height * channels;
    Image img= {} ;

    img.data = alloc_slice(arena,u8,buffer_size);
    memcpy(img.data.raw,data,buffer_size);

    img.width=width;
    img.height=height;
    img.channels=channels;
    
    return img;
#else
# error "asset_fetch_image implementation missing"
#endif
}


funcdef bool
game_load_code(Game_Code *code, string path)
{
	code->handle = os_load_library(path);

	if (!code->handle.v)
		return false;

	OS_FileData file_data = os_file_data(path);
	code->timestamp = file_data.timestamp;

	Load_Proc proc  = (Load_Proc) os_load_symbol(code->handle, "game_get_code");
	if (!proc) {
		game_unload_code(code);
		return false;
	}

	Game_Code new_code = proc();

	code->init = new_code.init;
	code->update = new_code.update;

	return true;
}

funcdef void
game_unload_code(Game_Code *code)
{
	os_unload_library(code->handle);
	memset(code, 0x0, sizeof(Game_Code));
}
