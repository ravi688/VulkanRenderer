#------------- Generated By Build Master 1.0.0 ------------------

# ------------------------------ DOCUMENTATION ---------------------------------
# Release build
# -------------------------
# $ meson setup --wipe <builddir> # wipe the build artifacts (like object files)
# $ meson setup <builddir> --reconfigure --buildtype=release # reconfigure the build directory for release build
# $ meson compile -C <builddir> # compile the project
#
# Debug build
# -------------------------
# $ meson setup --wipe <buildir> # wipe the build artifacts (like object files)
# $ meson setup <builddir> --reconfigure --buildtype=release # reconfigure the build directory for debug build
# $ meson compile -C <builddir> # compile the project
#
# Static Library
# -------------------------
# $ meson setup --wipe <buildir> # wipe the build artifacts (like object files)
# # NOTE: --buildtype=release or --buildtype=debug options can be added here  
# $ meson setup -C <builddir> --reconfigure --default-library=static # reconfigure the build directory for static library
# $ meson compile -C <builddir> # compile the project
# $ meson install -C <builddir> # install the static library
#
# Shared Library
# -------------------------
# $ meson setup --wipe <buildir> # whipe the build artifacts (like object files)
# # NOTE: --buildtype=release or --buildtype=debug options can be added here
# $ meson setup -C <builddir> --reconfigure --default-library=shared # reconfigure the build directory for shared library
# $ meson compile -C <builddir> # compile the project
# $ meson install -C <builddir> # install the shared library
#
# Artifact Installation Directories
# ---------------------------------
# Headers: /include/<ProjectNameInSmallCase>
# Static Libraries: /lib/lib<ProjectNameInSmallCase>.a-
# Shared Libraries: /bin/lib<ProjectNameInSmallCase>.dll
# PkgConfig (.pc) for static library: $PKG_CONFIG_PATH/<ProjectNameInSmallCase>_static.pc
# PkgConfig (.pc) for shared library: $PKG_CONFIG_PATH/<ProjectNameInSmallCase>_shared.pc
#
# -------------------------------- PROJECT CONFIGS -----------------------------

project('SGE', 'c', 'cpp',
  version : '1.0.0',
  meson_version: '>=1.1',
  default_options : [
    'warning_level=3',
    'buildtype=debug',
    'c_std=c17',
    'cpp_std=c++20'
  ]
)

# Variables
test_sources = run_command('find', 'source/tests', '-maxdepth', '1', '-type', 'f', check : false).stdout().strip().split()
sge_sources = run_command('find', 'source/sge', '-maxdepth', '3', '-type', 'f', check : false).stdout().strip().split()
sge_cpp_sources = run_command('find', 'source/sge-cpp', '-maxdepth', '3', '-type', 'f', check : false).stdout().strip().split()
stb_sources = run_command('find', 'source/stb', '-maxdepth', '1', '-type', 'f', check : false).stdout().strip().split()
vulkan_sdk_path = run_command(find_program('python'), '-c', 'import os; print(os.environ["VK_SDK_PATH"])', check : false).stdout().strip()
vulkan_libs_path = vulkan_sdk_path + '/Lib/'


# Defines
defines_bm_internal__ = [
'-DSGE_VULKAN_DRIVER'
]

# Release Build Defines
release_defines_bm_internal__ = [
'-DGLOBAL_RELEASE', '-DRELEASE', '-DSGE_RELEASE', '-DLOG_RELEASE'
] 

# Debug Build Defines
debug_defines_bm_internal__ = [
'-DGLOBAL_DEBUG', '-DDEBUG', '-DSGE_DEBUG', '-DLOG_DEBUG'
]

# Source files (common to all targets)
sources_bm_internal__ = files(
sge_sources, sge_cpp_sources, stb_sources
)

# Include directories
inc_bm_internal__ = include_directories(
'include'
)

# Library Install Directory
lib_install_dir_bm_internal__ = get_option('libdir')/'sge'

# Dependencies
dependencies_bm_internal__ = [
dependency('shader_compiler'),
dependency('common'),
dependency('glslcommon'),
dependency('hpml'),
dependency('meshlib'),
dependency('vulkanheaders'),
dependency('freetype2'),
dependency('glfw3'),
dependency('ttf2mesh'),
dependency('shaderc')
]

# Linker Arguments
windows_link_args_bm_internal__ = [ 
'-L' +  vulkan_libs_path, '-lvulkan-1'
]
linux_link_args_bm_internal__ = [
'-L' +  vulkan_libs_path, '-lvulkan-1'
]
darwin_link_args_bm_internal__ = [

]

# -------------------------------------------------------------------------------
# ------------------------------ FIXTURE ----------------------------------------

# Compiler configuration
add_project_arguments('-m64', language : 'c')
add_project_arguments('-m64', language : 'cpp')
# Linker configuration
link_args_bm_internal__ = []
os_name_bm_internal__ = host_machine.system()
if os_name_bm_internal__ == 'windows'
  link_args_bm_internal__ += windows_link_args_bm_internal__
elif os_name_bm_internal__ == 'linux'
  link_args_bm_internal__ += linux_link_args_bm_internal__
elif os_name_bm_internal__ == 'darwin'
  link_args_bm_internal__ += darwin_link_args_bm_internal__
endif
add_project_link_arguments('-m64', link_args_bm_internal__, language : 'c')
add_project_link_arguments('-m64', link_args_bm_internal__, language : 'cpp')

# Build type specific defines
project_build_mode_defines_bm_internal__ = defines_bm_internal__
if get_option('buildtype') == 'release'
  add_project_arguments(release_defines_bm_internal__, language : 'c')
  add_project_arguments(release_defines_bm_internal__, language : 'cpp')
  project_build_mode_defines_bm_internal__ += release_defines_bm_internal__
else
  add_project_arguments(debug_defines_bm_internal__, language : 'c')
  add_project_arguments(debug_defines_bm_internal__, language : 'cpp')
  project_build_mode_defines_bm_internal__ += debug_defines_bm_internal__
endif

# pkg-config package installation
# Try PKG_CONFIG_PATH first, typicallly it succeeds on MINGW64 (MSYS2)
python_pkg_config_path_result_bm_internal__ = run_command(find_program('python'), '-c', 'import os; print(os.environ["PKG_CONFIG_PATH"])', check : false)
pkgconfig_install_path_bm_internal__ = ''
if python_pkg_config_path_result_bm_internal__.returncode() == 0
  str = python_pkg_config_path_result_bm_internal__.stdout()
  # Unix
  if str.startswith('/')
    pkgconfig_install_path_bm_internal__ = str.replace(';', ':').split(':')[0]
  # Windows
  else
    pkgconfig_install_path_bm_internal__ = str.split(';')[0]
  endif
endif
if pkgconfig_install_path_bm_internal__ == ''
  # Otherwise use pkg-config to query its lookup directories
  message('PKG_CONFIG_PATH seems to be empty, trying another method')
  python_pkg_config_path_result_bm_internal__ = run_command('pkg-config', '--variable', 'pc_path', 'pkg-config', check : false)
  if python_pkg_config_path_result_bm_internal__.returncode() == 0
    str = python_pkg_config_path_result_bm_internal__.stdout()
    if str.startswith('/')
      pkgconfig_install_path_bm_internal__ = str.replace(';', ':').split(':')[0]
    else
      pkgconfig_install_path_bm_internal__ = str.split(';')[0]
    endif
  # Finally if the above attempts fail, use 'libdir' value
  else
    pkgconfig_install_path_bm_internal__ = get_option('libdir')
  endif
endif
message('pkg config path: ' + pkgconfig_install_path_bm_internal__)

#-------------------------------------------------------------------------------
#--------------------------------BUILD TARGETS----------------------------------
pkgmod = import('pkgconfig')

# -------------- Target: main_test ------------------
main_test_sources_bm_internal__ = [
'source/main.c',
'source/test.c',
'source/legal/legal.c',
test_sources
]
main_test_include_dirs_bm_internal__ = [

]
main_test_dependencies_bm_internal__ = [

]
main_test_link_args_bm_internal__ = {
'windows' : [],
'linux' : [],
'darwin' : []
}
main_test_defines_bm_internal__ = [

]
main_test = executable('main_test',
	main_test_sources_bm_internal__ + sources_bm_internal__,
	dependencies: dependencies_bm_internal__ + main_test_dependencies_bm_internal__,
	include_directories: [inc_bm_internal__, main_test_include_dirs_bm_internal__],
	install: false,
	c_args: main_test_defines_bm_internal__ + project_build_mode_defines_bm_internal__,
	cpp_args: main_test_defines_bm_internal__ + project_build_mode_defines_bm_internal__, 
	link_args: main_test_link_args_bm_internal__[host_machine.system()],
	gnu_symbol_visibility: 'hidden'
)

# -------------- Target: sge_static ------------------
sge_static_sources_bm_internal__ = [

]
sge_static_include_dirs_bm_internal__ = [

]
sge_static_dependencies_bm_internal__ = [

]
sge_static_link_args_bm_internal__ = {
'windows' : [],
'linux' : [],
'darwin' : []
}
sge_static_build_defines_bm_internal__ = [
'-DSGE_BUILD_STATIC_LIBRARY'
]
sge_static_use_defines_bm_internal__ = [
'-DSGE_USE_STATIC_LIBRARY'
]
sge_static = static_library('sge_static',
	sge_static_sources_bm_internal__ + sources_bm_internal__,
	dependencies: dependencies_bm_internal__ + sge_static_dependencies_bm_internal__,
	include_directories: [inc_bm_internal__, sge_static_include_dirs_bm_internal__],
	install: true,
	install_dir: lib_install_dir_bm_internal__,
	c_args: sge_static_build_defines_bm_internal__ + project_build_mode_defines_bm_internal__,
	cpp_args: sge_static_build_defines_bm_internal__ + project_build_mode_defines_bm_internal__, 
	link_args: sge_static_link_args_bm_internal__[host_machine.system()],
	gnu_symbol_visibility: 'hidden'
)
sge_static_dep = declare_dependency(
	link_with: sge_static,
	include_directories: [inc_bm_internal__, sge_static_include_dirs_bm_internal__],
	compile_args: sge_static_use_defines_bm_internal__ + project_build_mode_defines_bm_internal__
)
pkgmod.generate(sge_static,
	name: 'SGE',
	description: 'Static Library for SGE',
	filebase: 'sge_static',
	install_dir: pkgconfig_install_path_bm_internal__,
	extra_cflags: sge_static_use_defines_bm_internal__ + project_build_mode_defines_bm_internal__
)

# -------------- Target: sge_shared ------------------
sge_shared_sources_bm_internal__ = [

]
sge_shared_include_dirs_bm_internal__ = [

]
sge_shared_dependencies_bm_internal__ = [

]
sge_shared_link_args_bm_internal__ = {
'windows' : [],
'linux' : [],
'darwin' : []
}
sge_shared_build_defines_bm_internal__ = [
'-DSGE_BUILD_DYNAMIC_LIBRARY'
]
sge_shared_use_defines_bm_internal__ = [
'-DSGE_USE_DYNAMIC_LIBRARY'
]
sge_shared = shared_library('sge_shared',
	sge_shared_sources_bm_internal__ + sources_bm_internal__,
	dependencies: dependencies_bm_internal__ + sge_shared_dependencies_bm_internal__,
	include_directories: [inc_bm_internal__, sge_shared_include_dirs_bm_internal__],
	install: true,
	install_dir: lib_install_dir_bm_internal__,
	c_args: sge_shared_build_defines_bm_internal__ + project_build_mode_defines_bm_internal__,
	cpp_args: sge_shared_build_defines_bm_internal__ + project_build_mode_defines_bm_internal__, 
	link_args: sge_shared_link_args_bm_internal__[host_machine.system()],
	gnu_symbol_visibility: 'hidden'
)
sge_shared_dep = declare_dependency(
	link_with: sge_shared,
	include_directories: [inc_bm_internal__, sge_shared_include_dirs_bm_internal__],
	compile_args: sge_shared_use_defines_bm_internal__ + project_build_mode_defines_bm_internal__
)
pkgmod.generate(sge_shared,
	name: 'SGE',
	description: 'Shared Library for SGE',
	filebase: 'sge_shared',
	install_dir: pkgconfig_install_path_bm_internal__,
	extra_cflags: sge_shared_use_defines_bm_internal__ + project_build_mode_defines_bm_internal__
)


#-------------------------------------------------------------------------------
#--------------------------------Header Intallation----------------------------------
# Header installation
install_subdir('include/sge', install_dir : get_option('includedir'))
install_subdir('include/sge-cpp', install_dir : get_option('includedir'))


