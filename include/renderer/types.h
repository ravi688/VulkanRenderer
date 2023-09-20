#pragma once

typedef enum text_render_space_type_t
{
	/* selects the 'screen projection matrix' from the camera's projection matrices */
	TEXT_RENDER_SPACE_TYPE_2D,
	/* selects the 'orthographic/perspective matrix' from the camera's projection matrices */
	TEXT_RENDER_SPACE_TYPE_3D
} text_render_space_type_t;


