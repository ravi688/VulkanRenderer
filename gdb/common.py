
indent = 0

def getIndent(level):
  indentStr = ''
  while level > 0:
    indentStr += '\t'
    level -= 1
  return indentStr

def is_basic_type(str):
  if str == 'u8':
    return True
  elif str == 'u16':
    return True
  elif str == 'u32':
    return True
  elif str == 'u64':
    return True
  elif str == 's8':
    return True
  elif str == 's16':
    return True
  elif str == 's32':
    return True
  elif str == 's64':
    return True
  elif str == 'f32':
    return True
  elif str == 'f64':
    return True
  elif str == 'uint':
    return True
  else:
    return False

def PrintArray(parent, arrName, arrSize):
  global indent
  indents = getIndent(indent)
  output = indents
  if arrSize <= 0:
    return output + f"{arrName}(s) = not found\n"
  output += arrName + ' = \n' + indents + '['
  i = 0
  output += '\n'
  while i < arrSize:
    indent += 1
    t = parent.val[arrName][i]
    t_str = str(t.type)
    if is_basic_type(t_str):
      output += getIndent(indent)
    output += str(t)
    indent -= 1
    if i != (arrSize - 1):
      output += ','
    output += '\n'
    i += 1
  output += indents + ']\n'
  return output

def PrintValue(parent, valueName):
  global indent
  return getIndent(indent) + str(valueName) + ' = ' + str(parent.val[valueName]) + '\n'

def GetInt(parent, valueName):
  return int(parent.val[valueName])

def PrintOpenBrace():
  global indent
  output = getIndent(indent) + '{\n'
  indent += 1
  return output

def PrintCloseBrace():
  global indent
  indent -= 1
  output = getIndent(indent) + '}'
  return output
  