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
CLIENT_AR=ar
CLIENT_CFLAGS=-static -s -flto -Wall --pedantic -Isrc/client -std=c11
CLIENT_ARFLAGS=-cv
CLIENT_TARGET=libaerend.a

TEST_CC=musl-gcc
TEST_CFLAGS=-static -s -flto -Wall --pedantic -L. -laerend -Isrc/test -Isrc/client -std=c11
TEST_LDFLAGS=-static -s -flto -L. -laerend
TEST_TARGET=net_test

SERVER_SRCS=$(wildcard $(SERVER_SRCDIR)/gui_test.cpp $(SERVER_SRCDIR)/AerendServer.cpp $(SERVER_SRCDIR)/*/*.cpp)
SERVER_OBJS=$(patsubst $(SERVER_SRCDIR)/%.cpp,$(SERVER_OBJDIR)/%.o,$(SERVER_SRCS))

CLIENT_SRCS=$(wildcard $(CLIENT_SRCDIR)/*.c)
CLIENT_OBJS=$(patsubst $(CLIENT_SRCDIR)/%.c,$(CLIENT_OBJDIR)/%.o,$(CLIENT_SRCS))

TEST_SRCS=$(TEST_SRCDIR)/net_test.c
TEST_OBJS=$(TEST_OBJDIR)/net_test.o

all: SERVER_CPPFLAGS += -O3
all: CLIENT_CFLAGS += -O3
all: TEST_CFLAGS += -O3
all: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

debug: SERVER_CPPFLAGS += -pg -fsanitize=address
debug: SERVER_LDFLAGS += -pg -fsanitize=address
debug: CLIENT_CFLAGS += -pg -fsanitize=address
debug: TEST_CFLAGS += -pg -fsanitize=address
debug: TEST_LDFLAGS += -pg -fsanitize=address
debug: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(SERVER_CC) $^ $(SERVER_LDFLAGS) -o $@

$(SERVER_OBJDIR)/%.o: $(SERVER_SRCDIR)/%.cpp
	$(SERVER_CC) -c $< $(SERVER_CPPFLAGS) -o $@

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CLIENT_AR) $(CLIENT_ARFLAGS) -r $@ $^

$(CLIENT_OBJDIR)/%.o: $(CLIENT_SRCDIR)/%.c
	$(CLIENT_CC) -c $< $(CLIENT_CFLAGS) -o $@

$(TEST_TARGET): $(TEST_OBJS) $(CLIENT_TARGET)
	$(TEST_CC) $^ $(TEST_LDFLAGS) -o $@

$(TEST_OBJDIR)/%.o: $(TEST_SRCDIR)/%.c
	$(TEST_CC) -c $< $(TEST_CFLAGS) -o $@

prof: $(SERVER_TARGET) $(PROFDIR)
	-./$(SERVER_TARGET)
	gprof ./$(SERVER_TARGET) | gprof2dot | dot -Tpng -o $(PROFDIR)/profile-$(DATE).png
	rm -f gmon.out

.PHONY : all clean prof
clean:
	rm -f $(SERVER_TARGET) $(SERVER_OBJS) $(CLIENT_TARGET) $(CLIENT_OBJS) $(TEST_TARGET) $(TEST_OBJS)
	rm -f gmon.out
	rm -rf asm
