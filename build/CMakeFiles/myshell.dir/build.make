# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bleverly/mycode/myshell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bleverly/mycode/myshell/build

# Include any dependencies generated for this target.
include CMakeFiles/myshell.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myshell.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myshell.dir/flags.make

CMakeFiles/myshell.dir/src/main.cpp.o: CMakeFiles/myshell.dir/flags.make
CMakeFiles/myshell.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bleverly/mycode/myshell/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/myshell.dir/src/main.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myshell.dir/src/main.cpp.o -c /home/bleverly/mycode/myshell/src/main.cpp

CMakeFiles/myshell.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myshell.dir/src/main.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bleverly/mycode/myshell/src/main.cpp > CMakeFiles/myshell.dir/src/main.cpp.i

CMakeFiles/myshell.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myshell.dir/src/main.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bleverly/mycode/myshell/src/main.cpp -o CMakeFiles/myshell.dir/src/main.cpp.s

CMakeFiles/myshell.dir/src/mycmd.cpp.o: CMakeFiles/myshell.dir/flags.make
CMakeFiles/myshell.dir/src/mycmd.cpp.o: ../src/mycmd.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bleverly/mycode/myshell/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/myshell.dir/src/mycmd.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myshell.dir/src/mycmd.cpp.o -c /home/bleverly/mycode/myshell/src/mycmd.cpp

CMakeFiles/myshell.dir/src/mycmd.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myshell.dir/src/mycmd.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bleverly/mycode/myshell/src/mycmd.cpp > CMakeFiles/myshell.dir/src/mycmd.cpp.i

CMakeFiles/myshell.dir/src/mycmd.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myshell.dir/src/mycmd.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bleverly/mycode/myshell/src/mycmd.cpp -o CMakeFiles/myshell.dir/src/mycmd.cpp.s

CMakeFiles/myshell.dir/src/myerr.cpp.o: CMakeFiles/myshell.dir/flags.make
CMakeFiles/myshell.dir/src/myerr.cpp.o: ../src/myerr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bleverly/mycode/myshell/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/myshell.dir/src/myerr.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myshell.dir/src/myerr.cpp.o -c /home/bleverly/mycode/myshell/src/myerr.cpp

CMakeFiles/myshell.dir/src/myerr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myshell.dir/src/myerr.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bleverly/mycode/myshell/src/myerr.cpp > CMakeFiles/myshell.dir/src/myerr.cpp.i

CMakeFiles/myshell.dir/src/myerr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myshell.dir/src/myerr.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bleverly/mycode/myshell/src/myerr.cpp -o CMakeFiles/myshell.dir/src/myerr.cpp.s

CMakeFiles/myshell.dir/src/process.cpp.o: CMakeFiles/myshell.dir/flags.make
CMakeFiles/myshell.dir/src/process.cpp.o: ../src/process.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bleverly/mycode/myshell/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/myshell.dir/src/process.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myshell.dir/src/process.cpp.o -c /home/bleverly/mycode/myshell/src/process.cpp

CMakeFiles/myshell.dir/src/process.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myshell.dir/src/process.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bleverly/mycode/myshell/src/process.cpp > CMakeFiles/myshell.dir/src/process.cpp.i

CMakeFiles/myshell.dir/src/process.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myshell.dir/src/process.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bleverly/mycode/myshell/src/process.cpp -o CMakeFiles/myshell.dir/src/process.cpp.s

# Object files for target myshell
myshell_OBJECTS = \
"CMakeFiles/myshell.dir/src/main.cpp.o" \
"CMakeFiles/myshell.dir/src/mycmd.cpp.o" \
"CMakeFiles/myshell.dir/src/myerr.cpp.o" \
"CMakeFiles/myshell.dir/src/process.cpp.o"

# External object files for target myshell
myshell_EXTERNAL_OBJECTS =

../bin/myshell: CMakeFiles/myshell.dir/src/main.cpp.o
../bin/myshell: CMakeFiles/myshell.dir/src/mycmd.cpp.o
../bin/myshell: CMakeFiles/myshell.dir/src/myerr.cpp.o
../bin/myshell: CMakeFiles/myshell.dir/src/process.cpp.o
../bin/myshell: CMakeFiles/myshell.dir/build.make
../bin/myshell: CMakeFiles/myshell.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bleverly/mycode/myshell/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable ../bin/myshell"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myshell.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myshell.dir/build: ../bin/myshell

.PHONY : CMakeFiles/myshell.dir/build

CMakeFiles/myshell.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myshell.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myshell.dir/clean

CMakeFiles/myshell.dir/depend:
	cd /home/bleverly/mycode/myshell/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bleverly/mycode/myshell /home/bleverly/mycode/myshell /home/bleverly/mycode/myshell/build /home/bleverly/mycode/myshell/build /home/bleverly/mycode/myshell/build/CMakeFiles/myshell.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myshell.dir/depend

