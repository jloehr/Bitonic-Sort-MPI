
CC = mpicc
CFLAGS = -O3 -std=c99
LDFLAGS = -lm

SOURCEDIR = ./src/
OBJECTDIR = ./obj/
BINDIR = ./bin/

SOURCES = $(wildcard $(SOURCEDIR)*.c)
OBJECTS = $(addprefix $(OBJECTDIR), $(notdir $(SOURCES:.c=.o)))
BINARY = $(BINDIR)bitonic-mpi

all:$(BINARY)
rebuild: clean $(BINARY)

# Link
$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Header Dependencies
# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)
	
# Compile
$(OBJECTDIR)%.o: $(SOURCEDIR)%.c 
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM -w $(CFLAGS) $< > $(OBJECTDIR)$*.d

# Clean
clean:
	rm -f $(BINARY) $(OBJECTDIR)*.o $(OBJECTDIR)*.d
	
	
# Directories
# Objects
$(OBJECTS): | $(OBJECTDIR)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	
# Binary
$(BINARY): | $(BINDIR)

$(BINDIR):
	mkdir -p $(BINDIR)
