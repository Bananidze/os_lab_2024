# Определяем компилятор и флаги
CC = gcc
CFLAGS = -Wall -g -pthread

# Исходные файлы
SRCS = parallel_sum.c sum.c utils.c
# Объектные файлы
OBJS = $(SRCS:.c=.o)
# Имя исполняемого файла
TARGET = psum

# Правило по умолчанию
all: $(TARGET)

# Правило для создания исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Правило для создания объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(OBJS) $(TARGET)

# Правило для запуска программы (можно изменить аргументы по умолчанию)
run: $(TARGET)
	./$(TARGET) -t 4 -s 12345 -a 1000000

.PHONY: all clean run