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
include splat_test/CMakeFiles/test_smsplat_ui.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include splat_test/CMakeFiles/test_smsplat_ui.dir/compiler_depend.make

# Include the progress variables for this target.
include splat_test/CMakeFiles/test_smsplat_ui.dir/progress.make

# Include the compile flags for this target's objects.
include splat_test/CMakeFiles/test_smsplat_ui.dir/flags.make

splat_test/test_smsplat_ui_autogen/timestamp: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/./libexec/moc
splat_test/test_smsplat_ui_autogen/timestamp: splat_test/CMakeFiles/test_smsplat_ui.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target test_smsplat_ui"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E cmake_autogen /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/CMakeFiles/test_smsplat_ui_autogen.dir/AutogenInfo.json Release
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/local/lib/python3.10/dist-packages/cmake/data/bin/cmake -E touch /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/test_smsplat_ui_autogen/timestamp

splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o: splat_test/CMakeFiles/test_smsplat_ui.dir/flags.make
splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o: splat_test/test_smsplat_ui_autogen/mocs_compilation.cpp
splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o: splat_test/CMakeFiles/test_smsplat_ui.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o -MF CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/test_smsplat_ui_autogen/mocs_compilation.cpp

splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/test_smsplat_ui_autogen/mocs_compilation.cpp > CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.i

splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/test_smsplat_ui_autogen/mocs_compilation.cpp -o CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.s

splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o: splat_test/CMakeFiles/test_smsplat_ui.dir/flags.make
splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o: /home/levonyeghiazaryan/repos/splatRepo/splat/test/test_QtSMSplat_UI.cpp
splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o: splat_test/CMakeFiles/test_smsplat_ui.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o -MF CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o.d -o CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o -c /home/levonyeghiazaryan/repos/splatRepo/splat/test/test_QtSMSplat_UI.cpp

splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.i"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/levonyeghiazaryan/repos/splatRepo/splat/test/test_QtSMSplat_UI.cpp > CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.i

splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.s"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/levonyeghiazaryan/repos/splatRepo/splat/test/test_QtSMSplat_UI.cpp -o CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.s

# Object files for target test_smsplat_ui
test_smsplat_ui_OBJECTS = \
"CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o"

# External object files for target test_smsplat_ui
test_smsplat_ui_EXTERNAL_OBJECTS =

bin/test_smsplat_ui: splat_test/CMakeFiles/test_smsplat_ui.dir/test_smsplat_ui_autogen/mocs_compilation.cpp.o
bin/test_smsplat_ui: splat_test/CMakeFiles/test_smsplat_ui.dir/test_QtSMSplat_UI.cpp.o
bin/test_smsplat_ui: splat_test/CMakeFiles/test_smsplat_ui.dir/build.make
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Test.so.6.6.3
bin/test_smsplat_ui: lib/libQtSMSplat.a
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Quick.so.6.6.3
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6QmlModels.so.6.6.3
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Qml.so.6.6.3
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Network.so.6.6.3
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6OpenGL.so.6.6.3
bin/test_smsplat_ui: lib/libSMSplatListModel.a
bin/test_smsplat_ui: lib/libSMSplatDB.a
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Sql.so.6.6.3
bin/test_smsplat_ui: lib/libsm_splat_manager_lib.a
bin/test_smsplat_ui: lib/libsplat_lib.a
bin/test_smsplat_ui: /usr/local/lib/libspdlog.a
bin/test_smsplat_ui: /usr/lib/libgdal.so
bin/test_smsplat_ui: lib/libsplat_utils.a
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Gui.so.6.6.3
bin/test_smsplat_ui: /home/levonyeghiazaryan/Qt/6.6.3/gcc_64/lib/libQt6Core.so.6.6.3
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libGLX.so
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libOpenGL.so
bin/test_smsplat_ui: lib/libsm_utils.a
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.4.5.4d
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libopencv_videoio.so.4.5.4d
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so.4.5.4d
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.4.5.4d
bin/test_smsplat_ui: /usr/lib/x86_64-linux-gnu/libopencv_core.so.4.5.4d
bin/test_smsplat_ui: splat_test/CMakeFiles/test_smsplat_ui.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/levonyeghiazaryan/repos/splatRepo/splat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../bin/test_smsplat_ui"
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_smsplat_ui.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
splat_test/CMakeFiles/test_smsplat_ui.dir/build: bin/test_smsplat_ui
.PHONY : splat_test/CMakeFiles/test_smsplat_ui.dir/build

splat_test/CMakeFiles/test_smsplat_ui.dir/clean:
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test && $(CMAKE_COMMAND) -P CMakeFiles/test_smsplat_ui.dir/cmake_clean.cmake
.PHONY : splat_test/CMakeFiles/test_smsplat_ui.dir/clean

splat_test/CMakeFiles/test_smsplat_ui.dir/depend: splat_test/test_smsplat_ui_autogen/timestamp
	cd /home/levonyeghiazaryan/repos/splatRepo/splat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/levonyeghiazaryan/repos/splatRepo/splat /home/levonyeghiazaryan/repos/splatRepo/splat/test /home/levonyeghiazaryan/repos/splatRepo/splat/build /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test /home/levonyeghiazaryan/repos/splatRepo/splat/build/splat_test/CMakeFiles/test_smsplat_ui.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : splat_test/CMakeFiles/test_smsplat_ui.dir/depend

