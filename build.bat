call clean.bat
gcc -c *.c
gcc *.o -lws2_32 -o main.exe
