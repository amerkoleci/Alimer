// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Assets/ShaderCompiler.h"
#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"
#include <spdlog/fmt/fmt.h>

#if defined(_WIN32)
#include <atlbase.h>
#include "PlatformInclude.h"
#include <dxcapi.h>

namespace Alimer::ShaderCompiler
{
    DxcCreateInstanceProc DxcCreateInstance = nullptr;

    [[nodiscard]] constexpr uint32_t GetMajor(ShaderModel shaderModel)
    {
        switch (shaderModel)
        {
            case ShaderModel::Model6_0:
            case ShaderModel::Model6_1:
            case ShaderModel::Model6_2:
            case ShaderModel::Model6_3:
            case ShaderModel::Model6_4:
            case ShaderModel::Model6_5:
            case ShaderModel::Model6_6:
            case ShaderModel::Model6_7:
                return 6;

            default:
                return 6;
                break;
        }
    }

    [[nodiscard]] constexpr uint32_t GetMinor(ShaderModel shaderModel)
    {
        switch (shaderModel)
        {
            case ShaderModel::Model6_0:
                return 0;
            case ShaderModel::Model6_1:
                return 1;
            case ShaderModel::Model6_2:
                return 2;
            case ShaderModel::Model6_3:
                return 3;
            case ShaderModel::Model6_4:
                return 4;
            case ShaderModel::Model6_5:
                return 5;
            case ShaderModel::Model6_6:
                return 6;
            case ShaderModel::Model6_7:
                return 7;

            default:
                return 0;
                break;
        }
    }

    std::wstring ShaderProfileName(ShaderStage stage, ShaderModel shaderModel)
    {
        uint32_t major = GetMajor(shaderModel);
        uint32_t minor = GetMinor(shaderModel);

        std::wstring shaderProfile;
        switch (stage)
        {
            case ShaderStage::Vertex:
                shaderProfile = L"vs";
                break;
            case ShaderStage::Hull:
                shaderProfile = L"hs";
                break;
            case ShaderStage::Domain:
                shaderProfile = L"ds";
                break;
            case ShaderStage::Geometry:
                shaderProfile = L"gs";
                break;
            case ShaderStage::Pixel:
                shaderProfile = L"ps";
                break;
            case ShaderStage::Compute:
                shaderProfile = L"cs";
                break;
            case ShaderStage::Mesh:
                shaderProfile = L"ms";
                if (shaderModel < ShaderModel::Model6_5)
                {
                    minor = GetMinor(ShaderModel::Model6_5);
                }
                break;
            case ShaderStage::Amplification:
                shaderProfile = L"as";
                if (shaderModel < ShaderModel::Model6_5)
                {
                    minor = GetMinor(ShaderModel::Model6_5);
                }
                break;
            case ShaderStage::Library:
                shaderProfile = L"lib";
                if (shaderModel < ShaderModel::Model6_5)
                {
                    minor = GetMinor(ShaderModel::Model6_5);
                }
                break;

            default:
                ALIMER_UNREACHABLE();
        }

        shaderProfile.push_back(L'_');
        shaderProfile.push_back(L'0' + major);
        shaderProfile.push_back(L'_');
        shaderProfile.push_back(L'0' + minor);

        return shaderProfile;
    }

    class IncludeHandler : public IDxcIncludeHandler
    {
    private:
        CComPtr<IDxcUtils> utils;
        std::string fullPath;

    public:
        IncludeHandler(CComPtr<IDxcUtils> utils_, const std::string& fullPath_)
            : utils(utils_)
            , fullPath(fullPath_)
        {
        }

        HRESULT STDMETHODCALLTYPE LoadSource(
            _In_ LPCWSTR pFilename,
            _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
        {
            // TODO: pFileName is different when in RELEASE
            auto filePath = Path::Join(fullPath, GetFileNameAndExtension(ToUtf8(pFilename)));
            if (!File::Exists(filePath))
            {
                return S_FALSE;
            }

            CComPtr<IDxcBlobEncoding> pEncoding;
            HRESULT hr = utils->LoadFile(ToUtf16(filePath).c_str(), DXC_CP_ACP, &pEncoding);
            if (SUCCEEDED(hr))
            {
                *ppIncludeSource = pEncoding.Detach();
            }
            return hr;
        }


        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override { return E_NOTIMPL; }
        ULONG STDMETHODCALLTYPE AddRef() override { return E_NOTIMPL; }
        ULONG STDMETHODCALLTYPE Release() override { return E_NOTIMPL; }
    };

    bool Compile(const std::string& fileName, Shader* shader)
    {
        ShaderCompileOptions options{};
        options.source = File::ReadAllText(fileName);
        return Compile(options, shader);
    }

    bool Compile(ShaderStage stage, const std::string& fileName, Shader* shader)
    {
        ShaderCompileOptions options{};
        options.source = File::ReadAllText(fileName);
        if (stage == ShaderStage::Vertex)
        {
            options.entryPoint = "VSMain";
        }
        else if (stage == ShaderStage::Pixel)
        {
            options.entryPoint = "PSMain";
        }

        options.fileName = fileName;
        options.stage = stage;
        return Compile(options, shader);
    }

    bool Compile(const ShaderCompileOptions& options, Shader* shader)
    {
        if (DxcCreateInstance == nullptr)
        {
#if defined(_WIN32)
            HMODULE dxcompilerDLL = LoadLibraryW(L"dxcompiler.dll");
            if (dxcompilerDLL == nullptr)
            {
                LOGE("Failed to load dxcompiler.dll");
                return {};
            }

            DxcCreateInstance = (DxcCreateInstanceProc)GetProcAddress(dxcompilerDLL, "DxcCreateInstance");
            ALIMER_ASSERT(DxcCreateInstance != nullptr);
            LOGD("ShaderCompiler: dxcompiler.dll with success");
#endif
        }

        CComPtr<IDxcUtils> dxcUtils;
        CComPtr<IDxcCompiler3> dxcCompiler;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler)));

        // Setup arguments 
        std::vector<LPCWSTR> args;

        // Add source file name if present. This will be displayed in error messages
        if (!options.fileName.empty())
        {
            std::wstring wFileName = ToUtf16(options.fileName);
            args.push_back(wFileName.c_str());
        }

        // Entry point
        std::wstring entryPointW = ToUtf16(options.entryPoint);
        args.push_back(L"-E"); args.push_back(entryPointW.c_str());

        // Target
        args.push_back(L"-T");
        auto profile = ShaderProfileName(options.stage, options.shaderModel);
        args.push_back(profile.c_str());

        args.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR); // Column major
#ifdef _DEBUG
        args.push_back(DXC_ARG_SKIP_OPTIMIZATIONS); // Skip optimizations.
        args.push_back(DXC_ARG_DEBUG); // Enable debug information.
#else
        args.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

        args.push_back(DXC_ARG_RESOURCES_MAY_ALIAS);
        args.push_back(L"-flegacy-macro-expansion");
        //args.push_back(L"-no-legacy-cbuf-layout");
        //args.push_back(L"-pack-optimized");
        //args.push_back(DXC_ARG_ALL_RESOURCES_BOUND);

        // Defines
        for (auto& define : options.defines)
        {
            std::wstring wDefine = ToUtf16(define);
            args.push_back(L"-D"); args.push_back(wDefine.c_str());
        }

        if (GDevice->CheckCapability(GRAPHICSDEVICE_CAPABILITY_BINDLESS_DESCRIPTORS))
        {
            args.push_back(L"-D"); args.push_back(L"BINDLESS");
        }

        switch (GDevice->GetShaderFormat())
        {
            case ShaderFormat::DXIL:
                args.push_back(L"-D"); args.push_back(L"DXIL");
                break;
            case ShaderFormat::SPIRV:
                args.push_back(L"-D"); args.push_back(L"SPIRV");
                args.push_back(L"-spirv");
                args.push_back(L"-fspv-target-env=vulkan1.2");
                args.push_back(L"-fvk-use-dx-layout");
                args.push_back(L"-fvk-use-dx-position-w");

                //args.push_back(L"-fvk-b-shift"); args.push_back(L"0"); args.push_back(L"0");
                args.push_back(L"-fvk-t-shift"); args.push_back(L"1000"); args.push_back(L"0");
                args.push_back(L"-fvk-u-shift"); args.push_back(L"2000"); args.push_back(L"0");
                args.push_back(L"-fvk-s-shift"); args.push_back(L"3000"); args.push_back(L"0");
                break;
        }

        std::string fullPath = GetPath(options.fileName);
        if (!fullPath.empty())
        {
            fullPath = Path::Join(Directory::GetCurrent(), fullPath);
            //std::wstring wFullPath = StringUtils::ToUtf16(fullPath);
            //args.push_back(L"-I");
            //args.push_back(wFullPath.c_str());
        }

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = options.source.c_str();
        sourceBuffer.Size = options.source.size();
        sourceBuffer.Encoding = DXC_CP_ACP;

        IncludeHandler includeHandler(dxcUtils, fullPath);

        CComPtr<IDxcResult> pResults;
        HRESULT hr = dxcCompiler->Compile(
            &sourceBuffer,                          // Source buffer.
            args.data(),							// Array of pointers to arguments.
            static_cast<UINT32>(args.size()),		// Number of arguments.
            &includeHandler,
            IID_PPV_ARGS(&pResults)                  // Compiler output status, buffer, and errors.
        );

        if (FAILED(hr)) {
            return {};
        }

        CComPtr<IDxcBlobUtf8> pErrorsUtf8 = nullptr;
        hr = pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrorsUtf8), nullptr);
        ALIMER_ASSERT(SUCCEEDED(hr));
        if (pErrorsUtf8 != nullptr && pErrorsUtf8->GetStringLength() > 0)
        {
            OutputDebugStringA(pErrorsUtf8->GetStringPointer());
            std::string errorMessage = pErrorsUtf8->GetStringPointer();
            LOGE("Failed to compiler shader, errors: {}", errorMessage);
        }

        HRESULT hrStatus;
        hr = pResults->GetStatus(&hrStatus);
        ALIMER_ASSERT(SUCCEEDED(hr));
        if (FAILED(hrStatus)) {
            return {};
        }

        // Save shader binary.
        CComPtr<IDxcBlob> pShader = nullptr;
        //CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
        hr = pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr/* &pShaderName*/);
        ALIMER_ASSERT(SUCCEEDED(hr));

#if TODO
        if (pShader != nullptr)
        {
            FILE* fp = NULL;

            _wfopen_s(&fp, pShaderName->GetStringPointer(), L"wb");
            fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
            fclose(fp);
        }
#endif

        std::vector<uint8_t> bytecode(pShader->GetBufferSize());
        memcpy(bytecode.data(), pShader->GetBufferPointer(), pShader->GetBufferSize());
        return GDevice->CreateShader(options.stage, bytecode.data(), bytecode.size(), shader);
    }
}

#else

namespace Alimer
{
    std::vector<uint8_t> Compile(const ShaderCompileOptions& options)
    {
        return {};
    }
}

#endif
