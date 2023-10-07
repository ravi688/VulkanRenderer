

set history save on
set history size 10000
set history filename ./gdb/.gdbhistory
set listsize 20
set logging enabled on

source ./gdb/common.py
source ./gdb/vulkan_render_pass_description.py
source ./gdb/vulkan_attachment_type.py
source ./gdb/VkPipelineStageFlags.py
source ./gdb/VkAccessFlags.py
source ./gdb/glsl_glyph_render_data.py
