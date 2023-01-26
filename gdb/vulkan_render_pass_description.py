
import gdb

class VulkanSubPassDescription:
  def __init__(self, val):
    self.val = val

  def to_string(self):
    output =  PrintOpenBrace()
    output += PrintValue(self, 'pipeline_description_index')
    output += PrintArray(self, 'color_attachments', GetInt(self, 'color_attachment_count'))
    output += PrintArray(self, 'input_attachments', GetInt(self, 'input_attachment_count'))
    output += PrintValue(self, 'depth_stencil_attachment')
    output += PrintArray(self, 'preserve_attachments', GetInt(self, 'preserve_attachment_count'))
    output += PrintCloseBrace()
    return output

def VulkanSubPassDescriptionPrintHandler(val):
    if str(val.type) == 'vulkan_subpass_description_t' : return VulkanSubPassDescription(val)


class VkSubpassDependency:

  def __init__(self, val):
    self.val = val

  def to_string(self):
    output =  PrintOpenBrace()
    output += PrintValue(self, 'srcSubpass')
    output += PrintValue(self, 'dstSubpass')
    output += PrintValue(self, 'srcStageMask')
    output += PrintValue(self, 'dstStageMask')
    output += PrintValue(self, 'srcAccessMask')
    output += PrintValue(self, 'dstAccessMask')
    output += PrintValue(self, 'dependencyFlags')
    output += PrintCloseBrace()
    return output

def VkSubpassDependencyPrintHandler(val):
  if str(val.type) == 'VkSubpassDependency' : return VkSubpassDependency(val)

class VulkanRenderPassDescription:

  def __init__(self, val):
    self.val = val

  def to_string(self):
    output =  PrintOpenBrace()
    output += PrintArray(self, 'subpass_descriptions', GetInt(self, 'subpass_description_count'))
    output += PrintArray(self, 'subpass_dependencies', GetInt(self, 'subpass_dependency_count'))
    output += PrintArray(self, 'attachments', GetInt(self,'attachment_count'))
    output += PrintArray(self, 'input_attachments', GetInt(self, 'input_attachment_count'))
    output += PrintValue(self, 'type')
    output += PrintCloseBrace()
    return output

def VulkanRenderPassDescriptionPrintHandler(val):
  if str(val.type) == 'vulkan_render_pass_description_t': return VulkanRenderPassDescription(val)


gdb.pretty_printers.append(VulkanRenderPassDescriptionPrintHandler)
gdb.pretty_printers.append(VulkanSubPassDescriptionPrintHandler)
gdb.pretty_printers.append(VkSubpassDependencyPrintHandler)
