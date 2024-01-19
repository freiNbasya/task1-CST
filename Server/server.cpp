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

    void receiveCommand() {
        char buffer[1024];
        memset(buffer, 0, 1024);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::string request = buffer;
            if (request.substr(0, 3) == "GET") {
                std::string fileName = request.substr(4);
                GET(fileName);
            }
            else if (request.substr(0, 3) == "PUT") {
                std::string fileName = request.substr(4);
                PUT(fileName);
            }
            else if (request == "LIST") {
                std::string fileList = LIST(directoryPath);
                std::cout << "Sending file list to client:\n" << fileList << std::endl;
                send(clientSocket, fileList.c_str(), static_cast<int>(fileList.length()), 0);
            }else if (request.substr(0, 6) == "DELETE") {
                std::string fileName = request.substr(7);
                DEL(fileName);
            }else if (request.substr(0, 4) == "INFO") {
                std::string fileName = request.substr(5);
                INFO(fileName);
            }
            else if (request == "QUIT") {
                loop = false;
            }

            
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

    std::string LIST(const std::string& directoryPath) {
        std::string fileList;
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            fileList += entry.path().filename().string() + "\n";
        }
        return fileList;
    }

    void GET(const std::string& fileName) {
        std::ifstream file(directoryPath + "\\" + fileName, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            send(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
            std::cout << "Sent file size " << fileSize << " to client." << std::endl;
            std::vector<char> buffer(fileSize);
            if (file.read(buffer.data(), fileSize)) {
                send(clientSocket, buffer.data(), static_cast<int>(fileSize), 0);
                std::cout << "Sent file " << fileName << " to client." << std::endl;
            }
            else {
                std::cerr << "Error reading file " << fileName << std::endl;
            }
            file.close();
        }
        else {
            std::cerr << "Unable to open file " << fileName << std::endl;
        }
    }

    void PUT(const std::string& fileName) {

        std::streamsize fileSize;
        recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
        std::cout << "Received file size " << fileSize << " from server." << std::endl;
        std::vector<char> buffer(fileSize);
        std::streamsize totalBytesReceived = 0;
        while (totalBytesReceived < fileSize) {
            int bytesReceived = recv(clientSocket, buffer.data() + totalBytesReceived, static_cast<int>(fileSize - totalBytesReceived), 0);
            if (bytesReceived > 0) {
                totalBytesReceived += bytesReceived;
            }
            else if (bytesReceived == 0) {
                std::cerr << "Connection closed by client." << std::endl;
                break;
            }
            else {
                std::cerr << "Error receiving file data from client." << std::endl;
                break;
            }
        }
        std::ofstream outFile(directoryPath + "\\" + fileName, std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(buffer.data(), buffer.size());
            outFile.close();
            std::cout << "Received file " << fileName << " from client." << std::endl;
        }
        else {
            std::cerr << "Unable to create file " << fileName << std::endl;
        }
    }
    void DEL(const std::string& fileName) {
        std::string filePath = directoryPath + "\\" + fileName;
        if (std::filesystem::remove(filePath)) {
            std::string response = "File " + fileName + " deleted successfully\n";
            send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
            std::cout << "Deleted file " << fileName << " from server." << std::endl;
            
        }
        else {
            std::cerr << "Unable to delete file " << fileName << " from server." << std::endl;
            
        }
    }
    void INFO(const std::string& fileName) {
        std::string filePath = directoryPath + "\\" + fileName;
        std::filesystem::path file(filePath);
        if (std::filesystem::exists(file)) {

            std::string info = "File size: " + std::to_string(std::filesystem::file_size(file)) + " bytes\n";
            send(clientSocket, info.c_str(), static_cast<int>(info.length()), 0);
            std::cout << "Sent file info for " << fileName << " to client." << std::endl;
        }
        else {
            std::cerr << "File " << fileName << " does not exist." << std::endl;
            
        }
    }


};

int main() {

    while (true) {
        int port = 12345;
        Server server(port);
        server.acceptClient();
        server.receiveCommand();
        if (server.getLoop() == false) {
            break;
        }
    }
    return 0;
}
