DATE=$(shell date +"%Y%m%d-%H%M%S")

SERVER_SRCDIR=src/server
SERVER_OBJDIR=obj/server
CLIENT_SRCDIR=src/client
CLIENT_OBJDIR=obj/client

SERVER_CC=g++
SERVER_CPPFLAGS=-Wall --pedantic -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libdrm -ldrm -lfreetype -Isrc/server -std=c++17
SERVER_LDFLAGS=-ldrm -lfreetype -lpthread
SERVER_TARGET=bin/aerend

CLIENT_CC=musl-gcc
CLIENT_DEBUG_CC=gcc
CLIENT_AR=ar
CLIENT_CFLAGS=-static -s -flto -Wall --pedantic -Isrc/client -std=c11 -O3
CLIENT_DEBUG_CFLAGS=-Wall --pedantic -Isrc/client -std=c11 -fsanitize=address
CLIENT_ARFLAGS=-cv
CLIENT_TARGET=lib/libaerend.a

SERVER_SRCS=$(wildcard $(SERVER_SRCDIR)/AerendServer.cpp $(SERVER_SRCDIR)/*/*.cpp)
SERVER_OBJS=$(patsubst $(SERVER_SRCDIR)/%.cpp,$(SERVER_OBJDIR)/%.o,$(SERVER_SRCS))

CLIENT_SRCS=$(wildcard $(CLIENT_SRCDIR)/*.c)
CLIENT_OBJS=$(patsubst $(CLIENT_SRCDIR)/%.c,$(CLIENT_OBJDIR)/%.o,$(CLIENT_SRCS))

all: SERVER_CPPFLAGS += -O3
all: CLIENT_CFLAGS += -O3
all: $(SERVER_TARGET) $(CLIENT_TARGET)

debug: SERVER_CPPFLAGS += -pg -fsanitize=address
debug: SERVER_LDFLAGS += -pg -fsanitize=address
debug: CLIENT_CFLAGS += -pg -fsanitize=address
debug: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(SERVER_CC) $^ $(SERVER_LDFLAGS) -o $@

$(SERVER_OBJDIR)/%.o: $(SERVER_SRCDIR)/%.cpp $(SERVER_SRCDIR)/%.h
	$(SERVER_CC) -c $< $(SERVER_CPPFLAGS) -o $@

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CLIENT_AR) $(CLIENT_ARFLAGS) -r $@ $^

$(CLIENT_OBJDIR)/%.o: $(CLIENT_SRCDIR)/%.c
	$(CLIENT_CC) -c $< $(CLIENT_CFLAGS) -o $@

.PHONY : all clean debug
clean:
	rm -f $(SERVER_TARGET) $(SERVER_OBJS) $(CLIENT_TARGET) $(CLIENT_OBJS)
