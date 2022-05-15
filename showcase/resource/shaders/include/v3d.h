
/* set constants */
#define GLOBAL_SET 0 			// bound once [ Camera and Lights ]
#define RENDER_SET 1 			// bound for each render pass
#define SUB_RENDER_SET 2 		// bound for each sub pass
#define MATERIAL_SET 3 			// bound for each material [ textures, material properties ]
#define OBJECT_SET 4 			// bound for each object [ model and normal matrices ]

/* binding constants  */

// should be used with GLOBAL_SET
#define CAMERA_BINDING 0 		// binding for camera properties
#define LIGHT_BINDING 1 		// binding for light properties

// should be used with SUB_RENDER_SET
#define INPUT_ATTACHMENT_BINDING0 0
#define INPUT_ATTACHMENT_BINDING1 1
#define INPUT_ATTACHMENT_BINDING2 2


// should be used with MATERIAL_SET
#define MATERIAL_PROPERTIES_BINDING 0 	// binding for material properties

// should be used with MATERIAL_SET & RENDER_SET
#define TEXTURE_BINDING0 2 		// binding for texture unit 0
#define TEXTURE_BINDING1 3 		// binding for texture unit 1
#define TEXTURE_BINDING2 4 		// binding for texture unit 2
#define TEXTURE_BINDING3 5 		// binding for texture unit 3
#define TEXTURE_BINDING4 6 		// binding for texture unit 4
#define TEXTURE_BINDING5 7	 	// binding for texture unit 5
#define TEXTURE_BINDING6 8 		// binding for texture unit 6
#define TEXTURE_BINDING7 9		// binding for texture unit 7
#define TEXTURE_BINDING8 10 	// binding for texture unit 8

// should be used with OBJECT_SET
#define TRANSFORM_BINDING 0 	// binding for object transform matrices

/* location constants  */
#define POSITION_LOCATION  0
#define NORMAL_LOCATION 1
#define TEXCOORD_LOCATION 2
#define TANGENT_LOCATION 3


#define LightInfo \
LightInfo\
{\
	mat4 projectionMatrix;\
	mat4 viewMatrix;\
	vec3 dir;\
	vec3 color;\
	float intensity;\
}

#define ObjectInfo \
ObjectInfo\
{\
	mat4 modelMatrix;\
	mat4 normalMatrix;\
}

#define CameraInfo \
CameraInfo\
{\
	mat4 projectionMatrix;\
	mat4 viewMatrix;\
}
