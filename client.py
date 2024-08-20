import socket
import struct

def calculateChecksum(buffer):
    sum = 0
    for i in range(0,len(buffer), 2):
        word = (buffer[i] << 8) + (buffer[i+1])
        word &= 0xFFFF
        sum += word
        sum &= 0xFFFFFFFF
        if sum > 0xFFFF:
            sum = (sum & 0xFFFF) + (sum >> 16)
    while sum >> 16:
        sum = (sum & 0xFFFF) + (sum >> 16)
    return sum & 0xFFFF


PORT = 12345
HOST = 'localhost'
datagramSize = 10

sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    command = input("Enter the 'getfile' Command to send to server and get file: ")
    if command == "getfile":
        sockfd.sendto(command.encode(), (HOST, PORT))
        with open("TransmittedFile.txt", 'w') as file:
            while True:
                data, addr = sockfd.recvfrom(1024)
                if (data[:len("EOF_TERM")] == b"EOF_TERM"):
                    break
                dataBytes, checksum = struct.unpack(f">{datagramSize}sH", data)
                if calculateChecksum(dataBytes) != checksum:
                    print("Detected a corruption! but stil copying to the file")
                file.write(dataBytes.decode().strip('\0'))