import os


# notice customization

# for which year the notice should be issued
year_started = 2021
year_current = 2024
# who has written this software
author = 'Ravi Prakash Singh'
# name of your software
software = 'VulkanRenderer'
# description of your software 
description = '''\tVulkanRenderer (inclusive of its dependencies and subprojects 
\tsuch as toolchains written by the same author) is a software to render 
\t2D & 3D geometries by writing C/C++ code and shaders.'''
# root of the software's source code relative this python script
root = '../../'
# which directories are disallowed to be considered under this notice
disallowed_directories = {
	'dependencies',
	'shared-dependencies',
}
# which directories are allowed to be considered under this notice
allowed_directories = {
	'include/renderer/',
	'shaders/',
	'source/',
	'showcase/',
	'toolchain/',
	'unit_tests/'
}
# which files are allowed to be considered under this notice
allowed_extensions = {
	'.legal_test',
	'.c',
	'.h',
	'.cpp',
	'.hpp',
	'.v3dshader',
	'.vert',
	'.frag',
	'.geom',
	'.tese',
	'.tesc',
	'.comp'
}


def get_legal_notice(file):
	return '''/*
\t***This is computer generated notice - Do not modify it***

%s

\tFile: %s is a part of %s

\tCopyright (C) %d - %d  Author: %s

\tThis program is free software: you can redistribute it and/or modify
\tit under the terms of the GNU General Public License as published by
\tthe Free Software Foundation, either version 3 of the License, or
\t(at your option) any later version.

\tThis program is distributed in the hope that it will be useful,
\tbut WITHOUT ANY WARRANTY; without even the implied warranty of
\tMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
\tGNU General Public License for more details.

\tYou should have received a copy of the GNU General Public License
\talong with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/'''%(description, file, software, year_started, year_current, author)

def embed_legal_notice(file_path):
	file = open(file_path, 'r')
	data = file.read()
	file.close()
	index = data.find('/*\n\t***This is computer generated notice - ')
	length = len(data)
	if(index != -1):
		end_index = data[index:length].index('*/')
		data = data[(end_index + 2):length]
		print('Notice Overwritten: ' + file_path)
	else:
		print("Notice Added: " + file_path)
	file = open(file_path, 'w')
	notice = get_legal_notice(os.path.basename(file_path))
	if not data.startswith('\n\n'):
		notice += '\n\n'
	data = notice + data
	file.write(data)
	file.close()

def is_embed_allowed(entry):
	for extension in allowed_extensions:
		if(entry.name.endswith(extension)):
			return True
	return False

def is_disallowed_directory(directory):
	if os.path.basename(os.path.normpath(directory)) in disallowed_directories:
		return True
	return False

def embed_legal_notice_for_all(directory):
	if is_disallowed_directory(directory):
		return
	with os.scandir(directory) as entries:
		for entry in entries:
			if entry.is_file() and is_embed_allowed(entry):
				embed_legal_notice(entry.path)
			elif not entry.is_file():
				embed_legal_notice_for_all(entry)

def main():
	base_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), root)
	print(base_dir)
	for directory in allowed_directories:
		print(os.path.basename(os.path.join(root, directory)))
		embed_legal_notice_for_all(os.path.join(base_dir, directory))

main()
