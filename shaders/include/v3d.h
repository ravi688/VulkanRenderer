/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: v3d.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

#define SGE_UNIFORM_BUFFER_LAYOUT std140
#define SGE_STORAGE_BUFFER_LAYOUT std430

#define TEXT_RENDER_SPACE_TYPE_2D 0
#define TEXT_RENDER_SPACE_TYPE_3D 1

#define TEXT_RENDER_SURFACE_TYPE_CAMERA 0
#define TEXT_RENDER_SURFACE_TYPE_SCREEN 1


/* set constants */
#define CAMERA_SET 0 			// bound for each Camera
#define GLOBAL_SET 1 			// bound once [ Camera and Lights ]
#define RENDER_SET 2 			// bound for each render pass
#define SUB_RENDER_SET 3 		// bound for each sub pass
#define MATERIAL_SET 4 			// bound for each material [ textures, material properties ]
#define OBJECT_SET 5 			// bound for each object [ model and normal matrices ]

/* binding constants  */

// should be used with CAMERA_SET
#define CAMERA_PROPERTIES_BINDING 0 // binding for current camera properties

// should be used with GLOBAL_SET
#define CAMERA_BINDING 0 		// binding for camera properties
#define LIGHT_BINDING 1 		// binding for light properties
#define SCREEN_BINDING 2

// should be used with SUB_RENDER_SET
#define INPUT_ATTACHMENT_BINDING0 0
#define INPUT_ATTACHMENT_BINDING1 1
#define INPUT_ATTACHMENT_BINDING2 2


// should be used with MATERIAL_SET
#define MATERIAL_PROPERTIES_BINDING 0 	// binding for material properties

// should be used with MATERIAL_SET & RENDER_SET
#define TEXTURE_BINDING0 1 		// binding for texture unit 0
#define TEXTURE_BINDING1 2 		// binding for texture unit 1
#define TEXTURE_BINDING2 3 		// binding for texture unit 2
#define TEXTURE_BINDING3 4 		// binding for texture unit 3
#define TEXTURE_BINDING4 5 		// binding for texture unit 4
#define TEXTURE_BINDING5 6	 	// binding for texture unit 5
#define TEXTURE_BINDING6 7 		// binding for texture unit 6
#define TEXTURE_BINDING7 8		// binding for texture unit 7
#define TEXTURE_BINDING8 9 		// binding for texture unit 8

// should be used with OBJECT_SET
#define TRANSFORM_BINDING 0 	// binding for object transform matrices

/* location constants  */
#define POSITION_LOCATION  0
#define NORMAL_LOCATION 1
#define COLOR_LOCATION 2
#define TEXCOORD_LOCATION 3
#define TANGENT_LOCATION 4

/* <begin> for Vertex Attributes */
/* NOTE: vertex attributes are not interleaved so we need to store them in separate buffers */
#define VTX_POS_BIN 0 /* binding of the position attribute */
#define VTX_NML_BIN 1 /* binding of the normal attribute */
#define VTX_TXC_BIN 2 /* binding of the texture coordinate attribute */
#define VTX_TNG_BIN 3 /* binding of the tangent attribute */

#define VTX_POS_LOC 0 /* location of the position attribute */
#define VTX_NML_LOC 1 /* location of the normal attribute */
#define VTX_TXC_LOC 2 /* location of the texture coordinate attribute */
#define VTX_TNG_LOC 3 /* location of the tangent attribute */
/* <end> for Vertex Attributes */

/* <begin> bitmap text shader  */
#	define BTM_TXT_BND 4
	/* <begin> per instance of the glyphs */
# 	define BTM_TXT_TLTC_BND BTM_TXT_BND /* binding of top left texture coordinate of the quad */
# 	define BTM_TXT_TRTC_BND BTM_TXT_BND /* binding of top right texture coordinate of the quad */
# 	define BTM_TXT_BRTC_BND BTM_TXT_BND /* binding of bottom right texture coordinate of the quad */
# 	define BTM_TXT_BLTC_BND BTM_TXT_BND /* binding of bottom left texture coordinate of the quad */
#	define BTM_TXT_OFST_BND BTM_TXT_BND /* binding of offset of the quad (vec3) */
#	define BTM_TXT_XTNT_BND BTM_TXT_BND /* binding of extent of the quad (vec2) */
#	define BTM_TXT_ROTN_BND BTM_TXT_BND /* binding of rotation of the quad (vec3) */
#	define BTM_TXT_STID_BND BTM_TXT_BND /* binding of string id of the quad (to which it belongs) (uint) */
# 	define BTM_TXT_TLTC_LOC 4 /* location of top left texture coordinate of the quad */
# 	define BTM_TXT_TRTC_LOC 4 /* location of top right texture coordinate of the quad */
# 	define BTM_TXT_BRTC_LOC 5 /* location of bottom right texture coordinate of the quad */
# 	define BTM_TXT_BLTC_LOC 5 /* location of bottom left texture coordinate of the quad */
#	define BTM_TXT_OFST_LOC 6 /* location of offset of the quad (vec3) */
#	define BTM_TXT_XTNT_LOC 7 /* location of extent of the quad (vec2) */
#	define BTM_TXT_ROTN_LOC 8 /* location of rotation of the quad (vec3) */
#	define BTM_TXT_STID_LOC 8 /* location of string id of the quad (uint) */
# 	define BTM_TXT_TLTC_COMP 0 /* component of top left texture coordinate of the quad */
# 	define BTM_TXT_TRTC_COMP 2 /* component of top right texture coordinate of the quad */
# 	define BTM_TXT_BRTC_COMP 0 /* component of bottom right texture coordinate of the quad */
# 	define BTM_TXT_BLTC_COMP 2 /* component of bottom left texture coordinate of the quad */
#	define BTM_TXT_OFST_COMP 0 /* component of offset of the quad */
#	define BTM_TXT_XTNT_COMP 0 /* component of extent of the quad */
#	define BTM_TXT_ROTN_COMP 0 /* component of rotation of the quad */
#	define BTM_TXT_STID_COMP 3 /* component of string id of the quad */
	/* <end> per instance of the glyphs */
	/* <begin> per string object */
#	define BTM_TXT_TNFM_SET OBJECT_SET /* set number of the list of transform matrices */
#	define BTM_TXT_TNFM_BND (TRANSFORM_BINDING + 1) /* binding of the list of transform matrices */
	/* <end> per string object */
/* <end> bitmap text shader */

/* <begin> mesh text shader */
#	define MSH_TXT_BND 5
	/* <begin> per instance of the glyph */
#	define MSH_TXT_OFST_BND MSH_TXT_BND /* binding of offset of the instanced mesh of the glyph */
#	define MSH_TXT_SCAL_BND MSH_TXT_BND /* binding of scale of the instanced mesh of the glyph */
#	define MSH_TXT_ROTN_BND MSH_TXT_BND /* binding of rotation of the instanced mesh of the glyph */
#	define MSH_TXT_STID_BND MSH_TXT_BND /* binding of string id of the instanced mesh of the glyph */
#	define MSH_TXT_OFST_LOC 4 /* location of offset of the instanced mesh (vec3) */
#	define MSH_TXT_SCAL_LOC 5 /* location of scale of the instanced mesh (vec3) */
#	define MSH_TXT_ROTN_LOC 6 /* location of rotation of the instanced mesh (vec3) */
#	define MSH_TXT_STID_LOC 6 /* location of string id of the instanced mesh (to which it belongs) (uint) */
#	define MSH_TXT_OFST_COMP 0 /* component of offset of the instanced mesh  */
#	define MSH_TXT_SCAL_COMP 0 /* component of scale of the instanced mesh */
#	define MSH_TXT_ROTN_COMP 0 /* component of rotation of the instanced mesh */
#	define MSH_TXT_STID_COMP 3 /* component of string id of the instanced mesh */
	/* <end> per instance of the glyph */
	/* <begin> per string object */
#	define MSH_TXT_TNFM_SET OBJECT_SET /* set number of the list of transform matrices */
#	define MSH_TXT_TNFM_BND (TRANSFORM_BINDING + 1) /* binding of the list of transform matrices */
	/* <end> per string object */
/* <end> mesh text shader */

#define POSITION layout(location = POSITION_LOCATION) in vec3
#define NORMAL layout(location = NORMAL_LOCATION) in vec3
#define TEXCOORD layout(location = TEXCOORD_LOCATION) in vec2
#define TANGENT layout(location = TANGENT_LOCATION) in vec3

#define Light \
Light\
{\
	mat4 projection;\
	mat4 view;\
	vec3 color;\
	float intensity;\
}

#define DirectionalLight \
DirectionalLight\
{\
	mat4 projection;\
	mat4 view;\
	vec3 color;\
	float intensity;\
	vec3 direction;\
}

#define PointLight \
PointLight\
{\
	mat4 projection;\
	mat4 view;\
	vec3 color;\
	float intensity;\
	vec3 position;\
}

#define SpotLight \
SpotLight\
{\
	mat4 projection;\
	mat4 view;\
	vec3 color;\
	float intensity;\
	vec3 direction;\
	vec3 position;\
	float angle;\
}

#define ObjectInfo \
ObjectInfo\
{\
	mat4 transform;\
	mat4 normal;\
}

#define CameraInfo \
CameraInfo\
{\
	mat4 transform;\
	mat4 projection;\
	mat4 view;\
	mat4 screen;\
}



#define CAMERA layout(SGE_UNIFORM_BUFFER_LAYOUT, set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo
#define LIGHT layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform Light
#define DIRECTIONAL_LIGHT layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight
#define POINT_LIGHT layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight
#define SPOT_LIGHT layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform SpotLight
#define MATERIAL_PROPERTIES layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Properties
#define OBJECT layout(SGE_UNIFORM_BUFFER_LAYOUT, set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo
