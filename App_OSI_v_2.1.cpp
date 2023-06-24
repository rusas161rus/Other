#include <iostream>     
#include <fstream>
#include <string>
#include <windows.h>    // для кдировки вывода в консоль
#include <Wscapi.h>
#include <sstream>      // для буфера
#include <stdio.h>
#include <VersionHelpers.h>
#include <array>
#include <vector>

#define CURL_STATICLIB // Указывает компилятору использовать статическую версию библиотеки libcurl.
#include <curl/curl.h>

#pragma comment(lib, "wininet.lib")
#pragma comment (lib,"Wscapi.lib")
#pragma comment (lib,"Normaliz.lib")
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Wldap32.lib")
#pragma comment (lib,"Crypt32.lib")
#pragma comment (lib,"Version.lib")

using namespace std;
#define BUFF_LEN 1024

string printStatus(WSC_SECURITY_PROVIDER_HEALTH status) {
    switch (status) {
    case WSC_SECURITY_PROVIDER_HEALTH_GOOD: return "INCLUDED";
    case WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED: return "NOTMONITORED";
    case WSC_SECURITY_PROVIDER_HEALTH_POOR: return "DISABLED ";
    case WSC_SECURITY_PROVIDER_HEALTH_SNOOZE: return "SNOOZE";
    default: return "Status Error";
    }
}

void getHealth() {
    WSC_SECURITY_PROVIDER_HEALTH health;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_FIREWALL, &health))
        cout << "FIREWALL:          " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS, &health))
        cout << "AUTOUPDATE:        " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &health))
        cout << "ANTIVIRUS:         " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTISPYWARE, &health))
        cout << "ANTISPYWARE:       " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_INTERNET_SETTINGS, &health))
        cout << "INTERNET SETTINGS: " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_USER_ACCOUNT_CONTROL, &health))
        cout << "UAC:               " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_SERVICE, &health))
        cout << "SERVICE:           " << printStatus(health) << endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ALL, &health))
        cout << "ALL:               " << printStatus(health) << endl;
    cout << "\n\n";
}

void mergeFiles(const string& file1Path, const string& file2Path, const string& file3Path, const string& outputPath) {
    ifstream file1(file1Path);
    ifstream file2(file2Path);
    ifstream file3(file3Path);
    ofstream combined(outputPath, ios::app); // Открываем файл на добавление

    if (file1 && file2 && file3 && combined) {
        string line;

        // Запись метки перед file1.txt
        combined << "\n\n=== Содержимое curl_4054.txt ===\n\n" << '\n';

        // Запись содержимого file1.txt
        while (std::getline(file1, line)) {
            combined << line << '\n';
        }

        // Запись метки перед file2.txt
        combined << "\n\n=== Содержимое curl_5060.txt ===\n\n" << '\n';

        // Запись содержимого file2.txt
        while (getline(file2, line)) {
            combined << line << '\n';
        }

        // Запись метки перед file3.txt
        combined << "\n\n === Содержимое log.txt === \n\n" << '\n';

        // Запись содержимого file3.txt
        while (getline(file3, line)) {
            combined << line << '\n';
        }

        cout << "Files merged successfully." << endl;
    }
    else {
        cout << "Error opening files." << endl;
    }

    // Закрытие файлов
    file1.close();
    file2.close();
    file3.close();
    combined.close();

    // Удаление файлов
    remove(file1Path.c_str());
    remove(file2Path.c_str());
    remove(file3Path.c_str());
}

string ExecuteCommand(const string& command) {
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "Failed to execute command: " + command;
    }

    const int bufferSize = 128;
    char buffer[bufferSize];
    string output;

    while (fgets(buffer, bufferSize, pipe)) {
        output += buffer;
    }

    _pclose(pipe);

    return output;
}

auto executeCommandAndGetOutput(const string& command) {
    array<char, 128> buffer;
    string result;

    FILE* pipe = _popen(command.c_str(), "r");
    if (pipe) {
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            cout << buffer.data();
            result += buffer.data();
        }
        _pclose(pipe);
    }
    else {
        cerr << "Error executing command: " << command << endl;
    }

    return result;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t totalSize = size * nmemb;
    data->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string GetFileVersion(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(fileSize);
        if (file.read(buffer.data(), fileSize)) {
            DWORD versionInfoSize = GetFileVersionInfoSizeA(filePath.c_str(), nullptr);
            if (versionInfoSize > 0) {
                std::vector<BYTE> versionInfoBuffer(versionInfoSize);
                if (GetFileVersionInfoA(filePath.c_str(), 0, versionInfoSize, versionInfoBuffer.data())) {
                    VS_FIXEDFILEINFO* fileInfo = nullptr;
                    UINT fileInfoSize = 0;
                    if (VerQueryValueA(versionInfoBuffer.data(), "\\", reinterpret_cast<LPVOID*>(&fileInfo), &fileInfoSize) && fileInfoSize >= sizeof(VS_FIXEDFILEINFO)) {
                        std::stringstream ss;
                        ss << HIWORD(fileInfo->dwFileVersionMS) << "." << LOWORD(fileInfo->dwFileVersionMS) << "." << HIWORD(fileInfo->dwFileVersionLS) << "." << LOWORD(fileInfo->dwFileVersionLS);
                        return ss.str();
                    }
                }
            }
        }
    }

    return "";
}

std::string DownloadFile(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);        

        CURLcode result = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (result == CURLE_OK) {
            return response;
        }
    }

    return "";
}

int main() {
    getHealth();    
    string verYandexBrowserOutput = ExecuteCommand("reg query \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\YandexBrowser\" /v DisplayVersion");
    cout << verYandexBrowserOutput << endl;
    std::string fileUrl = "https://browser.yandex.ru/download?banerid=0500000134&statpromo=true&partner_id=exp_inst_2";  // Замените на ссылку на скачиваемый файл
    std::string filePath = "ya.exe";  // Замените на путь, куда сохранить скачанный файл

    std::string downloadedFile = DownloadFile(fileUrl);
    if (!downloadedFile.empty()) {
        // Сохранение скачанного файла
        std::ofstream fileStream(filePath, std::ios::binary);
        fileStream << downloadedFile;
        fileStream.close();

        // Получение версии файла
        std::string fileVersion = GetFileVersion(filePath);
        if (!fileVersion.empty()) {
            std::cout << "Current browser version: " << fileVersion << std::endl;
        }
        else {
            std::cout << "Failed to get Current browser version." << std::endl;
        }
    }
    else {
        std::cout << "Failed to download file." << std::endl;
    }
    
    string verOutput = ExecuteCommand("ver");
    cout << verOutput << endl;
    string audioOutput = ExecuteCommand("driverquery | findstr /i audio");
    cout << audioOutput << endl;
    char szHost[BUFF_LEN] = { 0 };

    // Получаем дескриптор консоли
    GetStdHandle(STD_OUTPUT_HANDLE);
    // Проверяем версию операционной системы
    if (IsWindows10OrGreater()) {
        // Версия Windows 10 или выше
        SetConsoleOutputCP(1252);  // Кодировка для Windows 10
    }
    else if (IsWindows7OrGreater()) {
        // Версия Windows 7
        SetConsoleOutputCP(437);  // Кодировка для Windows 7
    }

    // Установка кодировки вывода консоли
    //SetConsoleOutputCP(1252);
    ofstream logFile("log.txt", ios::out | ios::binary);

    if (!logFile) {
        cerr << "Error opening log file" << endl;
        return 1;
    }

    cout << "Choose a server\n";
    cout << "1. Altair\n";
    cout << "2. Capella\n";
    cout << "3. Mimosa\n";
    cout << "4. Sirius\n";
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
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '2') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '3') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        cout << "Wait for the procedure to complete" << endl;
    }
    else if (szHost[0] == '4') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        cout << "Wait for the procedure to complete" << endl;
    }

    string szPing_Command = szPing_String + szHost;
    string szTracer_Command = szTracer_String + szIP_Address;
    string szCurl0_Command = "curl -k -v --max-time 5 --trace-ascii \"%USERPROFILE%\\Desktop\\curl_4054.txt\" " + szIP_Address + ":4054";
    string szCurl1_Command = "curl -k -v --max-time 5 --trace-ascii \"%USERPROFILE%\\Desktop\\curl_5060.txt\" " + szIP_Address + ":5060";
    string szNetstat_Command = "netstat -p TCP -n | findstr \":5060 :4054\"";
    string szIpconfig_Command = "cmd /c ipconfig";
    string szSysteminfo_String = "cmd /c systeminfo ";
    //string szTasklist_String =  "cmd /c Tasklist ";
    //string szChkdsk_String = "cmd /c chkdsk ";
    string szAudio_String = "cmd /c driverquery | findstr /i audio ";

    szPing_String.append(szHost);
    ostringstream packet_process_buffer;
    ostringstream ping_statistics_buffer;
    ostringstream ping_result_buffer;
    ostringstream curl_buffer;

    string pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szTracer_Command);
    cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szAudio_String);
    cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szNetstat_Command);
    cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szIpconfig_Command);
    cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szSysteminfo_String);
    cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szPing_Command);
    cout << pipeOutput;
    size_t statisticsPos = pipeOutput.find("Ping statistics for");
    if (statisticsPos != string::npos) {
        ping_statistics_buffer << pipeOutput.substr(statisticsPos);
        ping_result_buffer << pipeOutput.substr(0, statisticsPos);
    }

    //pipeOutput = executeCommandAndGetOutput(szChkdsk_String);
    //cout << pipeOutput;
    //packet_process_buffer << pipeOutput;

    //pipeOutput = executeCommandAndGetOutput(szTasklist_String);
    //cout << pipeOutput;
    //packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szCurl0_Command);
    cout << pipeOutput;
    curl_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szCurl1_Command);
    cout << pipeOutput;
    curl_buffer << pipeOutput;

    // Запись в файл всех буферов
    logFile << ping_statistics_buffer.str();
    logFile << packet_process_buffer.str();
    logFile << curl_buffer.str();
    logFile << ping_result_buffer.str();

    logFile.close();

    string file2Path = "curl_4054.txt";
    string file3Path = "curl_5060.txt";
    string file1Path = "log.txt";
    string outputPath = "log_end.txt";
    mergeFiles(file2Path, file3Path, file1Path, outputPath);

    cout << "\nResults written to log_end.txt" << endl;
    cout << "\ndone, this window can be closed" << endl;
   
    system("PAUSE");
    return 0;
}
