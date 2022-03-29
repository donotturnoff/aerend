DATE=$(shell date +"%Y%m%d-%H%M%S")

SERVER_SRCDIR=src/server
SERVER_OBJDIR=obj/server
CLIENT_SRCDIR=src/client
CLIENT_OBJDIR=obj/client
TEST_SRCDIR=src/test
TEST_OBJDIR=obj/test
PROFDIR=test/profiles

SERVER_CC=g++
SERVER_CPPFLAGS=-Wall --pedantic -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libdrm -ldrm -lfreetype -Isrc/server -std=c++17
SERVER_LDFLAGS=-ldrm -lfreetype -lpthread
SERVER_TARGET=gui_test

CLIENT_CC=musl-gcc
CLIENT_DEBUG_CC=gcc
CLIENT_AR=ar
CLIENT_CFLAGS=-static -s -flto -Wall --pedantic -Isrc/client -std=c11 -O3
CLIENT_DEBUG_CFLAGS=-Wall --pedantic -Isrc/client -std=c11 -pg -fsanitize=address
CLIENT_ARFLAGS=-cv
CLIENT_TARGET=libaerend.a

TEST_CC=musl-gcc
TEST_DEBUG_CC=gcc
TEST_CFLAGS=-static -s -flto -Wall --pedantic -L. -laerend -Isrc/test -Isrc/client -std=c11
TEST_DEBUG_CFLAGS=-Wall --pedantic -L. -laerend -lm -Isrc/test -Isrc/client -std=c11 -pg -fsanitize=address
TEST_LDFLAGS=-s -flto -L. -laerend
TEST_DEBUG_LDFLAGS=-L. -laerend -lm  -pg -fsanitize=address

SERVER_SRCS=$(wildcard $(SERVER_SRCDIR)/gui_test.cpp $(SERVER_SRCDIR)/AerendServer.cpp $(SERVER_SRCDIR)/*/*.cpp)
SERVER_OBJS=$(patsubst $(SERVER_SRCDIR)/%.cpp,$(SERVER_OBJDIR)/%.o,$(SERVER_SRCS))

CLIENT_SRCS=$(wildcard $(CLIENT_SRCDIR)/*.c)
CLIENT_OBJS=$(patsubst $(CLIENT_SRCDIR)/%.c,$(CLIENT_OBJDIR)/%.o,$(CLIENT_SRCS))

TEST_SRCS=$(wildcard $(TEST_SRCDIR)/*.c $(TEST_SRCDIR)/*/*.c)

all: SERVER_CPPFLAGS += -O3
all: CLIENT_CFLAGS += -O3
all: TEST_CFLAGS += -O3
all: $(SERVER_TARGET) $(CLIENT_TARGET) mem_test instr_test pcap_test

debug: SERVER_CPPFLAGS += -pg -fsanitize=address
debug: SERVER_LDFLAGS += -pg -fsanitize=address
debug: CLIENT_CC = $(CLIENT_DEBUG_CC)
debug: CLIENT_CFLAGS = $(CLIENT_DEBUG_CFLAGS)
debug: TEST_CC = $(TEST_DEBUG_CC)
debug: TEST_CFLAGS = $(TEST_DEBUG_CFLAGS)
debug: TEST_LDFLAGS = $(TEST_DEBUG_LDFLAGS)
debug: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(SERVER_CC) $^ $(SERVER_LDFLAGS) -o $@

$(SERVER_OBJDIR)/%.o: $(SERVER_SRCDIR)/%.cpp
	$(SERVER_CC) -c $< $(SERVER_CPPFLAGS) -o $@

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CLIENT_AR) $(CLIENT_ARFLAGS) -r $@ $^

$(CLIENT_OBJDIR)/%.o: $(CLIENT_SRCDIR)/%.c
	$(CLIENT_CC) -c $< $(CLIENT_CFLAGS) -o $@

mem_test: $(TEST_SRCDIR)/widget_perf/mem_test.c
	$(TEST_CC) $< $(TEST_CFLAGS) -o $@

instr_test: $(TEST_SRCDIR)/widget_perf/instr_test.c
	$(TEST_CC) $< $(TEST_CFLAGS) -o $@

pcap_test: $(TEST_SRCDIR)/widget_perf/pcap_test.c
	gcc $< -Wall --pedantic -L. -laerend -Isrc/test -Isrc/client -std=c11 -D_GNU_SOURCE -lpcap -lpthread -fsanitize=address -o $@

prof: $(SERVER_TARGET) $(PROFDIR)
	-./$(SERVER_TARGET)
	gprof ./$(SERVER_TARGET) | gprof2dot | dot -Tpng -o $(PROFDIR)/profile-$(DATE).png
	rm -f gmon.out

.PHONY : all clean prof
clean:
	rm -f $(SERVER_TARGET) $(SERVER_OBJS) $(CLIENT_TARGET) $(CLIENT_OBJS) $(TEST_OBJS)
	rm -f gmon.out
	rm -rf asm
