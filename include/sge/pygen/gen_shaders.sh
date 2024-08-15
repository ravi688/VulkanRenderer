#!/usr/bin/bash

# TODO: move this to GNU makefile automaticaly generate shaders.h file whenever any of the following files changes.

python gen_shaders.py \
		../../../shaders/presets/solid_color.v3dshader \
		../../../sutk/fonts/Calibri\ Regular.ttf \
		../../../sutk/fonts/Roboto-Bold.ttf \
		../../../shaders/include/v3d.h \
		../../../shaders/builtins/bitmap_text_shader.v3dshader