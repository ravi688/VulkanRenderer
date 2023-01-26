
import gdb

class VulkanAttachmentType:

	def __init__(self, val):
		self.val = val

	def to_string(self):
		v = int(self.val)
		output = getIndent(indent)
		if v == 0:
			output += 'VULKAN_ATTACHMENT_TYPE_COLOR'
		elif v == int(1 << 1):
			output += 'VULKAN_ATTACHMENT_TYPE_DEPTH'
		elif v == int(1 << 2):
			output += 'VULKAN_ATTACHMENT_TYPE_STENCIL'
		elif v == (int(1 << 1) | int(1 << 2)):
			output += 'VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL'
		else:
			output += '<undefined: vulkan_attachment_type_t>'
		return output

def VulkanAttachmentTypePrintHandler(val):
	if str(val.type) == 'vulkan_attachment_type_t': return VulkanAttachmentType(val)

gdb.pretty_printers.append(VulkanAttachmentTypePrintHandler)
