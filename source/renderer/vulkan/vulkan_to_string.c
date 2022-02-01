
#include <renderer/internal/vulkan/vulkan_to_string.h>

void vk_bool32_to_string(const char* name, VkBool32 value, BUFFER* string_buffer)
{
	buf_push_string(string_buffer, name);
	switch(value)
	{
		case VK_TRUE:
			buf_push_string(string_buffer, "VK_TRUE\n"); 
            break;
		case VK_FALSE:
            buf_push_string(string_buffer, "VK_FALSE\n");
		    break;  
        default:
			buf_push_string(string_buffer, "Invalid VkBool32\n");
            break;
	}
}

void vk_sample_count_flags_to_string(const char* name, VkSampleCountFlags flags, BUFFER* string_buffer)
{
	buf_push_string(string_buffer, name);
    if(flags & VK_SAMPLE_COUNT_1_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_1_BIT ");
    if(flags & VK_SAMPLE_COUNT_2_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_2_BIT ");
    if(flags & VK_SAMPLE_COUNT_4_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_4_BIT ");
    if(flags & VK_SAMPLE_COUNT_8_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_8_BIT ");
    if(flags & VK_SAMPLE_COUNT_16_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_16_BIT ");
    if(flags & VK_SAMPLE_COUNT_32_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_32_BIT ");
    if(flags & VK_SAMPLE_COUNT_64_BIT)
    	buf_push_string(string_buffer, "VK_SAMPLE_COUNT_64_BIT ");
    buf_push_newline(string_buffer);
}

void vk_physical_device_type_to_string(const char* name, VkPhysicalDeviceType type, BUFFER* string_buffer)
{
	buf_push_string(string_buffer, name);
 	switch(type)
 	{
 		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
 			buf_push_string(string_buffer, "VK_PHYSICAL_DEVICE_TYPE_OTHER\n");
 			break;
    	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    		buf_push_string(string_buffer, "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU\n");
    		break;
    	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    		buf_push_string(string_buffer, "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU\n");
    		break;
    	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    		buf_push_string(string_buffer, "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU\n");
    		break;
    	case VK_PHYSICAL_DEVICE_TYPE_CPU:
    		buf_push_string(string_buffer, "VK_PHYSICAL_DEVICE_TYPE_CPU\n");
    		break;
    	default:
    		buf_push_string(string_buffer, "Unknown physical device type\n");
    		break;
    }
}

void vk_physical_device_properties_to_string(const char* description, VkPhysicalDeviceProperties* properties, BUFFER* string_buffer)
{
	char buffer[512];
    buf_push_string(string_buffer, description);
    buf_printf(string_buffer, buffer, "\t\tapiVersion: %lu\n", properties->apiVersion);
    buf_printf(string_buffer, buffer, "\t\tdriverVersion: %lu\n", properties->driverVersion);
    buf_printf(string_buffer, buffer, "\t\tvendorID: %lu\n", properties->vendorID);
    buf_printf(string_buffer, buffer, "\t\tdeviceID: %lu\n", properties->deviceID);
    vk_physical_device_type_to_string("\t\tdeviceType: ", properties->deviceType, string_buffer);
    buf_printf(string_buffer, buffer, "\t\tdeviceName: %s\n", properties->deviceName);
    buf_push_newline(string_buffer);
}

void vk_physical_device_limits_to_string(const char* description, VkPhysicalDeviceLimits* limits, BUFFER* string_buffer)
{
	buf_push_string(string_buffer, description);
	char buffer[512]; 	// staging buffer
	buf_printf(string_buffer, buffer, "\t\tmaxImageDimension1D: %lu\n", limits->maxImageDimension1D);
	buf_printf(string_buffer, buffer, "\t\tmaxImageDimension2D : %lu\n", limits->maxImageDimension2D);
    buf_printf(string_buffer, buffer, "\t\tmaxImageDimension3D : %lu\n", limits->maxImageDimension3D);
    buf_printf(string_buffer, buffer, "\t\tmaxImageDimensionCube : %lu\n", limits->maxImageDimensionCube);
    buf_printf(string_buffer, buffer, "\t\tmaxImageArrayLayers : %lu\n", limits->maxImageArrayLayers);
    buf_printf(string_buffer, buffer, "\t\tmaxTexelBufferElements : %lu\n", limits->maxTexelBufferElements);
    buf_printf(string_buffer, buffer, "\t\tmaxUniformBufferRange : %lu\n", limits->maxUniformBufferRange);
    buf_printf(string_buffer, buffer, "\t\tmaxStorageBufferRange : %lu\n", limits->maxStorageBufferRange);
    buf_printf(string_buffer, buffer, "\t\tmaxPushConstantsSize : %lu\n", limits->maxPushConstantsSize);
    buf_printf(string_buffer, buffer, "\t\tmaxMemoryAllocationCount : %lu\n", limits->maxMemoryAllocationCount);
    buf_printf(string_buffer, buffer, "\t\tmaxSamplerAllocationCount : %lu\n", limits->maxSamplerAllocationCount);
    buf_printf(string_buffer, buffer, "\t\tbufferImageGranularity: %llu\n", limits->bufferImageGranularity);
    buf_printf(string_buffer, buffer, "\t\tsparseAddressSpaceSize: %llu\n", limits->sparseAddressSpaceSize);
    buf_printf(string_buffer, buffer, "\t\tmaxBoundDescriptorSets: %lu\n", limits->maxBoundDescriptorSets);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorSamplers: %lu\n", limits->maxPerStageDescriptorSamplers);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorUniformBuffers: %lu\n", limits->maxPerStageDescriptorUniformBuffers);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorStorageBuffers: %lu\n", limits->maxPerStageDescriptorStorageBuffers);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorSampledImages: %lu\n", limits->maxPerStageDescriptorSampledImages);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorStorageImages: %lu\n", limits->maxPerStageDescriptorStorageImages);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageDescriptorInputAttachments: %lu\n", limits->maxPerStageDescriptorInputAttachments);
    buf_printf(string_buffer, buffer, "\t\tmaxPerStageResources: %lu\n", limits->maxPerStageResources);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetSamplers: %lu\n", limits->maxDescriptorSetSamplers);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetUniformBuffers: %lu\n", limits->maxDescriptorSetUniformBuffers);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetUniformBuffersDynamic: %lu\n", limits->maxDescriptorSetUniformBuffersDynamic);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetStorageBuffers: %lu\n", limits->maxDescriptorSetStorageBuffers);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetStorageBuffersDynamic: %lu\n", limits->maxDescriptorSetStorageBuffersDynamic);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetSampledImages: %lu\n", limits->maxDescriptorSetSampledImages);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetStorageImages: %lu\n", limits->maxDescriptorSetStorageImages);
    buf_printf(string_buffer, buffer, "\t\tmaxDescriptorSetInputAttachments: %lu\n", limits->maxDescriptorSetInputAttachments);
    buf_printf(string_buffer, buffer, "\t\tmaxVertexInputAttributes: %lu\n", limits->maxVertexInputAttributes);
    buf_printf(string_buffer, buffer, "\t\tmaxVertexInputBindings: %lu\n", limits->maxVertexInputBindings);
    buf_printf(string_buffer, buffer, "\t\tmaxVertexInputAttributeOffset: %lu\n", limits->maxVertexInputAttributeOffset);
    buf_printf(string_buffer, buffer, "\t\tmaxVertexInputBindingStride: %lu\n", limits->maxVertexInputBindingStride);
    buf_printf(string_buffer, buffer, "\t\tmaxVertexOutputComponents: %lu\n", limits->maxVertexOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationGenerationLevel: %lu\n", limits->maxTessellationGenerationLevel);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationPatchSize: %lu\n", limits->maxTessellationPatchSize);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationControlPerVertexInputComponents: %lu\n", limits->maxTessellationControlPerVertexInputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationControlPerVertexOutputComponents: %lu\n", limits->maxTessellationControlPerVertexOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationControlPerPatchOutputComponents: %lu\n", limits->maxTessellationControlPerPatchOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationControlTotalOutputComponents: %lu\n", limits->maxTessellationControlTotalOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationEvaluationInputComponents: %lu\n", limits->maxTessellationEvaluationInputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxTessellationEvaluationOutputComponents: %lu\n", limits->maxTessellationEvaluationOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxGeometryShaderInvocations: %lu\n", limits->maxGeometryShaderInvocations);
    buf_printf(string_buffer, buffer, "\t\tmaxGeometryInputComponents: %lu\n", limits->maxGeometryInputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxGeometryOutputComponents: %lu\n", limits->maxGeometryOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxGeometryOutputVertices: %lu\n", limits->maxGeometryOutputVertices);
    buf_printf(string_buffer, buffer, "\t\tmaxGeometryTotalOutputComponents: %lu\n", limits->maxGeometryTotalOutputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxFragmentInputComponents: %lu\n", limits->maxFragmentInputComponents);
    buf_printf(string_buffer, buffer, "\t\tmaxFragmentOutputAttachments: %lu\n", limits->maxFragmentOutputAttachments);
    buf_printf(string_buffer, buffer, "\t\tmaxFragmentDualSrcAttachments: %lu\n", limits->maxFragmentDualSrcAttachments);
    buf_printf(string_buffer, buffer, "\t\tmaxFragmentCombinedOutputResources: %lu\n", limits->maxFragmentCombinedOutputResources);
    buf_printf(string_buffer, buffer, "\t\tmaxComputeSharedMemorySize: %lu\n", limits->maxComputeSharedMemorySize);
    buf_printf(string_buffer, buffer, "\t\tmaxComputeWorkGroupCount: (%lu, %lu, %lu)\n", limits->maxComputeWorkGroupCount[0], limits->maxComputeWorkGroupCount[1], limits->maxComputeWorkGroupCount[2]);
    buf_printf(string_buffer, buffer, "\t\tmaxComputeWorkGroupInvocations: %lu\n", limits->maxComputeWorkGroupInvocations);
    buf_printf(string_buffer, buffer, "\t\tmaxComputeWorkGroupSize: (%lu, %lu, %lu)\n", limits->maxComputeWorkGroupSize[0], limits->maxComputeWorkGroupSize[1], limits->maxComputeWorkGroupSize[2]);
    buf_printf(string_buffer, buffer, "\t\tsubPixelPrecisionBits: %lu\n", limits->subPixelPrecisionBits);
    buf_printf(string_buffer, buffer, "\t\tsubTexelPrecisionBits: %lu\n", limits->subTexelPrecisionBits);
    buf_printf(string_buffer, buffer, "\t\tmipmapPrecisionBits: %lu\n", limits->mipmapPrecisionBits);
    buf_printf(string_buffer, buffer, "\t\tmaxDrawIndexedIndexValue: %lu\n", limits->maxDrawIndexedIndexValue);
    buf_printf(string_buffer, buffer, "\t\tmaxDrawIndirectCount: %lu\n", limits->maxDrawIndirectCount);
    buf_printf(string_buffer, buffer, "\t\tmaxSamplerLodBias: %f\n", limits->maxSamplerLodBias);
    buf_printf(string_buffer, buffer, "\t\tmaxSamplerAnisotropy: %f\n", limits->maxSamplerAnisotropy);
    buf_printf(string_buffer, buffer, "\t\tmaxViewports: %lu\n", limits->maxViewports);
    buf_printf(string_buffer, buffer, "\t\tmaxViewportDimensions: (%lu, %lu)\n", limits->maxViewportDimensions[0], limits->maxViewportDimensions[1]);
    buf_printf(string_buffer, buffer, "\t\tviewportBoundsRange: (%f, %f)\n", limits->viewportBoundsRange[0], limits->viewportBoundsRange[1]);
    buf_printf(string_buffer, buffer, "\t\tviewportSubPixelBits: %lu\n", limits->viewportSubPixelBits);
    buf_printf(string_buffer, buffer, "\t\tminMemoryMapAlignment: %llu\n", limits->minMemoryMapAlignment);
    buf_printf(string_buffer, buffer, "\t\tminTexelBufferOffsetAlignment: %llu\n", limits->minTexelBufferOffsetAlignment);
    buf_printf(string_buffer, buffer, "\t\tminUniformBufferOffsetAlignment: %llu\n", limits->minUniformBufferOffsetAlignment);
    buf_printf(string_buffer, buffer, "\t\tminStorageBufferOffsetAlignment: %llu\n", limits->minStorageBufferOffsetAlignment);
    buf_printf(string_buffer, buffer, "\t\tminTexelOffset: %d\n", limits->minTexelOffset);
    buf_printf(string_buffer, buffer, "\t\tmaxTexelOffset: %lu\n", limits->maxTexelOffset);
    buf_printf(string_buffer, buffer, "\t\tminTexelGatherOffset: %d\n", limits->minTexelGatherOffset);
    buf_printf(string_buffer, buffer, "\t\tmaxTexelGatherOffset: %lu\n", limits->maxTexelGatherOffset);
    buf_printf(string_buffer, buffer, "\t\tminInterpolationOffset: %f\n", limits->minInterpolationOffset);
    buf_printf(string_buffer, buffer, "\t\tmaxInterpolationOffset: %f\n", limits->maxInterpolationOffset);
    buf_printf(string_buffer, buffer, "\t\tsubPixelInterpolationOffsetBits: %lu\n", limits->subPixelInterpolationOffsetBits);
    buf_printf(string_buffer, buffer, "\t\tmaxFramebufferWidth: %lu\n", limits->maxFramebufferWidth);
    buf_printf(string_buffer, buffer, "\t\tmaxFramebufferHeight: %lu\n", limits->maxFramebufferHeight);
    buf_printf(string_buffer, buffer, "\t\tmaxFramebufferLayers: %lu\n", limits->maxFramebufferLayers);
    vk_sample_count_flags_to_string("\t\tframebufferColorSampleCounts: ", limits->framebufferColorSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tframebufferDepthSampleCounts: ", limits->framebufferDepthSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tframebufferStencilSampleCounts: ", limits->framebufferStencilSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tframebufferNoAttachmentsSampleCounts: ", limits->framebufferNoAttachmentsSampleCounts, string_buffer);
    buf_printf(string_buffer, buffer, "\t\tmaxColorAttachments: %lu\n", limits->maxColorAttachments);
    vk_sample_count_flags_to_string("\t\tsampledImageColorSampleCounts: ", limits->sampledImageColorSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tsampledImageIntegerSampleCounts: ", limits->sampledImageIntegerSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tsampledImageDepthSampleCounts: ", limits->sampledImageDepthSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tsampledImageStencilSampleCounts: ", limits->sampledImageStencilSampleCounts, string_buffer);
    vk_sample_count_flags_to_string("\t\tstorageImageSampleCounts: ", limits->storageImageSampleCounts, string_buffer);
    buf_printf(string_buffer, buffer, "\t\tmaxSampleMaskWords: %lu\n", limits->maxSampleMaskWords);
    vk_bool32_to_string("\t\ttimestampComputeAndGraphics: ", limits->timestampComputeAndGraphics, string_buffer);
    buf_printf(string_buffer, buffer, "\t\ttimestampPeriod: %f\n", limits->timestampPeriod);
    buf_printf(string_buffer, buffer, "\t\tmaxClipDistances: %lu\n", limits->maxClipDistances);
    buf_printf(string_buffer, buffer, "\t\tmaxCullDistances: %lu\n", limits->maxCullDistances);
    buf_printf(string_buffer, buffer, "\t\tmaxCombinedClipAndCullDistances: %lu\n", limits->maxCombinedClipAndCullDistances);
    buf_printf(string_buffer, buffer, "\t\tdiscreteQueuePriorities: %lu\n", limits->discreteQueuePriorities);
    buf_printf(string_buffer, buffer, "\t\tpointSizeRange: (min = %f, max = %f)\n", limits->pointSizeRange[0], limits->pointSizeRange[1]);
    buf_printf(string_buffer, buffer, "\t\tlineWidthRange: (min = %f, max = %f)\n", limits->lineWidthRange[0], limits->lineWidthRange[1]);
    buf_printf(string_buffer, buffer, "\t\tpointSizeGranularity: %f\n", limits->pointSizeGranularity);
    buf_printf(string_buffer, buffer, "\t\tlineWidthGranularity: %f\n", limits->lineWidthGranularity);
    vk_bool32_to_string("\t\tstrictLines: ", limits->strictLines, string_buffer);
    vk_bool32_to_string("\t\tstandardSampleLocations: ", limits->standardSampleLocations, string_buffer);
    buf_printf(string_buffer, buffer, "\t\toptimalBufferCopyOffsetAlignment: %llu\n", limits->optimalBufferCopyOffsetAlignment);
    buf_printf(string_buffer, buffer, "\t\toptimalBufferCopyRowPitchAlignment: %llu\n", limits->optimalBufferCopyRowPitchAlignment);
    buf_printf(string_buffer, buffer, "\t\tnonCoherentAtomSize: %llu\n", limits->nonCoherentAtomSize);
    buf_push_newline(string_buffer);
}
