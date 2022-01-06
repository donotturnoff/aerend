DATE=$(shell date +"%Y%m%d-%H%M%S")

SRCDIR=src
OBJDIR=obj
ASMDIR=asm
PROFDIR=test/profiles

CC=g++
CPPFLAGS=-Wall --pedantic -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libdrm -ldrm -lgbm -lfreetype -Isrc -std=c++17
LDFLAGS=-ldrm -lgbm -lfreetype -lpthread
TARGET=gui_test

SRCS=$(wildcard $(SRCDIR)/gui_test.cpp $(SRCDIR)/AerendServer.cpp $(SRCDIR)/*/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
ASMS=$(patsubst $(SRCDIR)/%.cpp,$(ASMDIR)/%.S,$(SRCS))

all: CPPFLAGS += -O3
all: $(TARGET)

debug: CPPFLAGS += -pg -fsanitize=address
debug: LDFLAGS += -pg -fsanitize=address
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

prof: $(TARGET) $(PROFDIR)
	-./$(TARGET)
	gprof ./$(TARGET) | gprof2dot | dot -Tpng -o $(PROFDIR)/profile-$(DATE).png
	rm -f gmon.out

.PHONY : all clean prof
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f gmon.out
	rm -rf asm
