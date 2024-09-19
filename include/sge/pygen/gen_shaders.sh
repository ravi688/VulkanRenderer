#!/usr/bin/bash

# TODO: move this to GNU makefile automaticaly generate shaders.h file whenever any of the following files changes.

python $(pwd)/$(dirname "$0")/gen_shaders.py \
		../../../shaders/presets/image_rect.v3dshader \
		../../../shaders/presets/solid_color.v3dshader \
		../../../shaders/presets/solid_color_rect_array.v3dshader \
		../../../shaders/presets/no_standalone_use/sdf_template.template \
		../../../sutk/fonts/Calibri\ Regular.ttf \
		../../../sutk/fonts/Roboto-Bold.ttf \
		../../../textures/white_image.bmp \
		../../../shaders/include/v3d.h \
		../../../shaders/builtins/bitmap_text_shader.v3dshader