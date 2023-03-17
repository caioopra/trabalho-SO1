CC=g++
CFLAGS=-c -Wall
SRCDIR=src
INCDIR=include
BUILDDIR=build
SOURCES=$(wildcard $(SRCDIR)/*.cc) $(wildcard $(INCDIR)/*.cc)
OBJECTS=$(patsubst %.cc, $(BUILDDIR)/%.o, $(notdir $(SOURCES)))
EXECUTABLE=trabalho1_SO

build: directories $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BUILDDIR)/$@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc | directories
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(INCDIR)/%.cc | directories
	$(CC) $(CFLAGS) $< -o $@

.PHONY: directories
directories:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)/*

.PHONY: clean
