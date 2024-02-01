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
    Client(int port) : port(port) {
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
        InetPton(AF_INET, serverIp, &serverAddr.sin_addr);
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
        std::string assetsFolderPath = "assets";
        if (!std::filesystem::exists(assetsFolderPath)) {
            std::filesystem::create_directory(assetsFolderPath);
            std::cout << "Created 'assets' folder." << std::endl;
        }
        directoryPath = std::filesystem::absolute(assetsFolderPath).string();
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

    void getFileFromServer(const std::string& fileName) {
        sendCommand("GET " + fileName);
        std::streamsize fileSize;
        recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
        std::cout << "Received file size " << fileSize << " from server." << std::endl;

        const int chunkSize = 1024;
        char buffer[chunkSize];
        std::ofstream file(directoryPath + "\\" + fileName, std::ios::binary);
        std::streamsize totalBytesReceived = 0;
        while (totalBytesReceived < fileSize) {
            int bytesReceived = recv(clientSocket, buffer, chunkSize, 0);
            if (bytesReceived > 0) {
                file.write(buffer, bytesReceived);
                totalBytesReceived += bytesReceived;
            }
        }
        file.close();
        std::cout << "Received file " << fileName << " from server." << std::endl;
    }

    void sendFileToServer(const std::string& fileName) {
        std::ifstream file(directoryPath + "\\" + fileName, std::ios::binary);
        if (file.is_open()) {
            sendCommand("PUT " + fileName);
            std::streamsize fileSize = std::filesystem::file_size(directoryPath + "\\" + fileName);
            send(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            std::cout << "Sent file size " << fileSize << " to server." << std::endl;

            const int chunkSize = 1024;
            char buffer[chunkSize];
            while (!file.eof()) {
                file.read(buffer, chunkSize);
                send(clientSocket, buffer, static_cast<int>(file.gcount()), 0);
            }
            file.close();
            std::cout << "Sent file " << fileName << " to server." << std::endl;
        }
    }


    void deleteFileFromServer(const std::string& fileName) {
        sendCommand("DELETE " + fileName);
        char buffer[1024];
        memset(buffer, 0, 1024);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << std::string(buffer, bytesReceived) << std::endl;
        }
    }

    void getFileInfoFromServer(const std::string& fileName) {
        sendCommand("INFO " + fileName);
        char buffer[1024];
        memset(buffer, 0, 1024);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << std::string(buffer, bytesReceived) << std::endl;
        }
    }

    void quitProgram() {
        sendCommand("QUIT");
    }

private:
    std::string directoryPath;
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    PCWSTR serverIp = L"127.0.0.1";;
    int port;
};

int main() {
    std::string command;
    
        int port = 12345;
        Client client(port);
        std::cout << "Enter command: ";
        std::cin >> command;
        if (command == "GET") {
            std::string fileName;
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            client.getFileFromServer(fileName);
        }
        else if (command == "PUT") {
            std::string fileName;
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            client.sendFileToServer(fileName);
        }
        else if (command == "DELETE") {
            std::string fileName;
            std::cout << "Enter file name to delete: ";
            std::cin >> fileName;
            client.deleteFileFromServer(fileName);
        }
        else if (command == "INFO") {
            std::string fileName;
            std::cout << "Enter file name to get info: ";
            std::cin >> fileName;
            client.getFileInfoFromServer(fileName);
        }
        else if (command == "LIST") {
            std::cout << "Received file list from server:\n" << client.listFilesFromServer() << std::endl;
        }
        else if (command == "QUIT") {
            client.quitProgram();
            
        }
    
    return 0;
}
