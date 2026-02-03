CC = gcc
CFLAGS = \
-std=c11 \
-g3 \
-fsanitize=address \
-Wpedantic \
-Wall \
-Wextra \
-Wformat=2 \
-Wsign-conversion \
-Wswitch-enum \
-Wno-switch-default \
-Wunused \
-Wfloat-equal \
-Wconversion \
-Wpointer-arith \
-Wtype-limits \
-Wcast-qual \
-Wenum-compare \
-Wsizeof-pointer-memaccess \
-Wstrict-prototypes \
`pkg-config --cflags check` \
-Wno-unused-parameter
OBJ = main.o bitmap.o
TARGET = bmptogba

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c bitmap.h
	$(CC) $(CFLAGS) -c main.c

bitmap.o: bitmap.c bitmap.h
	$(CC) $(CFLAGS) -c bitmap.c

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
