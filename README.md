Protocol

LIST command:

-  	Client sends string “LIST” (size: 4 bytes) to the server
-	Server receives the command goes through it’s assets directory and returns list of all files to the client

GET command:
-	Client sends command that consists of both string “GET” + name of desirable file to the server in the same string (size 3 bytes + lenght of file name).
-	Server receives the string, parses it to separate file name, finds the needed file, calculates it’s size and sends the size to the client.
-	Client receives the size and creates a buffer with the size of the file (8 bytes).
-	Server creates a buffer with file size and writes file inside. After that server sends it to the client.
-	Client writes all received data in the buffer and then rewrites buffer in separate file in it’s assets folder.

PUT command:
-	Client sends command that consists of both string “PUT” + name of desirable file to the server in the same string (size 3 bytes + lenght of file name), calculates it’s size and sends the size to the server (8 bytes).
-	Server receives the string, parses it to separate file name, server receives the size and creates a buffer with the size of the file.
-	Client creates a buffer with file size and writes file inside. After that server sends it to the server.
-	Server writes all received data in the buffer and then rewrites buffer in separate file with the initial name of file in it’s assets folder.

DELETE command:
-	Client sends command that consists of both string “DELETE” + name of desirable file to the server in the same string(size 6 bytes + lenght of file name).
-	Server receives the string, parses it to separate file name, find the correct file, deletes it and sends confirmation to the client.
-	Client receives the confirmation.

INFO command:
-	Client sends command that consists of both string “INFO” + name of desirable file to the server in the same string (size 4 bytes + lenght of file name).
-	Server receives the string, parses it to separate file name, find the correct file, calculates it’s size, sends the string with information to the client.
-	Client receives information about desired file.

CREATE command:
-	Client sends command that consists of both string “CREATE” + name of desirable folder name to the server in the same string (size 5 bytes).
-	Server receives the string, parses it to separate folder name, checks if such folder exists, if it doesn't creates one and assigns this folder as this particular client subfolder

QUIT command:
-  	Client sends string “QUIT” to the server, after which it breaks it’s while (true) loop and shuts down.
-	Server receives command and breaks it’s while (true) loop and shuts down.

