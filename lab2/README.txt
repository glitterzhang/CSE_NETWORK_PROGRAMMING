There are 2 scource files:
1. ftpc.c The client which is for sending file
2. ftps.c The server, which is for receiving file.

To run the program:

Step 1, please run make to compile the source files.

Step 2, run server,type:
./ftps <local-port>
For example:
./ftps <4000>

step 3 run client,type:
./ftpc <remote-IP> <remote-port> <local-file-to-transfer>
For example:
./ftpc 192.168.0.17 4000 1.jpg
