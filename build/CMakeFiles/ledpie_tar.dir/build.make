# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/meetpi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/meetpi/build

# Include any dependencies generated for this target.
include CMakeFiles/ledpie_tar.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ledpie_tar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ledpie_tar.dir/flags.make

CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o: CMakeFiles/ledpie_tar.dir/flags.make
CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o: ../ledpie_for_targets.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/meetpi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o -c /home/pi/meetpi/ledpie_for_targets.cpp

CMakeFiles/ledpie_tar.dir/ledpie_for_targets.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ledpie_tar.dir/ledpie_for_targets.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/meetpi/ledpie_for_targets.cpp > CMakeFiles/ledpie_tar.dir/ledpie_for_targets.i

CMakeFiles/ledpie_tar.dir/ledpie_for_targets.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ledpie_tar.dir/ledpie_for_targets.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/meetpi/ledpie_for_targets.cpp -o CMakeFiles/ledpie_tar.dir/ledpie_for_targets.s

# Object files for target ledpie_tar
ledpie_tar_OBJECTS = \
"CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o"

# External object files for target ledpie_tar
ledpie_tar_EXTERNAL_OBJECTS =

../bin/ledpie_tar: CMakeFiles/ledpie_tar.dir/ledpie_for_targets.o
../bin/ledpie_tar: CMakeFiles/ledpie_tar.dir/build.make
../bin/ledpie_tar: /usr/lib/arm-linux-gnueabihf/libjson-c.so
../bin/ledpie_tar: /usr/lib/libmatrix_creator_hal.so
../bin/ledpie_tar: CMakeFiles/ledpie_tar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/meetpi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/ledpie_tar"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ledpie_tar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ledpie_tar.dir/build: ../bin/ledpie_tar

.PHONY : CMakeFiles/ledpie_tar.dir/build

CMakeFiles/ledpie_tar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ledpie_tar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ledpie_tar.dir/clean

CMakeFiles/ledpie_tar.dir/depend:
	cd /home/pi/meetpi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/meetpi /home/pi/meetpi /home/pi/meetpi/build /home/pi/meetpi/build /home/pi/meetpi/build/CMakeFiles/ledpie_tar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ledpie_tar.dir/depend

