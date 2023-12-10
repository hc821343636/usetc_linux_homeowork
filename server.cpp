#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <fstream>
#include <sstream>
void receiveMessages(int clientSocket) {
    while (true) {
        // 接收客户端消息
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data from client." << std::endl;
            break;
        } else if (bytesRead == 0) {
            // 客户端关闭连接
            std::cout << "Client closed the connection." << std::endl;
            break;
        } else {
            buffer[bytesRead] = '\0';
            std::cout << "Received from client: " << buffer << std::endl;
        }
    }
}
void serializeFile(const std::string& sourcePath, const std::string& serializedPath) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    if (!sourceFile) {
        std::cerr << "Error opening source file: " << sourcePath << std::endl;
        return;
    }

    std::ostringstream oss;
    oss << sourceFile.rdbuf();

    std::ofstream serializedFile(serializedPath, std::ios::binary);
    if (!serializedFile) {
        std::cerr << "Error opening serialized file: " << serializedPath << std::endl;
        return;
    }

    serializedFile << oss.str();
    std::cout << "File serialized successfully." << std::endl;
}

void deserializeFile(const std::string& serializedPath, const std::string& destinationPath) {
    std::ifstream serializedFile(serializedPath, std::ios::binary);
    if (!serializedFile) {
        std::cerr << "Error opening serialized file: " << serializedPath << std::endl;
        return;
    }

    std::ofstream destinationFile(destinationPath, std::ios::binary);
    if (!destinationFile) {
        std::cerr << "Error opening destination file: " << destinationPath << std::endl;
        return;
    }

    destinationFile << serializedFile.rdbuf();
    std::cout << "File deserialized successfully." << std::endl;
}
int main(int argc, char **argv) {

	if(argc != 2)
	{
		std::cout << "Parameters error" << std::endl;
		return 0;
	}

	
    // 创建套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    // 准备地址结构体
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    std::string host;
	if(strcmp(argv[1], "2") == 0){
	serverAddr.sin_port = htons(54321);
	host="54321";
	}else{
	serverAddr.sin_port = htons(12345); // 使用端口号 12345
	host="12345";
	}
    // 绑定地址
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket." << std::endl;
        close(serverSocket);
        return -1;
    }

    // 监听
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port "<<host<<"..." << std::endl;

    // 接受连接
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection." << std::endl;
        close(serverSocket);
        return -1;
    }

    // 发送欢迎消息给客户端
    const char* welcomeMessage = "Welcome! Start chatting...";
    send(clientSocket, welcomeMessage, strlen(welcomeMessage), 0);
    std::cout << "Message sent to client: " << welcomeMessage << std::endl;

    // 启动接收消息的线程
    std::thread receiveThread(receiveMessages, clientSocket);
    
    // 进入消息发送循环
    while (true) {
        // 从用户输入获取消息
        std::cout << "Enter your message: ";
        std::string replyMessage;
        std::getline(std::cin, replyMessage);
	 if (replyMessage == "task") {
	 	std::cout << "Enter your taskFilePath: ";
	 	std::string taskFilePath;
	 	std::getline(std::cin, taskFilePath);
	 	std::cout << "Enter client number to test "<<taskFilePath<<" task ";
	 	std::string clientNumber;
	 	std::getline(std::cin, clientNumber);
	 	std::string serializedFilePath = "tmp.bin";
	 	serializeFile(taskFilePath, serializedFilePath);
	 	std::string destinationFilePath="./plugin"+clientNumber+"/"+taskFilePath;
	 	deserializeFile(serializedFilePath, destinationFilePath);
	 	replyMessage="server gives you a task to test named"+taskFilePath+"located in plugin"+clientNumber;	 	
	 }
        // 发送消息给客户端
       
        send(clientSocket, replyMessage.c_str(), replyMessage.size(), 0);
        
    }

    // 关闭连接
    close(clientSocket);
    close(serverSocket);

    return 0;
}

