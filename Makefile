CC = cc
TARGET = dram_test

CFLAGS = -std=c11 -Wall -Wextra -Werror -O2
DRAM_MB = 64

OBJS = main.o dram_model.o memory_test.o

all: $(TARGET)

$(TARGET): main.o dram_model.o memory_test.o
	$(CC) $(CFLAGS) main.o dram_model.o memory_test.o -o $(TARGET)

main.o: main.c dram_model.h memory_test.h
	$(CC) $(CFLAGS) -c main.c -o main.o

dram_model.o: dram_model.c dram_model.h
	$(CC) $(CFLAGS) -c dram_model.c -o dram_model.o

memory_test.o: memory_test.c memory_test.h dram_model.h
	$(CC) $(CFLAGS) -c memory_test.c -o memory_test.o

run: all
	./$(TARGET) $(DRAM_MB)

clean:
	rm -f $(TARGET) $(OBJS)
