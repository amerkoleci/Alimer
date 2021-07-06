// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/CommandLine.h"

#if defined(_WIN32) && !defined(ALIMER_WIN32_CONSOLE)
//#   include "Core/MiniDump.h"
#   include "PlatformInclude.h"
#   ifdef _MSC_VER
#       include <crtdbg.h>
#   endif
#endif

// MSVC debug mode: use memory leak reporting
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(ALIMER_WIN32_CONSOLE)
#define ALIMER_DEFINE_MAIN(function) \
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) \
{ \
    UNREFERENCED_PARAMETER(hPrevInstance);\
	UNREFERENCED_PARAMETER(lpCmdLine);\
	UNREFERENCED_PARAMETER(nCmdShow);\
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
    Alimer::CommandLine::Parse(GetCommandLineW()); \
    return function; \
}
// MSVC release mode: write minidump on crash
#elif defined(_MSC_VER) && defined(ALIMER_MINIDUMPS) && !defined(ALIMER_WIN32_CONSOLE)
#define ALIMER_DEFINE_MAIN(function) \
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) \
{ \
    UNREFERENCED_PARAMETER(hPrevInstance);\
	UNREFERENCED_PARAMETER(lpCmdLine);\
	UNREFERENCED_PARAMETER(nCmdShow);\
    Alimer::CommandLine::Parse(GetCommandLineW()); \
    int exitCode; \
    __try \
    { \
        exitCode = function; \
    } \
    __except(Alimer::WriteMiniDump("Alimer", GetExceptionInformation())) \
    { \
    } \
    return exitCode; \
}
// Other Win32 or minidumps disabled: just execute the function
#elif defined(_WIN32) && !defined(ALIMER_WIN32_CONSOLE)
#define ALIMER_DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
    Alimer::CommandLine::Parse(GetCommandLineW()); \
    return function; \
}
#else
#define ALIMER_DEFINE_MAIN(function) \
int main(int argc, char** argv) \
{ \
    Alimer::CommandLine::Parse(argc, argv); \
    return function; \
}
#endif
