Shader
{
	[NoParse]
    VertexLayout
    {
    	per_vertex [POSITION_BINDING, POSITION_LOCATION] vec3 position;
    }

    RenderPass
    {
        SubPass
        {
        	[NoParse]
            GraphicsPipeline { depthStencil { depthTestEnable = true, depthWriteEnable = true } }

            [NoParse]
            glsl
            {
                #stage vertex

				#version 450
				
				#include <v3d.h>
				
				layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
				layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;
				
				
				layout(location = POSITION_LOCATION) in vec3 position;
				
				void main()
				{
					vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
					clipPos.y = -clipPos.y;
					gl_Position = clipPos;
				}

				#stage fragment

				#version 450
				
				void main() { }
            }
        }
    }
}
