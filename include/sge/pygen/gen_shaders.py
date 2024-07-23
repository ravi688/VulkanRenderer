import sys
import os
import time

c_header_file_path='./shaders.h'
allowed_file_extensions = [ '.v3dshader', '.txt', '.hpp', '.cpp', '.h', '.c' ]

base_dir = os.path.dirname(os.path.realpath(__file__))

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

def embed_file(file_path, header_file):
	if not is_allowed(file_path):
		return
	abs_file_path = os.path.join(base_dir, file_path)
	header_file.write('static const char* %s = \n' % (get_var_name_from_file_path(file_path)))
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

def traverse_dir_recursively(root_dir, callback):
	for root, dirs, files in os.walk(root_dir):
			for file in files:
				callback[0](os.path.join(root, file), callback[1])
			for _dir in dirs:
				traverse_dir_recursively(os.path.join(root, _dir), callback)

template = """

/***This is computer generated file - Do not modify it***/

/* This is auto generated header file (by pygen/gen_shaders.py python script). Do not modify it directly.
 * Time & Date (yy/mm/yyy) of Generation: %dh:%dm:%ds, %d/%d/%d
 */

#pragma once
"""

def setup_header_file(file):
	t = time.localtime()
	_str = template % (t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year)
	file.write(_str)

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
				setup_header_file(header_file)
		if os.path.isdir(arg):
			traverse_dir_recursively(arg, (embed_file, header_file))
		else:
			embed_file(arg, header_file)
	if is_open:
		header_file.close()
	return

main()