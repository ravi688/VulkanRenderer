#!/usr/bin/bash

# TODO: move this to GNU makefile automaticaly generate shaders.h file whenever any of the following files changes.

python $(pwd)/$(dirname "$0")/generate_embedded_files.py \
		--header=include/sutk/EmbeddedFiles.hpp \
		--source=source/EmbeddedFiles.cpp \
		--namespace=SUTK \
		--include=sutk/EmbeddedFiles.hpp \
		--filter=embedded_files_filter.json \
		--embed ./themes