#include <iostream> // for std::cout
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

#define BUFF_LEN 1024

int main()
{
    char szHost[BUFF_LEN] = { 0 };
    // Установка кодировки вывода консоли
    SetConsoleOutputCP(1252);
    ofstream logFile("log.txt", ios::out | ios::binary);
    

    if (!logFile) {
        cerr << "Error opening log file" << endl;
        return 1;
    }   
    
    cout << "Choose a server\n";
    cout << "1. xxx.xxx.xx.x.\n";
    cout << "2. xxx.xxx.xx.x.\n";
    cout << "3. xxx.xxx.xx.x.\n";
    cout << "4. xxx.xxx.xx.x.\n";
    cout << "Empter a ping Host or IP Addr:\t";
    cin.getline(szHost, BUFF_LEN);
    cout << endl;

    string szPing_String = "cmd /c ping -n 50 ";
    string szTracer_String = "cmd /c tracert ";
    string szIP_Address;

    if (szHost[0] == '\0') {
        strcpy_s(szHost, "ya.ru");
    }
    else if (szHost[0] == '1') {
        strcpy_s(szHost, "xxx.xxx.xx.x.");
        szIP_Address = "xxx.xxx.xx.x.";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '2') {
        strcpy_s(szHost, "xxx.xxx.xx.x.");
        szIP_Address = "xxx.xxx.xx.x.";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '3') {
        strcpy_s(szHost, "xxx.xxx.xx.x.");
        szIP_Address = "xxx.xxx.xx.x.";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '4') {
        strcpy_s(szHost, "xxx.xxx.xx.x.");
        szIP_Address = "xxx.xxx.xx.x.";
        cout << "Wait for the procedure to complete" << endl;
    }
  
    string szPing_Command = szPing_String + szHost;
    string szTracer_Command = szTracer_String + szIP_Address;
    string szCurl0_Command = "curl -v --max-time 5 " + szIP_Address + ":4054";
    string szCurl1_Command = "curl -v --max-time 5 " + szIP_Address + ":5060";
    string szNetstat_Command = "netstat -p TCP -n | findstr \":5060 :4054\"";
    string szIpconfig_Command = "ipconfig";

    string szIpconfig_String = "cmd /c ipconfig ";

    szPing_String.append(szHost);


    FILE* pipe = _popen(szPing_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing ping command" << endl;
    }

    pipe = _popen(szTracer_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing traceroute command" << endl;
    }

    pipe = _popen(szNetstat_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing netstat command" << endl;
    }

    pipe = _popen(szCurl0_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing curl command" << endl;
    }

    pipe = _popen(szCurl1_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing curl command" << endl;
    }

    pipe = _popen(szIpconfig_Command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            logFile << buffer;
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing ipconfig command" << endl;
    }

    // Закрытие файла лог
    logFile.close();   

    cout << "Results written to log.txt" << endl;        
    cout << "done, this window can be closed" << endl;


    system("PAUSE");
    return 0;
}
