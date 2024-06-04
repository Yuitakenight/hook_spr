#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "detours.h"

#pragma comment(lib, "detours.lib")

VOID __declspec(dllexport)myfix()
{

}

BOOL InjectDll(LPCSTR lpApplicationName, LPCSTR lpDllNames[], int numDlls)
{
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL bSuccess = DetourCreateProcessWithDllsA(lpApplicationName, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi, numDlls, (LPCSTR*)lpDllNames, NULL);

    if (bSuccess)
    {
        printf("Process created successfully with DLL injection.\n");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        printf("Failed to create process with DLL injection. Error code: %d\n", GetLastError());
    }

    return bSuccess;
}

int main()
{
    std::string exeName;
    std::ifstream configFile("myconfig.ini");

    if (configFile.is_open())
    {
        std::string line;
        while (std::getline(configFile, line))
        {
            if (line.find("name=") != std::string::npos)
            {
                exeName = line.substr(5);
                break;
            }
        }
        configFile.close();
    }

    if (!exeName.empty())
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);

        std::string exePath = std::string(buffer) + "\\" + exeName;

        LPCSTR lpDllNames[] = {
            "Dll1.dll"
        };

        if (InjectDll(exePath.c_str(), lpDllNames, 1))
        {
            printf("DLL injected into %s.\n", exeName.c_str());
        }
        else
        {
            printf("Error injecting DLL into %s.\n", exeName.c_str());
        }
    }
    else
    {
        printf("Error: exe name not found in config.ini.\n");
    }

    return 0;
}
