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
#include <Urlmon.h>

//#define CURL_STATICLIB // Указывает компилятору использовать статическую версию библиотеки libcurl.
//#include <curl/curl.h>

#pragma comment(lib, "urlmon.lib")

#pragma comment (lib, "Wscapi.lib")
//#pragma comment (lib, "Normaliz.lib")
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Wldap32.lib")
//#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "Version.lib")
#pragma comment (lib, "wininet.lib")


#define BUFF_LEN 1024

std::string printStatus(WSC_SECURITY_PROVIDER_HEALTH status) {
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
        std::cout << "FIREWALL:          " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS, &health))
        std::cout << "AUTOUPDATE:        " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &health))
        std::cout << "ANTIVIRUS:         " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTISPYWARE, &health))
        std::cout << "ANTISPYWARE:       " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_INTERNET_SETTINGS, &health))
        std::cout << "INTERNET SETTINGS: " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_USER_ACCOUNT_CONTROL, &health))
        std::cout << "UAC:               " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_SERVICE, &health))
        std::cout << "SERVICE:           " << printStatus(health) << std::endl;
    if (S_OK == WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ALL, &health))
        std::cout << "ALL:               " << printStatus(health) << std::endl;
    std::cout << "\n\n";
}

void mergeFiles(const std::string& file1Path, const std::string& file2Path, const std::string& file3Path, const std::string& outputPath) {
    std::ifstream file1(file1Path);
    std::ifstream file2(file2Path);
    std::ifstream file3(file3Path);
    std::ofstream combined(outputPath, std::ios::app); // Открываем файл на добавление

    if (file1 && file2 && file3 && combined) {
        std::string line;

        // Запись метки перед file1.txt
        combined << "\n\n=== Содержимое curl_4054.txt ===\n\n" << '\n';

        // Запись содержимого file1.txt
        while (std::getline(file1, line)) {
            combined << line << '\n';
        }

        // Запись метки перед file2.txt
        combined << "\n\n=== Содержимое curl_5060.txt ===\n\n" << '\n';

        // Запись содержимого file2.txt
        while (std::getline(file2, line)) {
            combined << line << '\n';
        }

        // Запись метки перед file3.txt
        combined << "\n\n === Содержимое log.txt === \n\n" << '\n';

        // Запись содержимого file3.txt
        while (std::getline(file3, line)) {
            combined << line << '\n';
        }

        std::cout << "Files merged successfully." << std::endl;
    }
    else {
        std::cout << "Error opening files." << std::endl;
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

std::string ExecuteCommand(const std::string& command) {
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "Failed to execute command: " + command;
    }

    const int bufferSize = 128;
    char buffer[bufferSize];
    std::string output;

    while (fgets(buffer, bufferSize, pipe)) {
        output += buffer;
    }

    _pclose(pipe);

    return output;
}

auto executeCommandAndGetOutput(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;

    FILE* pipe = _popen(command.c_str(), "r");
    if (pipe) {
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            std::cout << buffer.data();
            result += buffer.data();
        }
        _pclose(pipe);
    }
    else {
        std::cerr << "Error executing command: " << command << std::endl;
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

/*std::string DownloadFile(const std::string& url) {
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
}*/

int main() {    
    getHealth();
    std::string verYandexBrowserOutput = ExecuteCommand("reg query \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\YandexBrowser\" /v DisplayVersion");
    std::cout << verYandexBrowserOutput << std::endl;
    HRESULT hr = URLDownloadToFile(NULL, "https://browser.yandex.ru/download?banerid=0500000134&statpromo=true&partner_id=exp_inst_2", "ya.exe", BINDF_GETNEWESTVERSION, NULL);
    if (FAILED(hr))
    {
        std::cerr << "Failed to download file: " << hr << std::endl;
        return 1;
    }
    std::cout << "File downloaded successfully" << std::endl;
    std::string filePath = "ya.exe";
    std::string fileVersion = GetFileVersion(filePath);
    if (!fileVersion.empty()) {
        std::cout << "Current browser version: " << fileVersion << std::endl;
    }
    else {
        std::cout << "Failed to get Current browser version." << std::endl;
    }   
    
    //std::string fileUrl = "https://browser.yandex.ru/download?banerid=0500000134&statpromo=true&partner_id=exp_inst_2";  // Замените на ссылку на скачиваемый файл
    //std::string filePath = "ya.exe";  // Замените на путь, куда сохранить скачанный файл

    /*std::string downloadedFile = DownloadFile(fileUrl);
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

        // Спрашиваем пользователя, удалить или оставить файл
        /*std::cout << "Do you want to delete the downloaded file? (Y/N): ";
        std::string userInput;
        std::cin >> userInput;
        if (userInput == "Y" || userInput == "y") {
            // Удаляем файл
            if (std::remove(filePath.c_str()) == 0) {
                std::cout << "File deleted." << std::endl;
            }
            else {
                std::cout << "Failed to delete the file." << std::endl;
            }
        }
        else {
            std::cout << "File kept." << std::endl;
        }
    }
    else {
        std::cout << "Failed to download file." << std::endl;
    }*/

    std::string verOutput = ExecuteCommand("ver");
    std::cout << verOutput << std::endl;
    std::string audioOutput = ExecuteCommand("driverquery | findstr /i audio");
    std::cout << audioOutput << std::endl;
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
    std::ofstream logFile("log.txt", std::ios::out | std::ios::binary);

    if (!logFile) {
        std::cerr << "Error opening log file" << std::endl;
        return 1;
    }

    std::cout << "Choose a server\n";
    std::cout << "1. Altair\n";
    std::cout << "2. Capella\n";
    std::cout << "3. Mimosa\n";
    std::cout << "4. Sirius\n";
    std::cout << "Empter a ping Host or IP Addr:\t";      
    std::cin.getline(szHost, BUFF_LEN);
    std::cout << std::endl;
    std::string szPing_String = "cmd /c ping -n 50 ";
    std::string szTracer_String = "cmd /c tracert ";
    std::string szIP_Address;

    if (szHost[0] == '\0') {
        strcpy_s(szHost, "ya.ru");
    }
    else if (szHost[0] == '1') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        std::cout << "Wait for the procedure to complete" << std::endl;
    }
    else if (szHost[0] == '2') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        std::cout << "Wait for the procedure to complete" << std::endl;
    }
    else if (szHost[0] == '3') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        std::cout << "Wait for the procedure to complete" << std::endl;
    }
    else if (szHost[0] == '4') {
        strcpy_s(szHost, "xxxxx");
        szIP_Address = "xxxxx";
        std::cout << "Wait for the procedure to complete" << std::endl;
    }

    std::string szPing_Command = szPing_String + szHost;
    std::string szTracer_Command = szTracer_String + szIP_Address;
    std::string szCurl0_Command = "curl -k -v --max-time 5 --trace-ascii \"%USERPROFILE%\\Desktop\\curl_4054.txt\" " + szIP_Address + ":4054";
    std::string szCurl1_Command = "curl -k -v --max-time 5 --trace-ascii \"%USERPROFILE%\\Desktop\\curl_5060.txt\" " + szIP_Address + ":5060";
    std::string szNetstat_Command = "netstat -p TCP -n | findstr \":5060 :4054\"";
    std::string szIpconfig_Command = "cmd /c ipconfig";
    std::string szSysteminfo_String = "cmd /c systeminfo ";
    //string szTasklist_String =  "cmd /c Tasklist ";
    //string szChkdsk_String = "cmd /c chkdsk ";
    std::string szAudio_String = "cmd /c driverquery | findstr /i audio ";

    szPing_String.append(szHost);
    std::ostringstream packet_process_buffer;
    std::ostringstream ping_statistics_buffer;
    std::ostringstream ping_result_buffer;
    std::ostringstream curl_buffer;

    std::string pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szTracer_Command);
    std::cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szAudio_String);
    std::cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szNetstat_Command);
    std::cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szIpconfig_Command);
    std::cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szSysteminfo_String);
    std::cout << pipeOutput;
    packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szPing_Command);
    std::cout << pipeOutput;
    size_t statisticsPos = pipeOutput.find("Ping statistics for");
    if (statisticsPos != std::string::npos) {
        ping_statistics_buffer << pipeOutput.substr(statisticsPos);
        ping_result_buffer << pipeOutput.substr(0, statisticsPos);
    }

    //pipeOutput = executeCommandAndGetOutput(szChkdsk_String);
    //std::cout << pipeOutput;
    //packet_process_buffer << pipeOutput;

    //pipeOutput = executeCommandAndGetOutput(szTasklist_String);
    //std::cout << pipeOutput;
    //packet_process_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szCurl0_Command);
    std::cout << pipeOutput;
    curl_buffer << pipeOutput;

    pipeOutput = executeCommandAndGetOutput(szCurl1_Command);
    std::cout << pipeOutput;
    curl_buffer << pipeOutput;

    // Запись в файл всех буферов
    logFile << ping_statistics_buffer.str();
    logFile << packet_process_buffer.str();
    logFile << curl_buffer.str();
    logFile << ping_result_buffer.str();

    logFile.close();

    std::string file2Path = "curl_4054.txt";
    std::string file3Path = "curl_5060.txt";
    std::string file1Path = "log.txt";
    std::string outputPath = "log_end.txt";
    mergeFiles(file2Path, file3Path, file1Path, outputPath);

    std::cout << "\nResults written to log_end.txt" << std::endl;
    std::cout << "\ndone, this window can be closed" << std::endl;
   
    system("PAUSE");
    return 0;
}
