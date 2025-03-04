CC = gcc
CFLAGS = -Wall -g 
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

OBJS = shellMain.o fileList.o HistoList.o auxFunctions.o memoryList.o processList.o dirList.o

TARGET = shell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

shellMain.o: shellMain.c shellMain.h globals.h types.h fileList.h HistoList.h memoryList.h
	$(CC) $(CFLAGS) -c shellMain.c

fileList.o: fileList.c fileList.h globals.h types.h
	$(CC) $(CFLAGS) -c fileList.c

HistoList.o: HistoList.c HistoList.h globals.h types.h
	$(CC) $(CFLAGS) -c HistoList.c

auxFunctions.o: auxFunctions.c auxFunctions.h globals.h types.h
	$(CC) $(CFLAGS) -c auxFunctions.c

memoryList.o: memoryList.c memoryList.h globals.h types.h
	$(CC) $(CFLAGS) -c memoryList.c

processList.o: processList.c processList.h globals.h types.h
	$(CC) $(CFLAGS) -c processList.c

clean:
	rm -f $(OBJS) $(TARGET)

clean_objs:
	rm -f $(OBJS)

clean_exec:
	rm -f $(TARGET)

valgrind: $(TARGET)
	valgrind $(VALGRIND_FLAGS) ./$(TARGET)