//Bukola Obajemu
//A server program that will wait for connection
// requests from your client and exchange one-line
//text messages with it.


#include <iostream>
#include<sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
using namespace std;

//stores weather information from file
vector <string>  weather;

//finds the weather info from the given city
string displayWeatherInfo(string city)
{
    //find the city + info
    string f ="empty";
    vector <string> result;
    for(int i=0;i<weather.size();i++)
    {
        size_t found = weather[i].find(city);
        if(found!=string::npos)
        {
            //splits the city,temp,condition & puts info in a result vector
            stringstream ss(weather[i]);
            while(ss.good())
            {
                string substr;
                getline(ss,substr,',');
                result.push_back(substr);
                f="found";
            }
            break;
        }
    }
    //if no info found
    if(f=="empty")
    {
        return "No data\n";
    }
    //if info found
    else
    {
        return "Tomorrow's maximum temperature is " + result[1] +" F"+"\nTomorrow's sky condition is "+result[2] +"\n";

    }
}


//checks to see if portNumber is an int
bool isNumber(string s)
{
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == 0)
            return false;

    return true;
}

int main() {

    string weat;
    string portNumber;
    //opens file and puts each line in a weather vector & if it doesn't open cerr an error msg
    ifstream file("weather20.txt");
    if(file.is_open())
    {
        while(getline(file, weat))
        {
            weather.push_back(weat);
        }

    }
    else
    {
        cerr<<"file doesn't open";
    }
    file.close();

    //ask for user to input port number
    int port;
    cout<<"Enter server port number: ";
    getline(cin,portNumber);
    if(isNumber(portNumber))
    {
        port = stoi(portNumber);
    }
    else
    {
        cerr<<"port number has to be an int";
        return -1;
    }


    //create a socket
    int listening = socket(AF_INET,SOCK_STREAM,0);
    if(listening==-1)
    {
        cerr<<"Can't create a socket";
        return -1;
    }

    //bind the socket to IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port); // htons == host to network short
    inet_pton(AF_INET,"0.0.0.0", &hint.sin_addr); //binding information

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    //mark the socket for listening in
    if(listen(listening,SOMAXCONN)==-1) //SOMAXCONN == max # of connections
    {
        cerr <<"Can't listen";
        return -3;
    }

    //accept a call
    sockaddr_in client;
    socklen_t  clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    //accept incoming connections
    int clientSocket = accept(listening, (sockaddr*)&client,&clientSize);

    if(clientSocket == -1)
    {
        cerr<<"Prob w/client connecting";
        return -4;
    }

    memset(host,0, NI_MAXHOST); //cleans up junk that's
    memset(svc,0,NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST,svc,NI_MAXSERV,0);

    if(result)
    {
        //cout<< host << "connected on " << svc  << endl;
    }
    else
    {
        inet_ntop(AF_INET,&client.sin_addr, host, NI_MAXHOST);//
        //cout<< host << "connected on" << ntohs(client.sin_port) << endl;
    }


    char buff[4096];
    while (true)
    {
        //clear buffer
        memset(buff,0,4096);
        //recv msg from client & checks for errors
        int bytesCityRecv = recv(clientSocket, buff, 4096, 0);
        if(bytesCityRecv ==-1)
        {
            cerr<<"Error in recv";
            break;
        }
        if(bytesCityRecv==0)
        {
            cerr<<"Clitent dis";
        }

        //prints out city name & weather info
        cout<<"Weather report for "<<string(buff,0,bytesCityRecv)<< endl;
        string city = string(buff,0,bytesCityRecv);
        cout<<displayWeatherInfo(city);
        //send weather info to client
        send(clientSocket, displayWeatherInfo(city).c_str(),displayWeatherInfo(city).size() +1,0);
        //clear buff
        memset(buff,0,4096);

        //keeps server open
        clientSocket=accept(listening, (sockaddr*)&client,&clientSize);

    }

    //close socket
    close (clientSocket);

    return 0;
}
