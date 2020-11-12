//Bukola Obajemu
//A client program that will connect with your
//server and send it requests for the weather report
//for a given city.


#include <iostream>
#include<sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;
//checks to see if portNumber is an int
bool isNumber(string s)
{
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == 0)
            return false;

    return true;
}

int main() {

    string portNumber;
    string hostName;

    //ask the user for the server host name
    cout<<"Enter the server host name: ";
    getline(cin,hostName);
    //exits out of code if the name is incorrect
    while(hostName!="localhost")
    {
        cerr<<"Incorrect server host";
        return -1;
    }

    //ask the user for the port number & converts to int
    int ports;
    cout <<"Enter the server port number: ";
    getline(cin,portNumber);

    if(isNumber(portNumber))
    {
        ports = stoi(portNumber);
    }
    else
    {
        cerr<<"port number has to be an int";
        return -1;
    }


    //finds the host by name
    struct hostent *p = gethostbyname(hostName.c_str());
    string ip_address =inet_ntoa( *((struct in_addr *)p->h_addr) );

    //creates a socket & if it doesn't exits
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1) return 1;

    //creat a hint structure for the server
    sockaddr_in hint; //internet address
    hint.sin_family = AF_INET;
    hint.sin_port = htons(ports);
    inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);

    //connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if(connectRes == -1) return 1;



    char buf[4096];
    string cityName;

    //ask for city name
    cout<<"Enter a city name: ";
    getline(cin,cityName);
    //sends the info to server & cleans up junk
    send(sock, cityName.c_str(),cityName.size()+1,0);
    memset(buf,0,4096);
    //recieves info about the city from server and prints it out
    int cityBytesRecv = recv(sock,buf,4096,0);
    cout<<string(buf,cityBytesRecv);

    //closes socket
    close(sock);
    return 0;
}
