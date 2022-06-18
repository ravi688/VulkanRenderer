
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
#define TEXCOORD_LOCATION 2
#define TANGENT_LOCATION 3


#define POSITION layout(location = POSITION_LOCATION) in vec3
#define NORMAL layout(location = NORMAL_LOCATION) in vec3
#define TEXCOORD layout(location = TEXCOORD_LOCATION) in vec2
#define TANGENT layout(location = TANGENT_LOCATION) in vec3

#define LightInfo \
LightInfo\
{\
	mat4 transform;\
	mat4 projection;\
	mat4 view;\
	vec3 dir;\
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



#define CAMERA layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo
#define LIGHT layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo
#define POINT_LIGHT layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight
#define MATERIAL_PROPERTIES layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Properties
#define OBJECT layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo
