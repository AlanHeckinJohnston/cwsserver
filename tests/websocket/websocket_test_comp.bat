cd %~dp0/../../
gcc -I. tests/websocket/websocket_test.c websocket.c headerprocessor.c array.c client_management.c socket_info.c lib/obj/cencode.o lib/obj/sha1.o -o b.exe -lws2_32