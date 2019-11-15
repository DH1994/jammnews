jammnews: main.c
	gcc -o jammnews main.c daemon.c functions.c jsonparser.c buffer.c tcpConnection.c silenceDetect.c 3dparty/b64/encode.c 3dparty/b64/decode.c 3dparty/cJSON/cJSON.c -lpthread -I.
