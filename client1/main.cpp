#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include "main.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET sock;    // Declares a socket, so that the name can be used in the send and receive functions
string userInput;   // A string is declared, so that the name can be used in the sending function for holding message to be sent

// Function for sending data to server - use in thread for sending
void sending(){
    // The loop is infinite, so that the client will always be able to send data
    while(true) {
        cout << "> " << endl;       // Prompt the user to input data
        getline(cin, userInput);    // the userInput variable is filled with the input, getLine makes it so spaces are sent too

        if (userInput.size() > 0) // Make sure the user has typed in something
        {
            // Send the data
            int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
            if (sendResult != SOCKET_ERROR) {
                //cout << "Message was sent" << endl;
            } else if (sendResult == SOCKET_ERROR) {
                cout << "No socket found to send through" << endl;
            }
        }
    }
}

// Function for receiving and handling the information received from server/other client
void receiving(){
    // The loop is infinite, so that the client will always be able to receive data
    while(true) {
        char buf[4096];         // Char array for holding received data
        ZeroMemory(buf, 4096);  // Clears the char array from before, same as memset, to not have residual data
        int bytesReceived = recv(sock, buf, 4096, 0);   // Receive data and save in int to check if the client received stuff
        if (bytesReceived > 0) {
            // display response in console
            cout << string(buf, 0, bytesReceived) << endl;
        }
    }
}


// Main function.
int main()
{
    string ipAddress = "127.0.0.1";			// IP Address of the server
    int port = 27015;						// Listening port # on the server

    // Initialize WinSock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "Winsock could not be initialized, error # " << wsResult << endl;
        return 1;
    }

    // Create socket
    SOCKET sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 == INVALID_SOCKET)
    {
        cerr << "Socket could not be created, error # " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Fill hint struct
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to server
    int connResult = connect(sock2, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "Connection couldn't be established, error # " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }


    // Do-while loop to send and receive data
    sock = sock2;       // Set the used socket (sock) from  the start to be the initialized socket (sock2).
    do
    {
        thread sender(sending);   // thread for handling sending of data
        sender.detach();  // detach() thread to allow later code to be run simultaneously

        thread receiver(receiving);   // thread for handling received data
        receiver.join();  // join() to make the thread run, no later code, so no need for detach()

    } while (userInput.size() > 0);

    //  Close down the client by shutting down the client and cleaning up
    closesocket(sock);
    WSACleanup();
    return 0;
}