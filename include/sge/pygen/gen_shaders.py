# Usage Example:
#	python gen_shaders.py ../../../shaders/
# 	output: ./shaders.h
#
# Single file can also be specified
#	python gen_shaders.py ../../../shaders/builtins/bitmap_text_shader.v3dshader


import sys
import os
import time

# path to the generated header file, it is relative to where this python scripts resides
c_header_file_path='./shaders.h'
# if a folder is specified, then which files to consider for embedding?
allowed_file_extensions = [ '.v3dshader', '.txt', '.hpp', '.cpp', '.h', '.c' ]

base_dir = os.path.dirname(os.path.realpath(__file__))

mappings = { }

# converts "../../../shaders/builtins/bitmap_text_shader.v3dshader" to "_________SHADERS_BUILTINS_BITMAP_TEXT_SHADER_V3DSHADER"
def get_var_name_from_file_path(path):
	path = path.upper()
	path = list(path)
	for i in range(0,len(path)):
		if not path[i].isalnum():
			path[i] = '_'
	if path[0].isdigit():
		path.insert(0, '_')
	return "".join(path)

def is_allowed(file_path):
	for extension in allowed_file_extensions:
		if file_path.endswith(extension):
			return True
	return False

# loads a file at 'file_path' and embeds its contents to the header file at 'c_header_file_path'
def embed_file(file_path, header_file):
	if not is_allowed(file_path):
		return
	abs_file_path = os.path.join(base_dir, file_path)
	var_name = get_var_name_from_file_path(file_path)
	header_file.write('static const char* %s = \n' % (var_name))
	file = open(abs_file_path, 'r')
	header_file.write('"');
	while True:
		char = file.read(1)
		if not char:
			break
		if char == '"':
			header_file.write('\\')
		header_file.write(char)
	header_file.write('";\n')
	file.close()
	mappings[abs_file_path] = var_name

# traverses a directory recursively and calls 'callback' upon encountering a file
def traverse_dir_recursively(root_dir, callback):
	for root, dirs, files in os.walk(root_dir):
			for file in files:
				callback[0](os.path.join(root, file), callback[1])
			for _dir in dirs:
				traverse_dir_recursively(os.path.join(root, _dir), callback)

prolog_template = """

/***This is computer generated file - Do not modify it***/

/* This is auto generated header file (by pygen/gen_shaders.py python script). Do not modify it directly.
 * Time & Date (yy/mm/yyy) of Generation: %dh:%dm:%ds, %d/%d/%d
 */

#pragma once
"""

def setup_header_file_prolog(file):
	t = time.localtime()
	_str = prolog_template % (t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year)
	file.write(_str)

epilog_template = """
typedef struct shader_file_path_and_data_mapping_t
{
	const char* file_path;
	const char* data;
} shader_file_path_and_data_mapping_t;

#define G_SHADER_MAPPING_COUNT %d

static const shader_file_path_and_data_mapping_t g_shader_mappings[G_SHADER_MAPPING_COUNT] =
"""

def setup_header_file_epilog(file):
	file.write(epilog_template % (len(mappings)))
	file.write('{\n')
	for i, (key, value) in enumerate(mappings.items()):
		file.write('\t{ \"%s\", \"%s\" }' % (key, value))
		if not (i + 1) == len(mappings):
			file.write(',')
		file.write('\n')
	file.write('};\n')

def main():
	arg_count = len(sys.argv)
	if arg_count <= 1:
		print('Error: not enough arguments are provided')


	is_open = False
	header_file = None
	for arg in sys.argv[1:]:
		if not is_open:
			abs_header_file_path = os.path.join(base_dir, c_header_file_path)
			header_file = open(abs_header_file_path, 'w')
			if header_file.closed == False:
				is_open = True
				setup_header_file_prolog(header_file)
		if os.path.isdir(arg):
			traverse_dir_recursively(arg, (embed_file, header_file))
		else:
			embed_file(arg, header_file)
	if is_open:
		setup_header_file_epilog(header_file);
		header_file.close()
	return

main()