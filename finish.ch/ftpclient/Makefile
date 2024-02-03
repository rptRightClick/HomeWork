# Имя исполняемого файла
TARGET = ModelistCentr

# Компилятор
CC = gcc

# Опции компилятора для GTK+ 3 и MiniUPnP
CFLAGS = $(shell pkg-config --cflags gtk+-3.0) -Wall -Wextra -std=c99

# Библиотеки для линковки (GTK+ 3, libcurl, MiniUPnP и Glib)
LIBS = $(shell pkg-config --libs gtk+-3.0) -lcurl -lole32 -lshell32 -luuid
#-mwindows
# Опции линковки
LDFLAGS = -mwindows

# Исходные файлы
SRCS = main.c ferstconnect.c loadfile.c parsing.c mach3.c install_manual.c

# Объектные файлы (автоматически сгенерируются из исходных)
OBJS = $(SRCS:.c=.o)

# Имя манифеста
MANIFEST = cnc.manifest

# Правило по умолчанию (создание исполняемого файла)
$(TARGET): $(OBJS) $(MANIFEST)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(LDFLAGS)

# Правило для компиляции .c файлов в .o файлы
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Цель "clean" для удаления объектных файлов и исполняемого файла
clean:
	rm -f $(OBJS) $(TARGET) $(MANIFEST)

# Цель "all" (по умолчанию) компилирует программу
all: $(TARGET)

.PHONY: clean
