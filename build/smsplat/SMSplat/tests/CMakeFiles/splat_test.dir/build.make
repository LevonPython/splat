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
include smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/compiler_depend.make

# Include the progress variables for this target.
include smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/progress.make

# Include the compile flags for this target's objects.
include smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/flags.make

smsplat/SMSplat/tests/splat_test_autogen/timestamp: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/./libexec/moc
smsplat/SMSplat/tests/splat_test_autogen/timestamp: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC for target splat_test"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E cmake_autogen /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/CMakeFiles/splat_test_autogen.dir/AutogenInfo.json Release
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E touch /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/splat_test_autogen/timestamp

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/flags.make
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/tests/splat_test_autogen/mocs_compilation.cpp
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o -MF CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/splat_test_autogen/mocs_compilation.cpp

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/splat_test_autogen/mocs_compilation.cpp > CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.i

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/splat_test_autogen/mocs_compilation.cpp -o CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.s

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/flags.make
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o: /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/splat_test.cpp
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o -MF CMakeFiles/splat_test.dir/splat_test.cpp.o.d -o CMakeFiles/splat_test.dir/splat_test.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/splat_test.cpp

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/splat_test.dir/splat_test.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/splat_test.cpp > CMakeFiles/splat_test.dir/splat_test.cpp.i

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/splat_test.dir/splat_test.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/splat_test.cpp -o CMakeFiles/splat_test.dir/splat_test.cpp.s

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/flags.make
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o: /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/src/splat.cpp
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o -MF CMakeFiles/splat_test.dir/__/src/splat.cpp.o.d -o CMakeFiles/splat_test.dir/__/src/splat.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/src/splat.cpp

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/splat_test.dir/__/src/splat.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/src/splat.cpp > CMakeFiles/splat_test.dir/__/src/splat.cpp.i

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/splat_test.dir/__/src/splat.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/src/splat.cpp -o CMakeFiles/splat_test.dir/__/src/splat.cpp.s

# Object files for target splat_test
splat_test_OBJECTS = \
"CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/splat_test.dir/splat_test.cpp.o" \
"CMakeFiles/splat_test.dir/__/src/splat.cpp.o"

# External object files for target splat_test
splat_test_EXTERNAL_OBJECTS =

/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test_autogen/mocs_compilation.cpp.o
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/splat_test.cpp.o
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/__/src/splat.cpp.o
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/build.make
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: lib/libsplat_lib.a
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libgtest_main.a
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libgtest.a
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: lib/libsm_utils.a
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.4.5.4d
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libopencv_videoio.so.4.5.4d
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so.4.5.4d
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.4.5.4d
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: /usr/lib/x86_64-linux-gnu/libopencv_core.so.4.5.4d
/home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test: smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/splat_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/build: /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests/tests/splat_test
.PHONY : smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/build

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/clean:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests && $(CMAKE_COMMAND) -P CMakeFiles/splat_test.dir/cmake_clean.cmake
.PHONY : smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/clean

smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/depend: smsplat/SMSplat/tests/splat_test_autogen/timestamp
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/levonyeghiazaryan/repos/splatRepo/splat /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat/tests /home/levonyeghiazaryan/repos/splatRepo/splat/build /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : smsplat/SMSplat/tests/CMakeFiles/splat_test.dir/depend

