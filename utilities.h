#pragma once

#define RUNNING_WINDOWS
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

namespace Utils
{

#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#include <Shlwapi.h>

inline std::string getWorkingDirectory()
{
//    HMODULE hModule = GetModuleHandle(nullptr);
//    if (hModule) {
//        char path[256];
//        GetModuleFileName(hModule, path, sizeof(path));
//        PathRemoveFileSpec(path);
//        strcat_s(path, "\\");
//        return std::string(path);
//    }

    return "";
}

inline void readQuotedString(std::stringstream &l_stream, std::string &l_string)
{
    l_stream >> l_string;
    if (l_string.at(0) == '"') {
        while (l_string.at(l_string.length() - 1) != '"' || !l_stream.eof()) {
            std::string str;
            l_stream >> str;
            l_string.append(" " + str);
        }
    }

    l_string.erase(std::remove(l_string.begin(), l_string.end(), '"'), l_string.end());
}
}
