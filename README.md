# UDP File Transfer System

## Overview
This project implements a file transfer system using UDP sockets, showcasing a server (C++) and client (Python) architecture. The system emphasizes reliability in an unreliable protocol by incorporating checksum verification for data integrity.

## Features
- **Checksum Verification**: Ensures data integrity by verifying each chunk of data with a calculated checksum.
- **UDP Protocol**: Utilizes UDP for fast data transfer, which is critical for real-time applications but lacks built-in reliability.
- **Cross-language Implementation**: Demonstrates interoperability between C++ and Python.

## Requirements
- C++ Compiler (g++, clang++, etc.)
- Python 3.x

## Compilation
Compile the C++ server with the following command:
```bash
g++ -std=c++11 -g -o server server.cpp
```
## Running the Application

### Server (C++)
Ensure the server executable is present in your directory. Run the server with:
```bash
./server
```
The server will listen on UDP port 12345 for incoming client requests.

### Client (Python)
Run the Python script to start the client:
```bash
python client.py
```
The client allows the user to send a 'getfile' command to request a file transfer. Received data will be saved to `TransmittedFile.txt`, checking each data chunk for integrity.

## How It Works
- **Server**: Listens for a 'getfile' command. Upon receiving this command, it reads the specified file in chunks, computes a checksum for each chunk, and sends the data with the checksum to the client.
- **Client**: Sends a 'getfile' command to the server and waits to receive file chunks. It verifies the checksum of each chunk before writing the data to a file. If checksum verification fails, it logs a warning but still writes the data.

## Closing Notes
This project serves as a simple demonstration of implementing a file transfer mechanism over UDP, highlighting the challenges and solutions in ensuring data integrity without TCP's reliability features.
