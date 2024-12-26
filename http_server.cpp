#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>

constexpr int PORT = 8080;
constexpr int MAX_CONNECTIONS = 5;
constexpr int BUFFER_SIZE = 1024;

bool endsWith(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void sendResponse(int clientSocket, const std::string &response) {
    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    recv(clientSocket, buffer, BUFFER_SIZE, 0);

    std::stringstream ss(buffer);
    std::string request, path, httpVersion;
    ss >> request >> path >> httpVersion;

    if (httpVersion != "HTTP/1.1") {
        std::string response = "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n";
        sendResponse(clientSocket, response);
        return;
    }

    std::string line;
    while (std::getline(ss, line) && line != "\r") {
        if (line.empty() || (line.back() != '\r' || line[line.size() - 2] != '\n')) {
            std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            sendResponse(clientSocket, response);
            return;
        }
    }

    std::string hostHeader;
    bool hostFound = false;
    while (ss >> hostHeader) {
        if (hostHeader == "Host:") {
            hostFound = true;
            break;
        }
    }

    if (!hostFound) {
        std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        sendResponse(clientSocket, response);
        return;
    }

    if (request == "GET" || request == "HEAD") {
        std::string basePath = "local_dir";
        std::string requestedPath = path;

        if (basePath.back() != '/' && requestedPath.front() != '/')
            basePath += '/';

        std::string filePath = basePath + requestedPath;

        //std::cout << filePath << std::endl;

        std::ifstream file(filePath, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
            sendResponse(clientSocket, response);
            return;
        } else {
            std::streamsize fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::ostringstream fileContent;
            fileContent << file.rdbuf();

            std::string contentType = "text/plain";
            if (endsWith(filePath, ".html") || endsWith(filePath, ".htm")) {
                contentType = "text/html";
            } else if (endsWith(filePath, ".gif")) {
                contentType = "image/gif";
            } else if (endsWith(filePath, ".jpg") || endsWith(filePath, ".jpeg")) {
                contentType = "image/jpeg";
            } else if (endsWith(filePath, ".pdf")) {
                contentType = "application/pdf";
            }

            std::string responseHeader = "HTTP/1.1 200 OK\r\n";
            responseHeader += "Server: YourServerName\r\n";
            responseHeader += "Content-Length: " + std::to_string(fileSize) + "\r\n";
            responseHeader += "Content-Type: " + contentType + "\r\n\r\n";

            if(request == "HEAD"){
                send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);

            }
            
            if (request == "GET") {

                responseHeader += fileContent.str();
                send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
            }
        }
    } else {
        std::string response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
        sendResponse(clientSocket, response);
        return;
    }

    close(clientSocket);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n";
        return -1;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Listen failed\n";
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::thread(handleClient, clientSocket).detach();
    }

    close(serverSocket);
    return 0;
}
