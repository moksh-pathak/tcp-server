#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

int main(){

    // Create a Socket
    int listening = socket(AF_INET, SOCK_STREAM, 0); //Creating a socket
    if(listening == -1){
        cerr<<"Can't create a socket!";
        return -1;
    }

    // Bind the socket to a IP/Port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){  //Bind listening socket with AF_INET format to this hint structure
        cerr<<"Can't bind to IP/Port";
        return -2;
    }
    // Mark the socket for listening in
    if(listen(listening, SOMAXCONN)==-1){ // If the listening socket isn't listening (no. of connections is max(SOMAXCONN), usually 128, but depends on OS), give error 
        cerr<<"Can't listen!";
        return -3;
    }
    // Accept a Call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV]; //svc = service

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if(clientSocket == -1){
        cerr << "Problem with client connecting!";
        return -4;
    }


    // Close the listening port
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, 
                            sizeof(client), 
                            host,
                            NI_MAXHOST, 
                            svc, 
                            NI_MAXSERV,
                            0);
    
    if(result){
        cout<< host << " connected on " << svc <<endl;
    }

    else{
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout<< host << " connect on " << ntohs(client.sin_port) << endl;
    }
    // While receiving - display message, echo message
    char buf[4096];
    while(true){
        // Clear the buffer
        memset(buf, 0, 4096);
        // Wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if(bytesRecv == -1){
            cerr << "There was a connection issue" << endl;
            break;
        }

        if(bytesRecv == 0){
            cout << "Client disconnected" << endl;
            break;
        }

        // Display message
        cout << "Received : " << string(buf, 0, bytesRecv) << endl;

        // Resend message
        send(clientSocket, buf, bytesRecv + 1, 0);
    }

    // Close socket
    close(clientSocket);

    return 0;
}