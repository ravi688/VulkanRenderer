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
# if a folder is specified, then which text files to consider for embedding?
text_file_extensions = [ '.v3dshader', '.template', '.txt', '.hpp', '.cpp', '.h', '.c' ]
# if a folder is specified, then which binary files to consider for embedding?
binary_file_extensions = [ '.ttf', '.bmp' ]
bytes_per_row = 20

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

def is_text_encoding(file_path):
	for extension in text_file_extensions:
		if file_path.endswith(extension):
			return True
	return False

def is_binary_encoding(file_path):
	for extension in binary_file_extensions:
		if file_path.endswith(extension):
			return True
	return False

def embed_text_file(file_path, header_file):
	abs_file_path = os.path.join(base_dir, file_path)
	var_name = get_var_name_from_file_path(file_path)
	header_file.write('static const char* %s = \n' % (var_name))
	file = open(abs_file_path, 'r')
	header_file.write('"');
	i = 0
	while True:
		char = file.read(1)
		if not char:
			break
		if char == '"' or char == '\\':
			header_file.write('\\')
		if char == '\n' or char == '\r':
			header_file.write('\\n"')
		header_file.write(char)
		if char == '\n' or char == '\r':
			header_file.write('"')
		i = i + 1
	header_file.write('";\n')
	file.close()
	mappings[abs_file_path] = (var_name, i)

def embed_binary_file(file_path, header_file):
	abs_file_path = os.path.join(base_dir, file_path)
	var_name = get_var_name_from_file_path(file_path)
	header_file.write('static const char %s[] = \n' % (var_name))
	file = open(abs_file_path, 'rb')
	file.seek(0, os.SEEK_END)
	file_size = file.tell()
	file.seek(0)
	header_file.write('{');
	i = 0
	while True:
		byte = file.read(1)
		if not byte:
			break
		header_file.write('0x%x' % byte[0])
		if not (i + 1) == file_size:
			header_file.write(', ')
		if ((i + 1) % bytes_per_row) == 0:
			header_file.write('\n')
		i = i + 1
	header_file.write('\n};\n')
	file.close()
	mappings[abs_file_path] = (var_name, i)


# loads a file at 'file_path' and embeds its contents to the header file at 'c_header_file_path'
def embed_file(file_path, header_file):
	if is_text_encoding(file_path):
		embed_text_file(file_path, header_file)
	elif is_binary_encoding(file_path):
		embed_binary_file(file_path, header_file)
	else:
		print('Skipping %s' % file_path)
	return

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

epilog_template_part1 = """
typedef struct shader_file_path_and_data_mapping_t
{
	const char* file_path;
	const char* data;
	long long data_size;
} shader_file_path_and_data_mapping_t;

#define G_SHADER_MAPPING_COUNT %d

static shader_file_path_and_data_mapping_t g_shader_mappings[G_SHADER_MAPPING_COUNT];
static bool g_is_shader_mappings_populated = false;

static __attribute__((unused)) const shader_file_path_and_data_mapping_t* g_get_shader_mappings()
{
	if(!g_is_shader_mappings_populated)
	{
"""

epilog_template_part2 = """
		g_is_shader_mappings_populated = true;
	}
	return g_shader_mappings;
}
"""

def setup_header_file_epilog(file):
	file.write(epilog_template_part1 % (len(mappings)))
	for i, (key, value) in enumerate(mappings.items()):
		file.write('\t\tg_shader_mappings[%d] = (shader_file_path_and_data_mapping_t) { \"%s\", %s, %lu };\n' % (i, key, value[0], value[1]))
	file.write(epilog_template_part2)

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