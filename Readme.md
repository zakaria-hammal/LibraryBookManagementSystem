# GTK4 C Program

A Simple Library Mangement Program for school project created in C using GTK4 for the graphical user interface (GUI).

---

## Prerequisites

Before you can compile and run this project, ensure the following tools and libraries are installed:

1. **GCC**: The GNU Compiler Collection.
2. **GTK4**: The GTK4 development libraries.
3. **pkg-config**: Used to retrieve the necessary compiler and linker flags.

### Compiling the Program
To compile the program, use the following command:
```bash
gcc $(pkg-config --cflags gtk4) -o prog library.c stack.c queue.c $(pkg-config --libs gtk4)
```

After compiling, run the program with:
```bash
./prog  