#include <iostream>
#include <fstream>
#include <filesystem>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class Client {
public:
    Client(const std::wstring& serverIp, int port) : serverIp(serverIp), port(port) {
        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            exit(1);
        }
        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }
        // Connect to the server
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        InetPton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
    }

    ~Client() {
        closesocket(clientSocket);
        WSACleanup();
    }

    void sendCommand(const std::string& command) {
        send(clientSocket, command.c_str(), static_cast<int>(command.length()), 0);
    }

    std::string listFilesFromServer() {
        sendCommand("LIST");
        char buffer[1024];
        memset(buffer, 0, 1024);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            return std::string(buffer);
        }
        return "";
    }

    void quitProgram() {
        sendCommand("QUIT");
    }

private:
    std::string directoryPath = "C:\\Labs_Kse\\CST\\task1\\client\\assets";
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    std::wstring serverIp;
    int port;
};

int main() {

    std::string command;
    while (true) {
        std::wstring serverIp = L"127.0.0.1";
        int port = 12345;
        Client client(serverIp, port);
        std::cout << "Enter command: ";
        std::cin >> command;
        if (command == "LIST") {
            std::cout << "Received file list from server:\n" << client.listFilesFromServer() << std::endl;
        }
        else if (command == "QUIT") {
            client.quitProgram();
            break;
        }
    }
    return 0;
}
