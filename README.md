# Multi-Client Chatroom
My implementation of a **SIMPLE** real-time communication system in C. It allows multiple clients connect and chat to each other over TCP. Uses the winsock2 header library for socket functionality, so it only builds on Windows :(

## Features I find Neat (maybe not you)
- The terminal that is running the server.exe displays all the messages and connection statuses of all the clients.
- All built in C, so there are a lot of exploitable things you could do on the client side (I made sure to include a shit ton of buffer handling bad practices).
- It works.
## Demo
https://github.com/user-attachments/assets/60cb4c4e-d5e4-4000-8a71-4bbc22355f09

## Quick Start
- Must open at least 2 terminal instances, one for the server and at least one for the clients.
```
$ ./server
START OF LOG
<----Listening for Connections---->
```
- In another terminal instance,
```
$ ./client
Connected to Server
Enter Username:
```

## Build from Source
- There is literally no way to mess this up (unless you don't have the make utility installed).
```
$ make
```

