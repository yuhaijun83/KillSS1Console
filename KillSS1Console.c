/**
C:\Program Files (x86)\D.O.S\SS1Agent\SS1ACT.exe
C:\Program Files (x86)\D.O.S\SS1Agent\SS1UserAgent.exe
C:\Program Files (x86)\D.O.S\SS1Agent\SS1Loader.exe

"C:\Program Files (x86)\D.O.S\SS1Agent\SS1AutoUpdate.exe"
"C:\Program Files (x86)\D.O.S\SS1Agent\SS1CollectServer.exe"
"C:\Program Files (x86)\D.O.S\SS1Agent\SS1NetWatchServer.exe"
"C:\Program Files (x86)\D.O.S\SS1Agent\SS1RCD.exe" -port 21652 -service
"C:\Program Files (x86)\D.O.S\SS1Agent\SS1SnmpServer.exe"
"C:\Program Files (x86)\D.O.S\SS1Agent\RBP.exe" -d -f "C:\Program Files (x86)\D.O.S\SS1Agent\rbp.conf"
*/

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

#include <stdlib.h>
#include <io.h>  // for _access()
#include <errno.h>

#define F_OK 0  // Check for file existence

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE 4096
#define SS1_COLLECT_SERVER_PORT 21650

char path[100];
char command[50];
FILE *fp;

void killProcess(const char* processName) {
    sprintf(command, "TASKKILL /F /IM %s >nul 2>nul", processName);
    system(command);
}

DWORD WINAPI TaskKillThread(LPVOID lpParam) {

    char buffer[30];
    time_t rawtime;
    struct tm timeinfo;

    do {
        // 打印当前时间
        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        asctime_s(buffer, sizeof buffer, &timeinfo);
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", &timeinfo);
        printf("KillSS1Console %s Start \n", buffer);

        // 1. SS1CollectServer.exe
        killProcess("SS1CollectServer.exe");
        // 2. SS1UserAgent.exe
        killProcess("SS1UserAgent.exe");
        // 3. SS1ACT.exe
        killProcess("SS1ACT.exe");
        // 4. SS1ChSvr.exe
        killProcess("SS1ChSvr.exe");
        // 5. SS1Loader.exe
        killProcess("SS1Loader.exe");
        // 6. SS1GH.exe
        killProcess("SS1GH.exe");
        // 7. SS1GHx64.exe
        killProcess("SS1GHx64.exe");
        // 8. SS1GHLoader.exe
        killProcess("SS1GHLoader.exe");
        // 9. SS1GHLoaderx64.exe
        killProcess("SS1GHLoaderx64.exe");
        // 10. ss1Injectx64.exe
        killProcess("ss1Injectx64.exe");
        // 11. ss1Inject2x64.exe
        killProcess("ss1Inject2x64.exe");

        // 打印当前时间
        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        asctime_s(buffer, sizeof buffer, &timeinfo);
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", &timeinfo);
        printf("KillSS1Console %s Stop \n", buffer);

        Sleep(10000); // 10s

    } while (1);
    
    return 0;
}

DWORD WINAPI ServerThread(LPVOID lpParam) {
    WSADATA wsaData;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    char buffer[MAX_BUFFER_SIZE];
    int c, recv_size;
    int iBindRet = 1;

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed to initialize winsock.\n");
        return 1;
    }

    // 创建socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        printf("Failed to create socket.\n");
        return 1;
    }

    // 设置服务器地址和端口
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SS1_COLLECT_SERVER_PORT);

    // �?�?3s?��让另个线程杀掉开端口�?进程，以免端口被�?导致绑定失败
    Sleep(3000);

    do {
        // 绑�?
        if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
        {
            printf("Binding failed. It will automatically try to bind again after 5 minutes.\n");
            // 如果绑定失败?�?5�?钟后再绑定一次?��直到成功
            Sleep(300000);
        } else {
            printf("Binding successful.\n");
            iBindRet = 0;
        }
    } while (iBindRet == 1);

    // 监听
    listen(s, 3);

    // 等�?连接
    printf("Waiting for incoming connections on %d ...\n", SS1_COLLECT_SERVER_PORT);
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
    {
        puts("Connection accepted");

        // 接收并打印数据
        if ((recv_size = recv(new_socket, buffer, MAX_BUFFER_SIZE, 0)) == SOCKET_ERROR)
        {
            puts("recv failed");
        }

        buffer[recv_size] = '\0';
        printf("Received data: %s\n", buffer);
    }

    if (new_socket == INVALID_SOCKET)
    {
        printf("Accept failed.\n");
        return 1;
    }

    // 关闭socket
    closesocket(s);

    // �?�?Winsock
    WSACleanup();

    return 0;
}

int file_exists(const char *path) {
    return _access(path, F_OK) != -1;
}

int rename_file(const char *original_path, const char *new_path) {
    if (rename(original_path, new_path) != 0) {
        perror("Failed to rename the file");
        return -1;
    }
    printf("File successfully renamed to %s\n", new_path);
    return 0;
}

int backfile_SS1ACT() {
    const char *file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1ACT.exe";
    const char *new_file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1ACT.exe.bak";

    if (file_exists(file_path)) {
        printf("File %s exists.\n", file_path);
        if (rename_file(file_path, new_file_path) != 0) {
            return EXIT_FAILURE;
        }
    } 

    return EXIT_SUCCESS;
}

int backfile_SS1UserAgent() {
    const char *file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1UserAgent.exe";
    const char *new_file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1UserAgent.exe.bak";

    if (file_exists(file_path)) {
        printf("File %s exists.\n", file_path);
        if (rename_file(file_path, new_file_path) != 0) {
            return EXIT_FAILURE;
        }
    } 

    return EXIT_SUCCESS;
}

int backfile_SS1Loader() {
    const char *file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1Loader.exe";
    const char *new_file_path = "C:\\Program Files (x86)\\D.O.S\\SS1Agent\\SS1Loader.exe.bak";

    if (file_exists(file_path)) {
        printf("File %s exists.\n", file_path);
        if (rename_file(file_path, new_file_path) != 0) {
            return EXIT_FAILURE;
        }
    } 

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    CreateThread(NULL, 0, TaskKillThread, NULL, 0, NULL);
    CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);

    backfile_SS1ACT();
    backfile_SS1UserAgent();
    backfile_SS1Loader();

    // 主线程进�?��?限循环?��以防止程序立即退出
    while (1) {
        Sleep(60000);
    }

    return 0;
}

