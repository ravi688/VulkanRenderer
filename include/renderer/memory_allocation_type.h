#pragma once

/* type of the memory allocation */
typedef enum memory_allocation_type_t
{
    MEMORY_ALLOCATION_TYPE_VKAPI,

    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_INSTANCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DEVICE_QUEUE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DEVICE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FENCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SEMAPHORE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EVENT,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_QUERY_POOL,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_BUFFER,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_BUFFER_VIEW,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SHADER_MODULE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_CACHE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_SHADER_STAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_VERTEX_INPUT_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_INPUT_ASSEMBLY_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_TESSELLATION_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_VIEWPORT_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_RASTERIZATION_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_MULTISAMPLE_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_DEPTH_STENCIL_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_COLOR_BLEND_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_DYNAMIC_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_GRAPHICS_PIPELINE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_COMPUTE_PIPELINE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_LAYOUT,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SAMPLER,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_SET_LAYOUT,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_POOL,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FRAMEBUFFER,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RENDER_PASS,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_COMMAND_POOL,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_LOADER_INSTANCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_LOADER_DEVICE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DEVICE_GROUP_DEVICE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RENDER_PASS_INPUT_ATTACHMENT_ASPECT,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_USAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RENDER_PASS_MULTIVIEW,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SAMPLER_YCBCR_CONVERSION,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_UPDATE_TEMPLATE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXTERNAL_MEMORY_BUFFER,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXTERNAL_MEMORY_IMAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXPORT_FENCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXPORT_SEMAPHORE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_FORMAT_LIST,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RENDER_PASS_2,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_STENCIL_USAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SAMPLER_REDUCTION_MODE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FRAMEBUFFER_ATTACHMENTS,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SEMAPHORE_TYPE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SWAPCHAIN_KHR,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SURFACE_KHR,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VIEWPORT,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RECT2D,

    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PHYSICAL_DEVICE_FEATURES,

    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VERTEX_INPUT_BINDING_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_SHADER_STAGE_CREATE_INFO_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SURFACE_FORMAT_KHR_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PRESENT_MODE_KHR_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_QUEUE_FAMILY_PROPERTIES_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PHYSICAL_DEVICE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXTENSION_PROPERTIES_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_COLOR_BLEND_ATTACHMENT_STATE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RECT2D_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VIEWPORT_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DEPENDENCY_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_REFERENCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_ATTACHMENT_DESCRIPTION,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_SET_LAYOUT_BINDING_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DEVICE_QUEUE_CREATE_INFO_ARRAY, 

    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_CLEAR_VALUE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FORMAT_ARRAY,

    MEMORY_ALLOCATION_TYPE_OBJ_TEST,
    MEMORY_ALLOCATION_TYPE_OBJ_RENDERER,
    MEMORY_ALLOCATION_TYPE_OBJ_STRUCT_FIELD_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_HEADER,
    MEMORY_ALLOCATION_TYPE_OBJ_SUBPASS_SHADER_LIST_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_BUFFER,
    MEMORY_ALLOCATION_TYPE_OBJ_FONT,
    MEMORY_ALLOCATION_TYPE_OBJ_GLYPH_MESH_POOL,
    MEMORY_ALLOCATION_TYPE_OBJ_LIBRARY,
    MEMORY_ALLOCATION_TYPE_OBJ_MESH3D,
    MEMORY_ALLOCATION_TYPE_OBJ_RENDER_WINDOW,
    MEMORY_ALLOCATION_TYPE_OBJ_STRING_BUILDER,
    MEMORY_ALLOCATION_TYPE_OBJ_TEXT_MESH,

    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDERER,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ALLOCATOR,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_CMD_BUFFER_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_PHYSICAL_DEVICE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_PHYSICAL_DEVICE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_INSTANCE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_MATERIAL,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_UNIFORM_RESOURCE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE_VIEW,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE_VIEW_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_MESH,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_MODULE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SWAPCHAIN,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_TEXTURE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_SCENE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_QUEUE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_OBJECT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS,
    MEMORy_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_POOL,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_BUFFER,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_CAMERA,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_CAMERA_SYSTEM,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_FRAMEBUFFER,

    MEMORY_ALLOCATION_TYPE_OBJ_VK_DIRECTIONAL_LIGHT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_POINT_LIGHT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SPOT_LIGHT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_AMBIENT_LIGHT,

    MEMORY_ALLOCATION_TYPE_OBJ_VK_VERTEX_BUFFER_LAYOUT_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RESOURCE_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_NEXT_PASS_USAGE_ARRAY,

    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_CREATE_INFO,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_SETTINGS,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_DESCRIPTION_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_TYPE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_PIPELINE_LAYOUT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_LIBRARY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_PIPELINE_LAYOUT_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RENDER_PASS_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_LAYOUT,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_LAYOUT_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_LOGICAL_DEVICE,

    MEMORY_ALLOCATION_TYPE_OBJ_VK_SPIRV_CODE_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VK_MATERIAL_LIBRARY,

    MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_U16_ARRAY,
    MEMORY_ALLOCATION_TYPE_OBJ_VEC3_ARRAY,

    MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER,
    MEMORY_ALLOCATION_TYPE_IN_MEMORY_CHAR_BUFFER,

    MEMORY_ALLOCATION_TYPE_MAX
} memory_allocation_type_t;
