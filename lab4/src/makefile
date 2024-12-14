# # Определяем компилятор и флаги
# CC = gcc
# CFLAGS = -Wall -g -pthread

# # Исходные файлы
# SRCS = parallel_sum.c sum.c utils.c
# # Объектные файлы
# OBJS = $(SRCS:.c=.o)
# # Имя исполняемого файла
# TARGET = psum

# # Правило по умолчанию
# all: $(TARGET)

# # Правило для создания исполняемого файла
# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) -o $@ $^

# # Правило для создания объектных файлов
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # Правило для очистки сгенерированных файлов
# clean:
# 	rm -f $(OBJS) $(TARGET)

# # Правило для запуска программы (можно изменить аргументы по умолчанию)
# run: $(TARGET)
# 	./$(TARGET) -t 4 -s 12345 -a 1000000

# .PHONY: all clean run
# Определяем компилятор и флаги
CC = gcc
CFLAGS = -Wall -g -pthread

# Исходные файлы
SRCS = parallel_sum.c sum.c utils.c process_memory.c parallel_min_max.c zombie.c
# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Имена исполняемых файлов
TARGETS = parallel_sum process_memory parallel_min_max zombie

# Правило по умолчанию
all: $(TARGETS)

# Правило для создания исполняемого файла parallel_sum
parallel_sum: parallel_sum.o sum.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Правило для создания исполняемого файла process_memory
process_memory: process_memory.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Правило для создания исполняемого файла parallel_min_max
parallel_min_max: parallel_min_max.o sum.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Правило для создания исполняемого файла zombie
zombie: zombie.o
	$(CC) $(CFLAGS) -o $@ $^

# Правило для создания объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(OBJS) $(TARGETS)

# Правило для запуска программ (можно изменить аргументы по умолчанию)
run_parallel_sum: parallel_sum
	./parallel_sum -t 4 -s 12345 -a 1000000

run_process_memory: process_memory
	./process_memory

run_zombie: zombie
	./zombie

run_parallel_min_max: parallel_min_max
	./parallel_min_max --seed 42 --array_size 100 --pnum 4 --timeout 5

.PHONY: all clean run_parallel_sum run_process_memory run_parallel_min_max