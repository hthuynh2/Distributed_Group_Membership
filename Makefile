# GNU Make Special Characters
COMMA := ,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

# Compiler Variables
CXX = g++
LD = g++
WARNINGS = -Wall -Werror -Wfatal-errors -Wextra -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable
CXXFLAGS = -c -g -O3 -march=native -std=c++1z $(LIBRARY_IPATH) $(WARNINGS)
LDFLAGS = $(LIBRARIES) $(LIBRARY_LPATH)

LIBRARY_IPATH = -I$(INCLUDEDIR)
LIBRARY_LPATH =
LIBRARIES =

# project directory variables
DEPSDIR = deps
OBJSDIR = objs
OUTDIR = out

INCLUDEDIR = include
SRCDIR = src
MODULEDIR = modules

TMPDIRS = $(DEPSDIR) $(OBJSDIR) $(OUTDIR)
SRCDIRS = $(INCLUDEDIR) $(SRCDIR) $(MODULEDIR)

# executable file name
MAIN = main

# object and module file names
EXCLUDE_OBJS = common
OBJS = $(filter-out $(EXCLUDE_OBJS),$(patsubst $(INCLUDEDIR)/%.h,%,$(wildcard $(INCLUDEDIR)/*.h))) $(MAIN)
EXCLUDE_MODULES =
MODULES = $(filter-out $(EXCLUDE_MODULES),$(patsubst $(MODULEDIR)/%.cpp,%,$(wildcard $(MODULEDIR)/*.cpp)))

# vpath directives
vpath %.h $(INCLUDEDIR)
vpath %.cpp $(SRCDIR) $(MODULEDIR)

# -------------
# Rules
# -------------
all: $(MAIN) $(MODULES)

# rule to create new workspace
workspace: tmpdirs srcdirs

# rules for making necessary directories
tmpdirs:
	@mkdir -p $(TMPDIRS)

srcdirs:
	@mkdir -p $(SRCDIRS)

# rule for making the main program
$(MAIN): $(patsubst %,$(OBJSDIR)/%.o,$(OBJS))
	$(LD) $^ $(LDFLAGS) -o $@

# rule for making linux executables
$(MODULES): %: $(OBJSDIR)/%.o
	$(LD) $^ $(LDFLAGS) -o $@

# rule for making .o files from .cpp files (linux version)
$(OBJSDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -MP -MMD -MF $(DEPSDIR)/$(*F).d

# include generated dependencies
-include $(DEPSDIR)/*.d

# rules for cleaning workspace
clean:
	rm -f $(subst $(SPACE),/*$(SPACE),$(TMPDIRS))/* $(MAIN) $(MODULES)

.PHONY: all workspace tmpdirs srcdirs clean
.SUFFIXES: