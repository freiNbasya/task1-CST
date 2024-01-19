Protocol
LIST command:
	-  Client sends string “LIST” to the server
-	Server receives the command goes through it’s assets directory and returns list of all files to the client
GET command:
-	Client sends command that consists of both string “GET” + name of desirable file to the server in the same string.
-	Server receives the string, parses it to separate file name, finds the needed file, calculates it’s size and sends the size to the client.
-	Client receives the size and creates a buffer with the size of the file.
-	Server creates a buffer with file size and writes file inside. After that server sends it to the client.
-	Client writes all received data in the buffer and then rewrites buffer in separate file in it’s assets folder.
PUT command:
-	Client sends command that consists of both string “PUT” + name of desirable file to the server in the same string, calculates it’s size and sends the size to the server.
-	Server receives the string, parses it to separate file name, server receives the size and creates a buffer with the size of the file.
-	Client creates a buffer with file size and writes file inside. After that server sends it to the server.
-	Server writes all received data in the buffer and then rewrites buffer in separate file with the initial name of file in it’s assets folder.


DELETE command:
-	Client sends command that consists of both string “DELETE” + name of desirable file to the server in the same string”.
-	Server receives the string, parses it to separate file name, find the correct file, deletes it and sends confirmation to the client.
-	Client receives the confirmation.
INFO command:
-	Client sends command that consists of both string “INFO” + name of desirable file to the server in the same string”.
-	Server receives the string, parses it to separate file name, find the correct file, calculates it’s size, sends the string with information to the client.
-	Client receives information about desired file.
QUIT command:
	-  Client sends string “QUIT” to the server, after which it breaks it’s while (true) loop and shuts down.
-	Server receives command and breaks it’s while (true) loop and shuts down.

