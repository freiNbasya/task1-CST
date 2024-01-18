#include <iostream>
#include <filesystem>
#include <vector>
#include <WinSock2.h>
#include <fstream>
#include <ctime>
#include <string>
#pragma comment(lib, "ws2_32.lib")

class Server {
public:
    Server(int port) : port(port) {
        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            exit(1);
        }
        // Create socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }
        // Bind the socket
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(1);
        }
        // Listen for incoming connections
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(1);
        }
        std::cout << "Server listening on port " << port << std::endl;
    }

    ~Server() {
        closesocket(serverSocket);
        WSACleanup();   
    }

    void acceptClient() {
        // Accept a client connection
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(1);
        }
    }



    bool getLoop() {
        return loop;
    }

private:
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    std::string directoryPath = "C:\\Labs_Kse\\CST\\task1\\Server\\assets";
    bool loop = true;
    int port;



};

int main() {

    while (true) {
        int port = 12345;
        Server server(port);
        server.acceptClient();
        server.handleClientRequest();
        if (server.getLoop() == false) {
            break;
        }
    }
    return 0;
}
