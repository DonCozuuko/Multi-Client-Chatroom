main:
	gcc server.c -o s -lws2_32 && gcc client.c -o c -lws2_32
