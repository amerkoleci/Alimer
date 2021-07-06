// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Core/String.h"
#include "Core/CommandLine.h"
#include <spdlog/fmt/fmt.h>

namespace Alimer::CommandLine
{
    static Vector<String> arguments;

    const Vector<String>& Parse(const String& cmdLine, bool skipFirstArgument)
    {
        arguments.clear();

        // At the time of writing compiler that comes with Visual Studio 16.5.4 broke. Passing cmdStart as
        // first parameter of substr() after for() loop crashes compiler. volatile prevents optimizer from
        // going off the rails and we get on with our lives.
#if _MSC_VER
        volatile
#endif
            size_t cmdStart = 0, cmdEnd = 0;
        bool inCmd = false;
        bool inQuote = false;

        for (size_t i = 0; i < cmdLine.length(); ++i)
        {
            if (cmdLine[i] == '\"')
                inQuote = !inQuote;
            if (cmdLine[i] == ' ' && !inQuote)
            {
                if (inCmd)
                {
                    inCmd = false;
                    cmdEnd = i;
                    // Do not store the first argument (executable name)
                    arguments.push_back(cmdLine.substr(cmdStart, cmdEnd - cmdStart));
                }
            }
            else
            {
                if (!inCmd)
                {
                    inCmd = true;
                    cmdStart = i;
                }
            }
        }
        if (inCmd)
        {
            cmdEnd = cmdLine.length();
            arguments.push_back(cmdLine.substr(cmdStart, cmdEnd - cmdStart));
        }

        // Strip double quotes from the arguments
        for (size_t i = 0; i < arguments.size(); ++i)
        {
            arguments[i] = ReplaceAll(arguments[i], "\"", "");
        }

        if (!arguments.empty())
        {
            //specifiedExecutableFile = arguments.front();
            if (skipFirstArgument)
                arguments.erase(arguments.begin());
        }

        return arguments;
    }

    const Vector<String>& Parse(const char* cmdLine)
    {
        return Parse(String(cmdLine));
    }

    const Vector<String>& Parse(const WString& cmdLine)
    {
        return Parse(ToUtf8(cmdLine));
    }

    const Vector<String>& Parse(const wchar_t* cmdLine)
    {
        WString wideBuffer(cmdLine);
        return Parse(ToUtf8(wideBuffer));
    }

    const Vector<String>& Parse(int argc, char** argv)
    {
        String cmdLine;

        for (int i = 0; i < argc; ++i)
        {
            cmdLine += fmt::format("\"{}\" ", (const char*)argv[i]);
        }

        return Parse(cmdLine);
    }

    const Vector<String>& GetArguments()
    {
        return arguments;
    }

    bool HasArgument(const String& argument)
    {
        for (size_t i = 0, count = argument.size(); i < count; ++i)
        {
            if (arguments[i].length() > 1)
            {
                if (arguments[i][0] == '-')
                {
                    String argumentCheck = ToLower(arguments[i].substr(1));
                    if (argumentCheck == argument)
                    {
                        return true;
                    }
                }
                else
                {
                    String argumentCheck = ToLower(arguments[i]);
                    if (argumentCheck == argument)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }
}
