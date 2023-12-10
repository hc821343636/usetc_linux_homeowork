#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include "CPluginController.h"
#include <string.h>
#include <stdlib.h>

void receiveMessages(int clientSocket) {
    while (true) {
        // 接收服务器消息
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data from server." << std::endl;
            break;
        } else if (bytesRead == 0) {
            // 服务器关闭连接
            std::cout << "Server closed the connection." << std::endl;
            break;
        } else {
            buffer[bytesRead] = '\0';
            std::cout << "Received from server: " << buffer << std::endl;
        }
    }
}


void doJob(int number){

}

int main() {
    // 创建套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    // 准备地址结构体
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的 IP 地址
    serverAddr.sin_port = htons(54321); // 使用端口号 54321

    // 连接到服务器
    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server." << std::endl;
        close(clientSocket);
        return -1;
    }

    // 接收服务器欢迎消息
    char welcomeBuffer[1024];
    ssize_t welcomeBytesRead = recv(clientSocket, welcomeBuffer, sizeof(welcomeBuffer), 0);
    if (welcomeBytesRead == -1) {
        std::cerr << "Error receiving welcome message from server." << std::endl;
    } else {
        welcomeBuffer[welcomeBytesRead] = '\0';
        std::cout << "Received from server: " << welcomeBuffer << std::endl;
    }

    // 启动接收消息的线程
     std::thread receiveThread(receiveMessages, clientSocket);

    // 进入消息发送循环
    while (true) {
        // 从用户输入获取消息
        std::cout << "Enter your message: ";
        std::string clientMessage;
        std::getline(std::cin, clientMessage);
        if(clientMessage=="test"){
         std::cout << "Enter task number: ";
         std::string task_number;
         std::getline(std::cin, task_number);
        int FunctionID = std::stoi(task_number);
        CPluginController pc;

	pc.InitializeController();

	pc.ProcessRequest(FunctionID);

	pc.UninitializeController();
	
        clientMessage="task "+task_number+"test successfully";
        }

        // 发送消息给服务器
        send(clientSocket, clientMessage.c_str(), clientMessage.size(), 0);
    }

    // 关闭连接
    close(clientSocket);

    return 0;
}

