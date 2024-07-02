/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_to_string.c is a part of VulkanRenderer

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


#include <sge/internal/vulkan/vulkan_to_string.h>

SGE_API void vk_present_mode_to_string(const char* description, VkPresentModeKHR present_mode, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    switch(present_mode)
    {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            buf_push_string(string_buffer, "VK_PRESENT_MODE_IMMEDIATE_KHR\n");
            break;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            buf_push_string(string_buffer, "VK_PRESENT_MODE_MAILBOX_KHR\n");
            break;
        case VK_PRESENT_MODE_FIFO_KHR:
            buf_push_string(string_buffer, "VK_PRESENT_MODE_FIFO_KHR\n");
            break;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            buf_push_string(string_buffer, "VK_PRESENT_MODE_FIFO_RELAXED_KHR\n");
            break;
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            buf_push_string(string_buffer, "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR\n");
            break;
        default:
            buf_push_string(string_buffer, "Invalid present mode\n");
            break;
    }
}

SGE_API void vk_format_to_string(const char* description, VkFormat format, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    switch(format)
    {
        case VK_FORMAT_UNDEFINED:
            buf_push_string(string_buffer, "VK_FORMAT_UNDEFINED");
            break;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            buf_push_string(string_buffer, "VK_FORMAT_R4G4_UNORM_PACK8");
            break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_R4G4B4A4_UNORM_PACK16");
            break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_B4G4R4A4_UNORM_PACK16");
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_R5G6B5_UNORM_PACK16");
            break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_B5G6R5_UNORM_PACK16");
            break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_R5G5B5A1_UNORM_PACK16");
            break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_B5G5R5A1_UNORM_PACK16");
            break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            buf_push_string(string_buffer, "VK_FORMAT_A1R5G5B5_UNORM_PACK16");
            break;
        case VK_FORMAT_R8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8_UNORM");
            break;
        case VK_FORMAT_R8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8_SNORM");
            break;
        case VK_FORMAT_R8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8_USCALED");
            break;
        case VK_FORMAT_R8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8_SSCALED");
            break;
        case VK_FORMAT_R8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8_UINT");
            break;
        case VK_FORMAT_R8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8_SINT");
            break;
        case VK_FORMAT_R8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_R8_SRGB");
            break;
        case VK_FORMAT_R8G8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_UNORM");
            break;
        case VK_FORMAT_R8G8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_SNORM");
            break;
        case VK_FORMAT_R8G8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_USCALED");
            break;
        case VK_FORMAT_R8G8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_SSCALED");
            break;
        case VK_FORMAT_R8G8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_UINT");
            break;
        case VK_FORMAT_R8G8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_SINT");
            break;
        case VK_FORMAT_R8G8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8_SRGB");
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_UNORM");
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_SNORM");
            break;
        case VK_FORMAT_R8G8B8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_USCALED");
            break;
        case VK_FORMAT_R8G8B8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_SSCALED");
            break;
        case VK_FORMAT_R8G8B8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_UINT");
            break;
        case VK_FORMAT_R8G8B8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_SINT");
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8_SRGB");
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_UNORM");
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_SNORM");
            break;
        case VK_FORMAT_B8G8R8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_USCALED");
            break;
        case VK_FORMAT_B8G8R8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_SSCALED");
            break;
        case VK_FORMAT_B8G8R8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_UINT");
            break;
        case VK_FORMAT_B8G8R8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_SINT");
            break;
        case VK_FORMAT_B8G8R8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8_SRGB");
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_UNORM");
            break;
        case VK_FORMAT_R8G8B8A8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_SNORM");
            break;
        case VK_FORMAT_R8G8B8A8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_USCALED");
            break;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_SSCALED");
            break;
        case VK_FORMAT_R8G8B8A8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_UINT");
            break;
        case VK_FORMAT_R8G8B8A8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_SINT");
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_R8G8B8A8_SRGB");
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_UNORM");
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_SNORM");
            break;
        case VK_FORMAT_B8G8R8A8_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_USCALED");
            break;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_SSCALED");
            break;
        case VK_FORMAT_B8G8R8A8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_UINT");
            break;
        case VK_FORMAT_B8G8R8A8_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_SINT");
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            buf_push_string(string_buffer, "VK_FORMAT_B8G8R8A8_SRGB");
            break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_UNORM_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_SNORM_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_USCALED_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_SSCALED_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_UINT_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_SINT_PACK32");
            break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A8B8G8R8_SRGB_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_UNORM_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_SNORM_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_USCALED_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_SSCALED_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_UINT_PACK32");
            break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2R10G10B10_SINT_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_UNORM_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_SNORM_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_USCALED_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_SSCALED_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_UINT_PACK32");
            break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_A2B10G10R10_SINT_PACK32");
            break;
        case VK_FORMAT_R16_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16_UNORM");
            break;
        case VK_FORMAT_R16_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16_SNORM");
            break;
        case VK_FORMAT_R16_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16_USCALED");
            break;
        case VK_FORMAT_R16_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16_SSCALED");
            break;
        case VK_FORMAT_R16_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16_UINT");
            break;
        case VK_FORMAT_R16_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16_SINT");
            break;
        case VK_FORMAT_R16_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R16_SFLOAT");
            break;
        case VK_FORMAT_R16G16_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_UNORM");
            break;
        case VK_FORMAT_R16G16_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_SNORM");
            break;
        case VK_FORMAT_R16G16_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_USCALED");
            break;
        case VK_FORMAT_R16G16_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_SSCALED");
            break;
        case VK_FORMAT_R16G16_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_UINT");
            break;
        case VK_FORMAT_R16G16_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_SINT");
            break;
        case VK_FORMAT_R16G16_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16_SFLOAT");
            break;
        case VK_FORMAT_R16G16B16_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_UNORM");
            break;
        case VK_FORMAT_R16G16B16_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_SNORM");
            break;
        case VK_FORMAT_R16G16B16_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_USCALED");
            break;
        case VK_FORMAT_R16G16B16_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_SSCALED");
            break;
        case VK_FORMAT_R16G16B16_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_UINT");
            break;
        case VK_FORMAT_R16G16B16_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_SINT");
            break;
        case VK_FORMAT_R16G16B16_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16_SFLOAT");
            break;
        case VK_FORMAT_R16G16B16A16_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_UNORM");
            break;
        case VK_FORMAT_R16G16B16A16_SNORM:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_SNORM");
            break;
        case VK_FORMAT_R16G16B16A16_USCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_USCALED");
            break;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_SSCALED");
            break;
        case VK_FORMAT_R16G16B16A16_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_UINT");
            break;
        case VK_FORMAT_R16G16B16A16_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_SINT");
            break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R16G16B16A16_SFLOAT");
            break;
        case VK_FORMAT_R32_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32_UINT");
            break;
        case VK_FORMAT_R32_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32_SINT");
            break;
        case VK_FORMAT_R32_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R32_SFLOAT");
            break;
        case VK_FORMAT_R32G32_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32_UINT");
            break;
        case VK_FORMAT_R32G32_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32_SINT");
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32_SFLOAT");
            break;
        case VK_FORMAT_R32G32B32_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32_UINT");
            break;
        case VK_FORMAT_R32G32B32_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32_SINT");
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32_SFLOAT");
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32A32_UINT");
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32A32_SINT");
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R32G32B32A32_SFLOAT");
            break;
        case VK_FORMAT_R64_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64_UINT");
            break;
        case VK_FORMAT_R64_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64_SINT");
            break;
        case VK_FORMAT_R64_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R64_SFLOAT");
            break;
        case VK_FORMAT_R64G64_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64_UINT");
            break;
        case VK_FORMAT_R64G64_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64_SINT");
            break;
        case VK_FORMAT_R64G64_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64_SFLOAT");
            break;
        case VK_FORMAT_R64G64B64_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64_UINT");
            break;
        case VK_FORMAT_R64G64B64_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64_SINT");
            break;
        case VK_FORMAT_R64G64B64_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64_SFLOAT");
            break;
        case VK_FORMAT_R64G64B64A64_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64A64_UINT");
            break;
        case VK_FORMAT_R64G64B64A64_SINT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64A64_SINT");
            break;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_R64G64B64A64_SFLOAT");
            break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_B10G11R11_UFLOAT_PACK32");
            break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32");
            break;
        case VK_FORMAT_D16_UNORM:
            buf_push_string(string_buffer, "VK_FORMAT_D16_UNORM");
            break;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            buf_push_string(string_buffer, "VK_FORMAT_X8_D24_UNORM_PACK32");
            break;
        case VK_FORMAT_D32_SFLOAT:
            buf_push_string(string_buffer, "VK_FORMAT_D32_SFLOAT");
            break;
        case VK_FORMAT_S8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_S8_UINT");
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_D16_UNORM_S8_UINT");
            break;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_D24_UNORM_S8_UINT");
            break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            buf_push_string(string_buffer, "VK_FORMAT_D32_SFLOAT_S8_UINT");
            break;
        default:
            debug_assert_wrn__(false,"Unhandled format");
            buf_push_string(string_buffer, "<unhandled VkFormat>");
            break;
    }
}

SGE_API void vk_color_space_to_string(const char* description, VkColorSpaceKHR color_space, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    switch(color_space)
    {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
            break;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT");
            break;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT");
            break;
        case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT");
            break;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT");
            break;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_BT709_LINEAR_EXT");
            break;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_BT709_NONLINEAR_EXT");
            break;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_BT2020_LINEAR_EXT");
            break;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_HDR10_ST2084_EXT");
            break;
        case VK_COLOR_SPACE_DOLBYVISION_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_DOLBYVISION_EXT");
            break;
        case VK_COLOR_SPACE_HDR10_HLG_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_HDR10_HLG_EXT");
            break;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT");
            break;
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT");
            break;
        case VK_COLOR_SPACE_PASS_THROUGH_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_PASS_THROUGH_EXT");
            break;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT");
            break;
        case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD: 
            buf_push_string(string_buffer, "VK_COLOR_SPACE_DISPLAY_NATIVE_AMD");
            break;
        default:
            buf_push_string(string_buffer, "Unvalid color space");
            break;
    }
}

SGE_API void vk_surface_format_to_string(const char* description, VkSurfaceFormatKHR surface_format, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    buf_push_char(string_buffer, '(');
    vk_format_to_string(" ", surface_format.format, string_buffer);
    buf_push_string(string_buffer, ", ");
    vk_color_space_to_string(" ", surface_format.colorSpace, string_buffer);
    buf_push_char(string_buffer, ')');
    buf_push_newline(string_buffer);
}

SGE_API void vk_surface_transform_flag_bits_to_string(const char* description, VkSurfaceTransformFlagBitsKHR transform, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    switch(transform)
    {
        case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n");
            break;
        case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR:
            buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n");
            break;
        default:
            buf_push_string(string_buffer, "Unkown surface transform flag bit\n");
    }
}

SGE_API void vk_surface_transform_flags_to_string(const char* description, VkSurfaceTransformFlagsKHR transform_flags, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    if(transform_flags & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR ");
    if(transform_flags & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR)
        buf_push_string(string_buffer, "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR ");
    buf_push_newline(string_buffer);
}

SGE_API void vk_surface_capabilities_to_string(const char* description, VkSurfaceCapabilitiesKHR* capabilities, BUFFER* string_buffer)
{
    buf_push_string(string_buffer, description);
    char stage_buffer[512];
    buf_printf(string_buffer, stage_buffer, "\t\tminImageCount: %lu\n", capabilities->minImageCount);
    buf_printf(string_buffer, stage_buffer, "\t\tmaxImageCount: %lu\n", capabilities->maxImageCount);
    buf_printf(string_buffer, stage_buffer, "\t\tcurrentExtent: (%lu, %lu)\n", capabilities->currentExtent.width, capabilities->currentExtent.height);
    buf_printf(string_buffer, stage_buffer, "\t\tminImageExtent: (%lu, %lu)\n", capabilities->minImageExtent.width, capabilities->minImageExtent.height);
    buf_printf(string_buffer, stage_buffer, "\t\tmaxImageExtent: (%lu, %lu)\n", capabilities->maxImageExtent.width, capabilities->maxImageExtent.height);
    buf_printf(string_buffer, stage_buffer, "\t\tmaxImageArrayLayers: %lu\n", capabilities->maxImageArrayLayers);
    vk_surface_transform_flags_to_string("\t\tsupportedTransforms: ", capabilities->supportedTransforms, string_buffer);
    vk_surface_transform_flag_bits_to_string("\t\tcurrentTransform: ", capabilities->currentTransform, string_buffer);
}

SGE_API void vk_bool32_to_string(const char* name, VkBool32 value, BUFFER* string_buffer)
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

SGE_API void vk_sample_count_flags_to_string(const char* name, VkSampleCountFlags flags, BUFFER* string_buffer)
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

SGE_API void vk_physical_device_type_to_string(const char* name, VkPhysicalDeviceType type, BUFFER* string_buffer)
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

SGE_API void vk_physical_device_properties_to_string(const char* description, VkPhysicalDeviceProperties* properties, BUFFER* string_buffer)
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

SGE_API void vk_physical_device_limits_to_string(const char* description, VkPhysicalDeviceLimits* limits, BUFFER* string_buffer)
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
