# Compiler and Archiver
CXX = g++
AR = ar

# Compiler flags
CXXFLAGS = -W -fPIC -Iinclude

# Directories
SRCDIR = src
INCDIR = include
LIBDIR = lib

# Library names
STATIC_LIB = $(LIBDIR)/libFMLib.a
DYNAMIC_LIB = $(LIBDIR)/libFMLib.so

# Source files and object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(LIBDIR)/%.o,$(SOURCES))

# Default rule to create both static and dynamic libraries
static_lib: clean $(STATIC_LIB) 
shared_lib: clean $(DYNAMIC_LIB)

# Rule to create the static library
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

# Rule to create the dynamic library
$(DYNAMIC_LIB): $(OBJECTS)
	$(CXX) -shared -o $@ $(OBJECTS)

# Rule to compile .cpp files into .o files
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp | $(LIBDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the lib directory if it doesn't exist
$(LIBDIR):
	mkdir -p $(LIBDIR)

# Clean up generated files
clean:
	rm -rf $(LIBDIR)

# Phony targets
.PHONY: all clean