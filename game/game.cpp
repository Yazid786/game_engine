#include "game.h"
#include "../base.cpp"

funcdef void
draw_quad(Draw_Data *data, vec2 min, vec2 max, color8 color)
{

	if (data->vertices.len + 4 > data->vertices.capacity ||
		data->indices.len + 6 > data->indices.capacity)
	{
		data->draw_proc(data);
	}

	u16 base = (u16)data->vertices.len;

	Vertex vertices[4] = {
		{ { min.x, min.y, 0.0f }, { 0.0f, 0.0f }, color },
		{ { max.x, min.y, 0.0f }, { 1.0f, 0.0f }, color },
		{ { max.x, max.y, 0.0f }, { 1.0f, 1.0f }, color },
		{ { min.x, max.y, 0.0f }, { 0.0f, 1.0f }, color },
	};

	u16 indices[6] = {
		(u16) (base + 0),
		(u16) (base + 1),
		(u16) (base + 2),

		(u16) (base + 2),
		(u16) (base + 3),
		(u16) (base + 0),
	};

	append_slice(&data->vertices, { vertices, 4 } );
	append_slice(&data->indices,  { indices, 6 } );
}


funcdef void
game_init(Game_State *state) {
}

funcdef void
game_update(Game_State *state, const OS_Input& input, Draw_Data *draw_data, f32 dt) {

	color8 col = Hex(0x00FF00FF);

	if (input.key_state[Key_A] & Key_Down) {

		col = Hex(0xFF0000FF);

	}

	draw_quad(draw_data, {-100,-100}, {100,100}, col);

}

export_fn Game_Code
game_get_code() {
	Game_Code result = {};
	result.init = game_init;
	result.update = game_update;
	return result;
}
