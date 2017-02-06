// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <shared.h>

#define CLASS_TYPE  LUAOBJ_TYPE_CANVAS
#define CLASS_NAME  "Canvas"

#define DISPLAY_TRANSFER_FLAGS (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

const char * canvasInit(love_canvas *self, int width, int height) {
    
	self->width = width;
	self->height = height;
	
	self->target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);

	C3D_RenderTargetSetOutput(self->target, sf2d_get_current_screen(), GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	return NULL;

}

int canvasNew(lua_State *L) {

	int width = luaL_optnumber(L, 1, 400);
	int height = luaL_optnumber(L, 2, 240);

	love_canvas *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = canvasInit(self, width, height);

	if (error) {
		luaError(L, error);
	}

	return 1;

}

int canvasRenderStart(love_canvas * canvas) {
    C3D_RenderTargetSetClear(canvas->target, 0, 0, 0); // don't clear again until marked to
	
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	C3D_FrameDrawOn(canvas->target);
	
	return 0;
}

int canvasRenderEnd() {
	C3D_FrameEnd(0);
	
	return 0;
}

int canvasGC(lua_State *L) { // Garbage Collection

	love_canvas *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self->target) return 0;

	C3D_RenderTargetDelete(self->target);
	self->target = NULL;

	return 0;

}

int canvasClear(lua_State * L) {

    love_canvas * self = luaobj_checkudata(L, 1, CLASS_TYPE);

    C3D_RenderTargetSetClear(self->target, 0, 0, 0); // don't clear again until marked to

    return 0;

}


int initCanvasClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",			canvasNew	},
		{"__gc",		canvasGC	},
		{"clear",		canvasClear },
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, canvasNew, reg);

	return 1;

}