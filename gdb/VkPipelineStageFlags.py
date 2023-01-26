
import gdb

VkPipelineStageFlagBits = {
    0x00000001: 'VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT',
    0x00000002: 'VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT',
    0x00000004: 'VK_PIPELINE_STAGE_VERTEX_INPUT_BIT',
    0x00000008: 'VK_PIPELINE_STAGE_VERTEX_SHADER_BIT',
    0x00000010: 'VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT',
    0x00000020: 'VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT',
    0x00000040: 'VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT',
    0x00000080: 'VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT',
    0x00000100: 'VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT',
    0x00000200: 'VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT',
    0x00000400: 'VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT',
    0x00000800: 'VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT',
    0x00001000: 'VK_PIPELINE_STAGE_TRANSFER_BIT',
    0x00002000: 'VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT',
    0x00004000: 'VK_PIPELINE_STAGE_HOST_BIT',
    0x00008000: 'VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT',
    0x00010000: 'VK_PIPELINE_STAGE_ALL_COMMANDS_BIT',
    0x01000000: 'VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT',
    0x00040000: 'VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT',
    0x02000000: 'VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR',
    0x00200000: 'VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR',
    0x00080000: 'VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV',
    0x00100000: 'VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV',
    0x00800000: 'VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT',
    0x00400000: 'VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR',
    0x00020000: 'VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV',
    0: 'VK_PIPELINE_STAGE_NONE_KHR',
    0x00400000: 'VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV',
    0x00200000: 'VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV',
    0x02000000: 'VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV',
    0x7FFFFFFF: 'VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM'
}

class VkPipelineStageFlag:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        v = int(self.val)
        if VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR':
            return 'VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_ALL_COMMANDS_BIT':
            return 'VK_PIPELINE_STAGE_ALL_COMMANDS_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT':
            return 'VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT':
            return 'VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT':
            return 'VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV':
            return 'VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT':
            return 'VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT':
            return 'VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT':
            return 'VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT':
            return 'VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR':
            return 'VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_HOST_BIT':
            return 'VK_PIPELINE_STAGE_HOST_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT':
            return 'VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV':
            return 'VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_NONE_KHR':
            return 'VK_PIPELINE_STAGE_NONE_KHR'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR':
            return 'VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV':
            return 'VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT':
            return 'VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TRANSFER_BIT':
            return 'VK_PIPELINE_STAGE_TRANSFER_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT':
            return 'VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_VERTEX_INPUT_BIT':
            return 'VK_PIPELINE_STAGE_VERTEX_INPUT_BIT'
        elif VkPipelineStageFlagBits[v] == 'VK_PIPELINE_STAGE_VERTEX_SHADER_BIT':
            return 'VK_PIPELINE_STAGE_VERTEX_SHADER_BIT'
        else:
            return 'Undefined'

class VkPipelineStageFlags:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        v = int(self.val)
        output = ""
        index = 0
        while v > 0:
            if v & 1:
                output += VkPipelineStageFlag(1 << index).to_string()
            if v & 1 and ((v >> 1) != 0):
                output += '|'
            v = v >> 1
            index += 1
        if output == "":
            output += VkPipelineStageFlag(0).to_string()
        return output

def VkPipelineStageFlagsPrintHandler(val):
    if str(val.type) == 'VkPipelineStageFlags': return VkPipelineStageFlags(val)

gdb.pretty_printers.append(VkPipelineStageFlagsPrintHandler)
