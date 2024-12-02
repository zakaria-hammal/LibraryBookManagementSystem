To Compile : gcc $(pkg-config --cflags gtk4) -o prog library.c stack.c queue.c $(pkg-config --libs gtk4)
