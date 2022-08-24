Shader
{
	[NoParse]
	Properties
	{
		fragment [MATERIAL_SET, MATERIAL_PROPERTIES_BINDING] uniform Parameters
		{
			vec4 color;
			float reflectance;
		} parameters;

		fragment [MATERIAL_SET, TEXTURE_BINDING0] uniform samplerCube reflectionMap;
	}

	[NoParse]
	VertexLayout
	{
		per_vertex [POSITION_BINDING, POSITION_LOCATION] vec3 position;
		per_vertex [NORMAL_BINDING, NORMAL_LOCATION] vec3 normal;
	}

	RenderPass
	{
		Subpass
		{
			[NoParse]
			GraphicsPipeline
			{
				colorBlend { attachment { } }
				depthStencil { depthTestEnable = true, depthWriteEnable = true }
			}
		}

		[NoParse]
		glsl
		{
			#stage vertex

			#version 450
			
			#include <v3d.h>
			
			layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
			layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;
			
			
			layout(location = POSITION_LOCATION) in vec3 position;
			layout(location = NORMAL_LOCATION) in vec3 normal;
			
			layout(location = 0) out vec3 _normal;
			layout(location = 1) out vec3 _position;
			
			void main()
			{
				_position = normalize((objectInfo.normal * vec4(position, 0)).xyz);
				vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
				clipPos.y = -clipPos.y;
				gl_Position = clipPos;
				_normal = normalize((objectInfo.normal * vec4(normal, 0)).xyz);
			}

			#stage fragment

			#version 450
			
			#include <v3d.h>
			
			layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight lightInfo;
			layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo camera;
			
			layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform samplerCube reflectionMap;
			
			layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
			{
				vec4 color;
				float reflectance;
			} parameters;
			
			
			layout(location = 0) in vec3 normal;
			layout(location = 1) in vec3 position;
			
			layout(location = 0) out vec4 color;
			
			
			float linearize(float depth, float n, float f)
			{
				float t = ((n - f) * depth + f) * (f - n);
				return n * f * 1 / t;
			}
			
			
			void main()
			{
				mat4 camera_transform = camera.transform;
				vec3 camera_position = vec3(camera_transform[3][0], camera_transform[3][1], camera_transform[3][2]);
			
				vec3 eye_vector = normalize(camera_position - position);
			
				vec3 reflection_vector = 2 * dot(eye_vector, normal) * normal - eye_vector;
			
				float depth = texture(reflectionMap, vec3(position.z, position.y, position.x)).r;
				depth = linearize(depth, 0.04, 20.0);
				vec4 reflection_color = mix(vec4(depth, depth, depth, 1), vec4(1, 1, 1, 1), 1 - parameters.reflectance);
			
				vec4 specular_color = mix(parameters.color, vec4(1, 1, 1, 1), 0.5 * dot(normalize(reflection_vector), normal) + 0.5);
			
				float dp = 1.0; //0.5 * dot(-normal, lightInfo.direction) + 0.5;
				color = dp * specular_color * vec4(lightInfo.color, 1) * reflection_color;
			}
		}
	}
}
