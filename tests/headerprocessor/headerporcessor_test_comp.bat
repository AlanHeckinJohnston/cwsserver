cd %~dp0/../../
gcc -I. tests/headerprocessor/headerprocessor_test.c websocket.c headerprocessor.c array.c lib/obj/cencode.o lib/obj/sha1.o -o b.exe