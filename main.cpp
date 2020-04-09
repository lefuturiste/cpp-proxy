#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main() {
    std::cout << "Starting TCP Server..." << std::endl;
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cerr << "ERR: Can't open socket! \n";
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(9001);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "ERR: Can't bind to IP/port \n";
        return -2;
    }

    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "ERR: Can't bind to IP/port MAXCONN \n";
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    std::cout << "Accept client... \n";
    int clientSocket = accept(
            listening,
            (sockaddr*)&client,
            &clientSize
    );

    if (clientSocket == -1) {
        std::cerr << "ERR: Client can't connect \n";
        return -4;
    }

    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo(
            (sockaddr*)&client,
            sizeof(client),
            host,
            NI_MAXHOST,
            svc,
            NI_MAXSERV,
            0
    );

    if(result) {
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    char buf[4096];
    while (true) {
        // clear buffer
        memset(buf, 0, 4096);
        // wait for a msg
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1) {
            std::cout << "There was a connexion issue" << std::endl;
            break;
        }
        if (bytesRecv == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::cout << "Bytes received: " << std::string(buf, 0, bytesRecv) << std::endl;
        // display msg
        // resend message
        send(clientSocket, buf, bytesRecv + 1, 0);

        close(clientSocket);
    }

    std::cout << "End of server" << std::endl;

    return 0;
}
