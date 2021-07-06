// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"

namespace Alimer::CommandLine
{
    /// Parse arguments from the command line. First argument is by default assumed to be the executable name and is skipped.
    ALIMER_API const Vector<String>& Parse(const String& cmdLine, bool skipFirstArgument = true);
    /// Parse arguments from the command line.
    ALIMER_API const Vector<String>& Parse(const char* cmdLine);
    /// Parse arguments from a wide char command line.
    ALIMER_API const Vector<String>& Parse(const WString& cmdLine);
    /// Parse arguments from a wide char command line.
    ALIMER_API const Vector<String>& Parse(const wchar_t* cmdLine);
    /// Parse arguments from argc & argv.
    ALIMER_API const Vector<String>& Parse(int argc, char** argv);
    /// Return previously parsed arguments.
    ALIMER_API const Vector<String>& GetArguments();
    /// Check if given command line argument is passed.
    ALIMER_API bool HasArgument(const String& argument);
}
