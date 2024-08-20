#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#define datagramSize 10
#define PORT 12345
#define PATH "toSend.txt"

uint16_t calculateCheckSum(char *buffer, int bufferSize)
{
    uint32_t sum = 0;
    for (int i = 0; i < bufferSize; i += 2)
    {
        uint16_t word = (buffer[i] << 8) | (buffer[i + 1]);
        sum += word;
        if (sum > 0xFFFF)
            sum = (sum & 0xFFFF) + (sum >> 16);
    }
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return sum;
}

void handleFileTransfer(int sockfd, sockaddr_in &client_adress)
{
    std::ifstream inputFile(PATH);
    char buffer[datagramSize + 2];
    while (!inputFile.eof())
    {
        memset(buffer, 0, datagramSize + 2);
        if (inputFile.read(buffer, datagramSize) || inputFile.gcount() > 0)
        {
            int bytes_read = inputFile.gcount();
            uint16_t checksum = calculateCheckSum(buffer, bytes_read);
            checksum = htons(checksum);
            memcpy(buffer + datagramSize, &checksum, sizeof(uint16_t)); // [0x12, 0x34,...,checksum]
            sendto(sockfd, buffer, datagramSize + 2, 0, (struct sockaddr *)&client_adress, sizeof(sockaddr));
            std::cout << "Checksum: " << std::hex << checksum << std::endl;
            std::cout << std::dec << "Datagram: ";
            for (const auto &byte : buffer)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << "0x"
                          << static_cast<int>(static_cast<unsigned char>(byte)) << " ";
            }
            std::cout << std::dec << std::endl;
            std::cout << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::string endMsg = "EOF_TERM";
    memset(buffer, 0, datagramSize + 2);
    memcpy(buffer, endMsg.c_str(), endMsg.length());
    uint16_t checksum = calculateCheckSum(buffer, endMsg.length());
    checksum = htons(checksum);
    memcpy(buffer + datagramSize, &checksum, sizeof(uint16_t));
    sendto(sockfd, buffer, datagramSize + 2, 0, (struct sockaddr *)&client_adress, sizeof(sockaddr));
    std::cout << "Checksum: " << std::hex << checksum << std::endl;
    std::cout << std::dec << "Datagram: ";
    for (const auto &byte : buffer)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << "0x"
                  << static_cast<int>(static_cast<unsigned char>(byte)) << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << std::endl;
    inputFile.close();
}

int main()
{
    // handleFileTransfer();
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    // 1. create a socket for the server
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. set the server address sturct
    memset(&server_addr, 0, sizeof(sockaddr_in));
    memset(&client_addr, 0, sizeof(sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. bind the socket
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(sockaddr_in));
    std::cout << "Server is running on port " << PORT << std::endl;

    // server loop
    while (true)
    {
        socklen_t client_addr_len = sizeof(sockaddr_in);
        int bytes_recv = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        buffer[bytes_recv] = '\0';
        if (std::string(buffer) == "getfile")
        {
            handleFileTransfer(sockfd, client_addr);
        }
    }
    return 0;
}