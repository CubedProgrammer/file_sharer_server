# file\_sharer\_server
Server for sharing files.
## Compilation
```
clang -O3 -c *.c
clang -o fsserver *.o
```
## Usage
Just run the program.
Enviornment variable SHAREBUFSZ determines how much data to store in memory before sending data to receipients.
Defaults to 16384, increasing costs more memory.
Client may also change this variable to send more data at once with fewer system calls.
