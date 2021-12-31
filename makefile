# This makefile automatically adds all directories and sub-directories under the 'source' directory as c-source folders, cpp-source folders and include folders.
# If you want to exclude one or more folders from this automated process then add the folders to the EXCLUDEDIRS variable.
# You can manually add source and include files below, libraries and library folders have to be added manually.

EXE := Snow_Deformation.out
SRCAPP := source/main.cpp
OL := -O0
CPPFLAGS := -std=c++11

EXCLUDESRCS := 
EXCLUDEDIRS := source/vmath-0.13 source/assimp

SRCS := $(SRCAPP) source/vmath-0.13/src/vmath.cpp
INCLUDE := -Isource/vmath-0.13/src

LDIR := -Lassimp/lib -Wl,-rpath=../assimp/lib
LIBS := -ldl -lglfw -lassimp

########## DO NOT EDIT ANYTHING BELOW THIS ##########
########## DO NOT EDIT ANYTHING BELOW THIS ##########
########## DO NOT EDIT ANYTHING BELOW THIS ##########

# Constants
BIN := build
SRCROOT = source

# Windoes shell script to find all subdirectories in SRCROOT 
define FINDSRCDIRS
find $(SRCROOT) -type d -print
endef

# Store path to source files, in a convenient format
SRCDIRS := $(patsubst %/,%,$(dir $(SRCS)))
# Add source root as source directory
SRCDIRS += $(SRCROOT)
# Add all folders below source root as source directories, in a convenient format. Folders to and below the ones specified in the EXCLUDEDIRS variable are ignored
SRCDIRS += $(subst \,/,$(shell $(FINDSRCDIRS)))
# Remove excluded folders from source directories
SRCDIRS := $(filter-out $(addsuffix %,$(EXCLUDEDIRS)),$(SRCDIRS))

# Find all source files in all source directories
SRCS += $(wildcard $(SRCDIRS:%=%/*.c))
SRCS += $(wildcard $(SRCDIRS:%=%/*.cpp))
SRCS += $(wildcard $(SRCDIRS:%=%/*.cc))
# Remove excluded files from source files
SRCS := $(filter-out $(EXCLUDESRCS),$(SRCS))
# Set all source directories as include folders, in a valid format
INCLUDE += $(SRCDIRS:%=-I%) -MMD -MP

# TODO Remember to check for duplicates and abort if found
# Create object and dependency files from source files below the binary folder
OBJS := $(filter %.o,$(SRCS:%.c=$(BIN)/%.o) $(SRCS:%.cpp=$(BIN)/%.o) $(SRCS:%.cc=$(BIN)/%.o))
DEPS := $(OBJS:.o=.d)

# List of binary directories
BINDIRS := $(BIN) $(patsubst %,build/%,$(dir $(SRCS)))

# Dummy variable that's never used. Evaluated immidiately which creates all the needed binary directories
create-output-directories := $(shell for f in $(BINDIRS); do if [ ! -e "$$f" ]; then mkdir -p "$$f"; fi; done)

$(BIN)/$(EXE): $(OBJS)
	g++ $(CPPFLAGS) -o $@ $^ $(LDIR) $(LIBS)

$(BIN)/%.o: %.cpp
	g++ $(CPPFLAGS) -c $(OL) -o $@ $< $(INCLUDE)

$(BIN)/%.o: %.cc
	g++ $(CPPFLAGS) -c $(OL) -o $@ $< $(INCLUDE)

$(BIN)/%.o: %.c
	g++ $(CPPFLAGS) -c $(OL) -o $@ $< $(INCLUDE)

.PHONY: clean run
clean:
	rm -r $(BIN)
run: 
	cd $(BIN); ./$(EXE);

-include $(DEPS)

Resources for future improvement of this makefile:
http://aegis.sourceforge.net/auug97.pdf
http://uploads.mitechie.com/books/Managing_Projects_with_GNU_Make_Third_Edition.pdf
