DATE=$(shell date +"%Y%m%d-%H%M%S")

SRCDIR=src
OBJDIR=obj
PROFDIR=test/profiles

CC=g++
CPPFLAGS=-pg -fsanitize=address -Wall --pedantic -I/usr/include/libdrm -Isrc -std=c++11
LDFLAGS=-pg -fsanitize=address -ldrm
TARGET=drm_test

SRCS=$(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CC) $(CPPFLAGS) -c $< -o $@

prof: $(TARGET) $(PROFDIR)
	-./$(TARGET)
	gprof ./$(TARGET) | gprof2dot | dot -Tpng -o $(PROFDIR)/profile-$(DATE).png
	rm gmon.out

.PHONY : all clean prof
clean:
	rm -f $(TARGET) $(OBJS)
