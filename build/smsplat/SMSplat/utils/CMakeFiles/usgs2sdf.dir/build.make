# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/levonyeghiazaryan/repos/splatRepo/splat

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/levonyeghiazaryan/repos/splatRepo/splat/build

# Include any dependencies generated for this target.
include smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/compiler_depend.make

# Include the progress variables for this target.
include smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/progress.make

# Include the compile flags for this target's objects.
include smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/flags.make

smsplat/SMSplat/utils/usgs2sdf_autogen/timestamp: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/./libexec/moc
smsplat/SMSplat/utils/usgs2sdf_autogen/timestamp: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC for target usgs2sdf"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E cmake_autogen /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/CMakeFiles/usgs2sdf_autogen.dir/AutogenInfo.json Release
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E touch /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/usgs2sdf_autogen/timestamp

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/flags.make
smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/utils/usgs2sdf_autogen/mocs_compilation.cpp
smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o -MF CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/usgs2sdf_autogen/mocs_compilation.cpp

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/usgs2sdf_autogen/mocs_compilation.cpp > CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.i

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/usgs2sdf_autogen/mocs_compilation.cpp -o CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.s

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/flags.make
smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o: /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/utils/usgs2sdf.c
smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o -MF CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o.d -o CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/utils/usgs2sdf.c

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/usgs2sdf.dir/usgs2sdf.c.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/utils/usgs2sdf.c > CMakeFiles/usgs2sdf.dir/usgs2sdf.c.i

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/usgs2sdf.dir/usgs2sdf.c.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/utils/usgs2sdf.c -o CMakeFiles/usgs2sdf.dir/usgs2sdf.c.s

# Object files for target usgs2sdf
usgs2sdf_OBJECTS = \
"CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o"

# External object files for target usgs2sdf
usgs2sdf_EXTERNAL_OBJECTS =

bin/usgs2sdf: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf_autogen/mocs_compilation.cpp.o
bin/usgs2sdf: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/usgs2sdf.c.o
bin/usgs2sdf: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/build.make
bin/usgs2sdf: smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../../../bin/usgs2sdf"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/usgs2sdf.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/build: bin/usgs2sdf
.PHONY : smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/build

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/clean:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils && $(CMAKE_COMMAND) -P CMakeFiles/usgs2sdf.dir/cmake_clean.cmake
.PHONY : smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/clean

smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/depend: smsplat/SMSplat/utils/usgs2sdf_autogen/timestamp
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/levonyeghiazaryan/repos/splatRepo/splat /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/utils /home/levonyeghiazaryan/repos/splatRepo/splat/build /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : smsplat/SMSplat/utils/CMakeFiles/usgs2sdf.dir/depend

