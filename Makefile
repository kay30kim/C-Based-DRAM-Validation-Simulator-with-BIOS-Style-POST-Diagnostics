CC = cc
TARGET = dram_test

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
DRAM_MB = 64

OBJS = main.o dram_model.o memory_test.o logger.o

all: $(TARGET)

$(TARGET): main.o dram_model.o memory_test.o logger.o
	$(CC) $(CFLAGS) main.o dram_model.o memory_test.o logger.o -o $(TARGET)

main.o: main.c dram_model.h memory_test.h logger.h
	$(CC) $(CFLAGS) -c main.c -o main.o

dram_model.o: dram_model.c dram_model.h
	$(CC) $(CFLAGS) -c dram_model.c -o dram_model.o

memory_test.o: memory_test.c memory_test.h dram_model.h
	$(CC) $(CFLAGS) -c memory_test.c -o memory_test.o

logger.o: logger.c logger.h memory_test.h
	$(CC) $(CFLAGS) -c logger.c -o logger.o

run: all
	./$(TARGET) $(DRAM_MB)

clean:
	rm -f $(TARGET) $(OBJS) dram_test_results.csv
