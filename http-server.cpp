#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

constexpr int PORT = 8080;
constexpr size_t ALLOCATION_SIZE = 10 * 1024 * 1024;
int count = 0;

void handleRequest(int clientSocket) {
    std::cout << "Handling request " << ++count << std::endl;
    char* leak = (char*)malloc(ALLOCATION_SIZE);
    if (leak) {
        // Use the memory to store data (write something into it)
        memset(leak, 'A', ALLOCATION_SIZE - 1); // Fill with 'A's
        leak[ALLOCATION_SIZE - 1] = '\0'; // Null-terminate for safety
    } else {
        std::cerr << "Memory allocation failed!" << std::endl;
        close(clientSocket);
        return;
    }

    // Prepare the HTTP response
    const char* response = "HTTP/1.1 200 OK\n"
                           "Content-Type: text/plain\n"
                           "Content-Length: 30\n"
                           "\n"
                           "Memory Leak Example: Success!\n";

    // Send the HTTP response
    send(clientSocket, response, strlen(response), 0);

    // Close the socket for the client
    close(clientSocket);
}

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // Bind socket to the specified port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Binding failed!" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listening failed!" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is running on port " << PORT << std::endl;

    while (true) {
        // Accept incoming connections
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Accept failed!" << std::endl;
            continue;
        }

        // Handle the request (and leak memory)
        handleRequest(clientSocket);
    }

    // Clean up (won't be reached in this example)
    close(serverSocket);
    return 0;
}
