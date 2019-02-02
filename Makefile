# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /media/lei/TOU/2019SpringTerm/GeometricModelling/Assignments/CMPT764Assignment1-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/lei/TOU/2019SpringTerm/GeometricModelling/Assignments/CMPT764Assignment1-master

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /media/lei/TOU/2019SpringTerm/GeometricModelling/Assignments/CMPT764Assignment1-master/CMakeFiles /media/lei/TOU/2019SpringTerm/GeometricModelling/Assignments/CMPT764Assignment1-master/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /media/lei/TOU/2019SpringTerm/GeometricModelling/Assignments/CMPT764Assignment1-master/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named WingedEdge

# Build rule for target.
WingedEdge: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 WingedEdge
.PHONY : WingedEdge

# fast build rule for target.
WingedEdge/fast:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/build
.PHONY : WingedEdge/fast

#=============================================================================
# Target rules for targets named embed-resource

# Build rule for target.
embed-resource: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 embed-resource
.PHONY : embed-resource

# fast build rule for target.
embed-resource/fast:
	$(MAKE) -f lib/embed-resource/CMakeFiles/embed-resource.dir/build.make lib/embed-resource/CMakeFiles/embed-resource.dir/build
.PHONY : embed-resource/fast

WingedEdge.o: WingedEdge.cpp.o

.PHONY : WingedEdge.o

# target to build an object file
WingedEdge.cpp.o:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/WingedEdge.cpp.o
.PHONY : WingedEdge.cpp.o

WingedEdge.i: WingedEdge.cpp.i

.PHONY : WingedEdge.i

# target to preprocess a source file
WingedEdge.cpp.i:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/WingedEdge.cpp.i
.PHONY : WingedEdge.cpp.i

WingedEdge.s: WingedEdge.cpp.s

.PHONY : WingedEdge.s

# target to generate assembly for a file
WingedEdge.cpp.s:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/WingedEdge.cpp.s
.PHONY : WingedEdge.cpp.s

shader/frag.glsl.o: shader/frag.glsl.c.o

.PHONY : shader/frag.glsl.o

# target to build an object file
shader/frag.glsl.c.o:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/frag.glsl.c.o
.PHONY : shader/frag.glsl.c.o

shader/frag.glsl.i: shader/frag.glsl.c.i

.PHONY : shader/frag.glsl.i

# target to preprocess a source file
shader/frag.glsl.c.i:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/frag.glsl.c.i
.PHONY : shader/frag.glsl.c.i

shader/frag.glsl.s: shader/frag.glsl.c.s

.PHONY : shader/frag.glsl.s

# target to generate assembly for a file
shader/frag.glsl.c.s:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/frag.glsl.c.s
.PHONY : shader/frag.glsl.c.s

shader/vert.glsl.o: shader/vert.glsl.c.o

.PHONY : shader/vert.glsl.o

# target to build an object file
shader/vert.glsl.c.o:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/vert.glsl.c.o
.PHONY : shader/vert.glsl.c.o

shader/vert.glsl.i: shader/vert.glsl.c.i

.PHONY : shader/vert.glsl.i

# target to preprocess a source file
shader/vert.glsl.c.i:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/vert.glsl.c.i
.PHONY : shader/vert.glsl.c.i

shader/vert.glsl.s: shader/vert.glsl.c.s

.PHONY : shader/vert.glsl.s

# target to generate assembly for a file
shader/vert.glsl.c.s:
	$(MAKE) -f CMakeFiles/WingedEdge.dir/build.make CMakeFiles/WingedEdge.dir/shader/vert.glsl.c.s
.PHONY : shader/vert.glsl.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... WingedEdge"
	@echo "... embed-resource"
	@echo "... WingedEdge.o"
	@echo "... WingedEdge.i"
	@echo "... WingedEdge.s"
	@echo "... shader/frag.glsl.o"
	@echo "... shader/frag.glsl.i"
	@echo "... shader/frag.glsl.s"
	@echo "... shader/vert.glsl.o"
	@echo "... shader/vert.glsl.i"
	@echo "... shader/vert.glsl.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

