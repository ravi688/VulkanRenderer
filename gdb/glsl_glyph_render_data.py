
import gdb

class GLSLGlyphRenderData:

  def __init__(self, val):
    self.val = val

  def to_string(self):
    output =  PrintOpenBrace()
    output += PrintValue(self, 'ofst')
    output += PrintValue(self, 'indx')
    output += PrintValue(self, 'rotn')
    output += PrintValue(self, 'stid')
    output += PrintValue(self, 'scal')
    output += PrintCloseBrace()
    return output

def GLSLGlyphRenderDataPrintHandler(val):
  if str(val.type) == 'glsl_glyph_render_data_t': return GLSLGlyphRenderData(val)


gdb.pretty_printers.append(GLSLGlyphRenderDataPrintHandler)
