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

# Utility rule file for ContinuousSubmit.

# Include any custom commands dependencies for this target.
include smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/compiler_depend.make

# Include the progress variables for this target.
include smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/progress.make

smsplat/SMSplat/CMakeFiles/ContinuousSubmit:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/ctest -D ContinuousSubmit

ContinuousSubmit: smsplat/SMSplat/CMakeFiles/ContinuousSubmit
ContinuousSubmit: smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/build.make
.PHONY : ContinuousSubmit

# Rule to build all files generated by this target.
smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/build: ContinuousSubmit
.PHONY : smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/build

smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/clean:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousSubmit.dir/cmake_clean.cmake
.PHONY : smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/clean

smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/depend:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/levonyeghiazaryan/repos/splatRepo/splat /home/levonyeghiazaryan/repos/splatRepo/splat/external/smsplat/SMSplat /home/levonyeghiazaryan/repos/splatRepo/splat/build /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat /home/levonyeghiazaryan/repos/splatRepo/splat/build/smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : smsplat/SMSplat/CMakeFiles/ContinuousSubmit.dir/depend

