# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /root/chatRoom

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/chatRoom/build

# Include any dependencies generated for this target.
include src/client/CMakeFiles/chatClient.dir/depend.make

# Include the progress variables for this target.
include src/client/CMakeFiles/chatClient.dir/progress.make

# Include the compile flags for this target's objects.
include src/client/CMakeFiles/chatClient.dir/flags.make

src/client/CMakeFiles/chatClient.dir/chatClient.cc.o: src/client/CMakeFiles/chatClient.dir/flags.make
src/client/CMakeFiles/chatClient.dir/chatClient.cc.o: ../src/client/chatClient.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/chatRoom/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/client/CMakeFiles/chatClient.dir/chatClient.cc.o"
	cd /root/chatRoom/build/src/client && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/chatClient.dir/chatClient.cc.o -c /root/chatRoom/src/client/chatClient.cc

src/client/CMakeFiles/chatClient.dir/chatClient.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/chatClient.dir/chatClient.cc.i"
	cd /root/chatRoom/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/chatRoom/src/client/chatClient.cc > CMakeFiles/chatClient.dir/chatClient.cc.i

src/client/CMakeFiles/chatClient.dir/chatClient.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/chatClient.dir/chatClient.cc.s"
	cd /root/chatRoom/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/chatRoom/src/client/chatClient.cc -o CMakeFiles/chatClient.dir/chatClient.cc.s

src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.requires:

.PHONY : src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.requires

src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.provides: src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.requires
	$(MAKE) -f src/client/CMakeFiles/chatClient.dir/build.make src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.provides.build
.PHONY : src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.provides

src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.provides.build: src/client/CMakeFiles/chatClient.dir/chatClient.cc.o


src/client/CMakeFiles/chatClient.dir/clientMain.cc.o: src/client/CMakeFiles/chatClient.dir/flags.make
src/client/CMakeFiles/chatClient.dir/clientMain.cc.o: ../src/client/clientMain.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/chatRoom/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/client/CMakeFiles/chatClient.dir/clientMain.cc.o"
	cd /root/chatRoom/build/src/client && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/chatClient.dir/clientMain.cc.o -c /root/chatRoom/src/client/clientMain.cc

src/client/CMakeFiles/chatClient.dir/clientMain.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/chatClient.dir/clientMain.cc.i"
	cd /root/chatRoom/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/chatRoom/src/client/clientMain.cc > CMakeFiles/chatClient.dir/clientMain.cc.i

src/client/CMakeFiles/chatClient.dir/clientMain.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/chatClient.dir/clientMain.cc.s"
	cd /root/chatRoom/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/chatRoom/src/client/clientMain.cc -o CMakeFiles/chatClient.dir/clientMain.cc.s

src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.requires:

.PHONY : src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.requires

src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.provides: src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.requires
	$(MAKE) -f src/client/CMakeFiles/chatClient.dir/build.make src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.provides.build
.PHONY : src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.provides

src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.provides.build: src/client/CMakeFiles/chatClient.dir/clientMain.cc.o


# Object files for target chatClient
chatClient_OBJECTS = \
"CMakeFiles/chatClient.dir/chatClient.cc.o" \
"CMakeFiles/chatClient.dir/clientMain.cc.o"

# External object files for target chatClient
chatClient_EXTERNAL_OBJECTS =

../bin/chatClient: src/client/CMakeFiles/chatClient.dir/chatClient.cc.o
../bin/chatClient: src/client/CMakeFiles/chatClient.dir/clientMain.cc.o
../bin/chatClient: src/client/CMakeFiles/chatClient.dir/build.make
../bin/chatClient: src/client/CMakeFiles/chatClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/chatRoom/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../../bin/chatClient"
	cd /root/chatRoom/build/src/client && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/chatClient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/client/CMakeFiles/chatClient.dir/build: ../bin/chatClient

.PHONY : src/client/CMakeFiles/chatClient.dir/build

src/client/CMakeFiles/chatClient.dir/requires: src/client/CMakeFiles/chatClient.dir/chatClient.cc.o.requires
src/client/CMakeFiles/chatClient.dir/requires: src/client/CMakeFiles/chatClient.dir/clientMain.cc.o.requires

.PHONY : src/client/CMakeFiles/chatClient.dir/requires

src/client/CMakeFiles/chatClient.dir/clean:
	cd /root/chatRoom/build/src/client && $(CMAKE_COMMAND) -P CMakeFiles/chatClient.dir/cmake_clean.cmake
.PHONY : src/client/CMakeFiles/chatClient.dir/clean

src/client/CMakeFiles/chatClient.dir/depend:
	cd /root/chatRoom/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/chatRoom /root/chatRoom/src/client /root/chatRoom/build /root/chatRoom/build/src/client /root/chatRoom/build/src/client/CMakeFiles/chatClient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/client/CMakeFiles/chatClient.dir/depend
