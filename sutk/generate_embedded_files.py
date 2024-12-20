# Usage Example:
#	python generate_embedded_files.py --header=./include/sutk/EmbeddedFiles.hpp --source=./source/EmbeddedFiles.cpp --include=sutk/EmbeddedFiles.hpp <list of files or folders>
# 	output: <directory of python script>/include/sutk/EmbeddedFiles.hpp
#			<directory of python script>/source/EmbeddedFiles.cpp
#

import sys
import os
import time
import argparse
import json

namespace_name = ''
cpp_header_file_path=''
cpp_source_file_path=''
include_str = ''
# if a folder is specified, then which text files to consider for embedding?
text_file_extensions = []
# if a folder is specified, then which binary files to consider for embedding?
binary_file_extensions = []
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

def embed_text_file(file_path, source_file):
	print('Embedding text file: %s' % (file_path))
	abs_file_path = os.path.join(base_dir, file_path)
	var_name = get_var_name_from_file_path(file_path)
	source_file.write('static const char* %s = \n' % (var_name))
	file = open(abs_file_path, 'r')
	source_file.write('"');
	i = 0
	while True:
		char = file.read(1)
		if not char:
			break
		if char == '"' or char == '\\':
			source_file.write('\\')
		if char == '\n' or char == '\r':
			source_file.write('\\n"')
		source_file.write(char)
		if char == '\n' or char == '\r':
			source_file.write('"')
		i = i + 1
	source_file.write('";\n')
	file.close()
	mappings[file_path] = (var_name, i)

def embed_binary_file(file_path, source_file):
	print('Embedding binary file: %s' % (file_path))
	abs_file_path = os.path.join(base_dir, file_path)
	var_name = get_var_name_from_file_path(file_path)
	source_file.write('static const char %s[] = \n' % (var_name))
	file = open(abs_file_path, 'rb')
	file.seek(0, os.SEEK_END)
	file_size = file.tell()
	file.seek(0)
	source_file.write('{');
	i = 0
	while True:
		byte = file.read(1)
		if not byte:
			break
		source_file.write('0x%x' % byte[0])
		if not (i + 1) == file_size:
			source_file.write(', ')
		if ((i + 1) % bytes_per_row) == 0:
			source_file.write('\n')
		i = i + 1
	source_file.write('\n};\n')
	file.close()
	mappings[file_path] = (var_name, i)


# loads a file at 'file_path' and embeds its contents to the source file at 'cpp_source_file_path'
def embed_file(file_path, source_file):
	if is_text_encoding(file_path):
		embed_text_file(file_path, source_file)
	elif is_binary_encoding(file_path):
		embed_binary_file(file_path, source_file)
	else:
		print('Skipping %s' % file_path)
	return

# traverses a directory recursively and calls 'callback' upon encountering a file
def traverse_dir_recursively(root_dir, callback):
	for root, dirs, files in os.walk(root_dir):
			for file in files:
				callback[0](os.path.join(root, file), callback[1])

notice_template = """
/***This is computer generated file - Do not modify it***/

/* This is auto generated file (by generate_embedded_files.py python script). Do not modify it directly.
 * Time & Date (yy/mm/yyy) of Generation: %dh:%dm:%ds, %d/%d/%d
 */
"""

def get_notice_string():
	t = time.localtime()
	notice = notice_template % (t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);
	return notice

header_template = """
#pragma once

#include <string_view> // for std::string_view
#include <span> // for std::span<>
#include <cstddef> // for std::byte
#include <optional> // for std::optional<>

namespace %s
{
	namespace EmbeddedFiles
	{
		std::optional<std::span<const std::byte>> getData(const std::string_view filePath) noexcept;
	}
}
"""

def write_header_file(file):
	_str = get_notice_string() + header_template % (namespace_name);
	file.write(_str)

prolog_template = """
#include <%s>
#include <common/assert.h>
#include <common/debug.h>
#include <string.h>

namespace %s
{
	namespace EmbeddedFiles
	{
"""

def write_source_file_prolog(file):
	prolog_str = prolog_template % (include_str, namespace_name)
	file.write(get_notice_string() + prolog_str)

epilog_template_part1 = """
		typedef struct file_path_and_data_mapping_t
		{
			const char* file_path;
			const char* data;
			long long data_size;
		} file_path_and_data_mapping_t;
		
		#define G_FILE_MAPPING_COUNT %d
		
		static file_path_and_data_mapping_t g_shader_mappings[G_FILE_MAPPING_COUNT];
		static bool g_is_file_mappings_populated = false;
		
		static __attribute__((unused)) const file_path_and_data_mapping_t* g_get_file_mappings()
		{
			if(!g_is_file_mappings_populated)
			{
"""

epilog_template_part2 = """
				g_is_file_mappings_populated = true;
			}
			return g_shader_mappings;
		}

		static bool str_ends_with(const char* str, const char* suffix)
		{
			u32 str_len = strlen(str);
			u32 sfx_len = strlen(suffix);
		
			/* screening case */
			if(str_len < sfx_len)
				return false;
		
			return strncmp(str + (str_len - sfx_len), suffix, sfx_len) == 0;
		}

		static const char* get_builtin_file_data(const char* virtual_file_path, u64 OUT data_size)
		{
			_com_assert(strlen(virtual_file_path) > 0);
			if(virtual_file_path[0] != '/')
			{
				debug_log_error("Invalid root directory in %s", virtual_file_path);
				return NULL;
			}
		
			/* skip first '/' (possibly multiple adjacent) characters */
			while(*virtual_file_path == '/')
				virtual_file_path += 1;
		
			/* find a match */
			u32 match_index = U32_MAX;
			const file_path_and_data_mapping_t* mappings = g_get_file_mappings();
			for(u32 i = 0; i < G_FILE_MAPPING_COUNT; i++)
			{
				if(str_ends_with(mappings[i].file_path, virtual_file_path))
				{
					if(match_index != U32_MAX)
					{
						debug_log_error("Multiple matches were found, returning the first match: %s", mappings[i].file_path);
						break;
					}
					else match_index = i;
				}
			}
		
			if(match_index == U32_MAX)
			{
				debug_log_warning("No match found for virtual file path: %s, returning NULL", virtual_file_path);
				return NULL;
			}
		
			_com_assert(match_index < G_FILE_MAPPING_COUNT);
		
			if(data_size != NULL)
				OUT data_size = CAST_TO(u64, mappings[match_index].data_size);
		
			return mappings[match_index].data;
		}

		std::optional<std::span<const std::byte>> getData(const std::string_view filePath) noexcept
		{
			char buffer[filePath.length() + 1];
			memcpy(buffer, filePath.data(), filePath.length());
			buffer[filePath.length()] = 0;
			u64 data_size = 0;
			const char* data = get_builtin_file_data(buffer, &data_size);
			if(!data)
				return { };
			return { std::span<const std::byte> { reinterpret_cast<const std::byte*>(data), data_size } };
		}
	}
}
"""

def write_source_file_epilog(file):
	file.write(epilog_template_part1 % (len(mappings)))
	for i, (key, value) in enumerate(mappings.items()):
		file.write('\t\tg_shader_mappings[%d] = (file_path_and_data_mapping_t) { \"%s\", %s, %lu };\n' % (i, key, value[0], value[1]))
	file.write(epilog_template_part2)

json_filter_syntax_help = """
{
	"text_files" :
	[
		".txt",
		".theme",
		".svg"
	],
	"binary_files" : 
	[
		".ttf"
	]
}
"""

def verify_filter_json(json_doc):
	if not 'text_files' in json_doc or not type(json_doc['text_files']) is list:
		return False
	if not 'binary_files' in json_doc or not type(json_doc['binary_files']) is list:
		return False
	return True

def main():
	parser = argparse.ArgumentParser(description='Embeds text and binaries files into C++ Source Files, and generates a .hpp/.cpp pair');
	parser.add_argument('--header', action='store', dest='header_path', type=str, required=True)
	parser.add_argument('--source', action='store', dest='source_path', type=str, required=True)
	parser.add_argument('--namespace', action='store', dest='namespace_name', type=str, required=True)
	parser.add_argument('--include', action='store', dest='include_str', type=str, required=True)
	parser.add_argument('--filter', action='store', dest='filter_file', type=str, required=True)
	parser.add_argument('--embed', nargs='+', dest='embeddings', help = 'List of files or folders', required=True)
	given_args = parser.parse_args();

	global cpp_header_file_path
	global cpp_source_file_path
	global namespace_name
	global include_str
	global embeddings
	cpp_header_file_path = given_args.header_path
	cpp_source_file_path = given_args.source_path
	namespace_name = given_args.namespace_name
	include_str = given_args.include_str
	embeddings = given_args.embeddings

	# determine which file extensions should be considered for embedding
	with open(given_args.filter_file, "r") as json_file:
		json_str = json_file.read()
		json_doc = json.loads(json_str)
		if not verify_filter_json(json_doc):
			print('Filter Json File is invalid! It should be as in the following syntax: %s' % (json_filter_syntax_help))
			return
		global text_file_extensions
		global binary_file_extensions
		text_file_extensions = json_doc['text_files']
		binary_file_extensions = json_doc['binary_files']

	# write header file
	abs_header_file_path = os.path.join(base_dir, cpp_header_file_path)
	print('Writing to %s' % abs_header_file_path)
	header_file = open(abs_header_file_path, 'w')
	write_header_file(header_file)
	header_file.close()

	# write source file
	abs_source_file_path = os.path.join(base_dir, cpp_source_file_path)
	print('Writing to %s' % abs_source_file_path)
	source_file = open(abs_source_file_path, 'w')
	write_source_file_prolog(source_file)
	for embedding in embeddings:
		if os.path.isdir(embedding):
			traverse_dir_recursively(embedding, (embed_file, source_file))
		else:
			embed_file(embedding, source_file)
	write_source_file_epilog(source_file);
	source_file.close()
	return

main()