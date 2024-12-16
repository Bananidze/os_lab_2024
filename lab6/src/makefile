# Определяем компилятор и флаги
CC = gcc
LDFLAGS = -lpthread

# Определяем имена выходных файлов
CLIENT = client
SERVER = server

# Исходные файлы
CLIENT_SRC = client.c utils.c
SERVER_SRC = server.c utils.c

# Целевая установка по умолчанию
all: $(CLIENT) $(SERVER)

# Правила для компиляции клиента
$(CLIENT): $(CLIENT_SRC)
	$(CC) -o $(CLIENT) $(CLIENT_SRC) $(LDFLAGS)

# Правила для компиляции сервера
$(SERVER): $(SERVER_SRC)
	$(CC) -o $(SERVER) $(SERVER_SRC) $(LDFLAGS)

# Правила для очистки скомпилированных файлов
clean:
	rm -f $(CLIENT) $(SERVER)

.PHONY: all clean