# -----------------------------------------------------------------------------
# - Settings
# -----------------------------------------------------------------------------
CC = gcc
LD = gcc

# Flags
CFLAGS = -Wall -Werror
LDFLAGS =

# Project name
PROJECT = test

# Main directory
MAIN_DIR = ..

# Build directory
BUILDDIR = build

# Source files
SRC = main.c \
	  $(MAIN_DIR)/mymap.c \
	  $(MAIN_DIR)/rb_tree.c

# Include dirs
INCDIRS = $(MAIN_DIR)

# Libraries
LIBS = 

# -----------------------------------------------------------------------------
# - Rules
# -----------------------------------------------------------------------------
OBJS = $(addprefix $(BUILDDIR)/, $(notdir $(SRC:.c=.o)))

SRCPATHS = $(dir $(SRC))

VPATH = $(SRCPATHS)

.PHONY: clean

$(BUILDDIR)/$(PROJECT): $(OBJS)
	@echo "Linking..."
	@$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	@echo "Compiling" $(notdir $<)
	@$(CC) -c $(CFLAGS) $(foreach d, $(INCDIRS), -I$d) $< -o $@

$(BUILDDIR):
	@mkdir $(BUILDDIR)
	
clean:
	@rm -rf build
