CC          = arm-linux-gnueabihf-gcc
#CC          = arm-unknown-linux-uclibcgnueabihf-gcc
LINKER   	= $(CC) -o

CFLAGS   	= -O2 -x c -Wall
LFLAGS   	= -Wall -lm -lrt 

# change these to set the proper directories where each files should be
SRCDIR     = .
OBJDIR     = ./bin
BINDIR     = ./bin
INSTALLDIR = /bin/
TARGET     := smartcfg

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET)
	@echo "$(BINDIR)/$(TARGET): done"

clean:
	rm -f $(OBJECTS)	

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	LT=$<
	AT=$@
	mkdir -p $(OBJDIR)	
	@echo "Compile $@ -> $< ..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compile $@ -> $< done."
	
$(BINDIR)/$(TARGET): $(OBJECTS)
	LT=$<
	AT=$@
	mkdir -p $(BINDIR)	
	@echo "Linking $(OBJECTS) -> $(BINDIR)/$(TARGET) ..."
	$(LINKER) $(BINDIR)/$(TARGET) $(LFLAGS) $(OBJECTS)
	@echo "Linking $(BINDIR)/$(TARGET) done."
