#include <windows.h>
#include "detours.h"
#include <iostream>

#pragma comment(lib, "detours.lib") 

VOID __declspec(dllexport)CN()
{

}

static decltype(&CreateFontA) TrueCreateFontA = CreateFontA;
static decltype(&CreateFontIndirectA) TrueCreateFontIndirectA = CreateFontIndirectA;
static decltype(&CreateFileA) TrueCreateFileA = CreateFileA;


HFONT WINAPI MyCreateFontA(
    int cHeight,
    int cWidth,
    int cEscapement,
    int cOrientation,
    int cWeight,
    DWORD bItalic,
    DWORD bUnderline,
    DWORD bStrikeOut,
    DWORD iCharSet,
    DWORD iOutPrecision,
    DWORD iClipPrecision,
    DWORD iQuality,
    DWORD iPitchAndFamily,
    LPCSTR pszFaceName)
{
    iCharSet = 0x86; 
    pszFaceName = "Heiti"; 
    return TrueCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}


HFONT WINAPI MyCreateFontIndirectA(LOGFONTA* lplf)
{
    lplf->lfCharSet = 0x86; // 
    strcpy_s(lplf->lfFaceName, "Heiti"); 
    return TrueCreateFontIndirectA(lplf);
}


HANDLE WINAPI MyCreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    std::string modifiedFileName(lpFileName);

   
    if (modifiedFileName.find("ame.csx") != std::string::npos)
    {
        size_t pos = modifiedFileName.rfind("ame.csx");
        if (pos != std::string::npos) {
            modifiedFileName.replace(pos, std::string("ame.csx").length(), "amecn.csx");
        }
    }

   
    if (modifiedFileName.find("opening.mei") != std::string::npos)
    {
        size_t pos = modifiedFileName.rfind("opening.mei");
        if (pos != std::string::npos) {
            modifiedFileName.replace(pos, std::string("opening.mei").length(), "opening.avi");
        }
    }

 
    lpFileName = modifiedFileName.c_str();

    return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


bool InstallHooks()
{
    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // 对CreateFontA、CreateFontIndirectA、和CreateFileA应用hooks
    DetourAttach(&(PVOID&)TrueCreateFontA, MyCreateFontA);
    DetourAttach(&(PVOID&)TrueCreateFontIndirectA, MyCreateFontIndirectA);
    DetourAttach(&(PVOID&)TrueCreateFileA, MyCreateFileA);

    if (DetourTransactionCommit() == NO_ERROR)
    {
        return true; 
    }
    return false; 
}

void RemoveHooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    
    DetourDetach(&(PVOID&)TrueCreateFontA, MyCreateFontA);
    DetourDetach(&(PVOID&)TrueCreateFontIndirectA, MyCreateFontIndirectA);
    DetourDetach(&(PVOID&)TrueCreateFileA, MyCreateFileA);

    DetourTransactionCommit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InstallHooks();
        break;
    case DLL_PROCESS_DETACH:
        RemoveHooks();
        break;
    }
    return TRUE;
}
