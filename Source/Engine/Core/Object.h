// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Core/StringId.h"

namespace Alimer
{
    /// Type info.
    class ALIMER_API TypeInfo final
    {
    public:
        /// Construct.
        TypeInfo(const char* typeName_, const TypeInfo* baseTypeInfo_);
        /// Destruct.
        ~TypeInfo() = default;

        /// Check current type is type of specified type.
        bool IsTypeOf(StringId32 type) const;
        /// Check current type is type of specified type.
        bool IsTypeOf(const TypeInfo* typeInfo) const;
        /// Check current type is type of specified class type.
        template <typename T> bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

        /// Return type.
        StringId32 GetType() const { return type; }
        /// Return type name.
        const String& GetTypeName() const { return typeName; }
        /// Return base type info.
        const TypeInfo* GetBaseTypeInfo() const { return baseTypeInfo; }

    private:
        /// Type.
        StringId32 type;
        /// Type name.
        String typeName;
        /// Base class type info.
        const TypeInfo* baseTypeInfo;
    };

    class ObjectFactory;
    template <class T> class ObjectFactoryImpl;
    template <class T> class ObjectCreateFactoryImpl;

    /// Base class for objects with type identification
    class ALIMER_API Object : public RefCounted
    {
    public:
        /// Constructor.
        Object() = default;
        /// Destructor.
        virtual ~Object() = default;

        /// Return type hash.
        virtual StringId32 GetType() const = 0;
        /// Return type name.
        virtual const std::string& GetTypeName() const = 0;
        /// Return type info.
        virtual const TypeInfo* GetTypeInfo() const = 0;

        /// Return type info static.
        static const TypeInfo* GetTypeInfoStatic() { return nullptr; }
        /// Check current instance is type of specified type.
        bool IsInstanceOf(StringId32 type) const;
        /// Check current instance is type of specified type.
        bool IsInstanceOf(const TypeInfo* typeInfo) const;
        /// Check current instance is type of specified class.
        template <typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }
        /// Cast the object to specified most derived class.
        template <typename T> T* Cast() { return IsInstanceOf<T>() ? static_cast<T*>(this) : nullptr; }
        /// Cast the object to specified most derived class.
        template <typename T> const T* Cast() const
        {
            return IsInstanceOf<T>() ? static_cast<const T*>(this) : nullptr;
        }

        /// Register an object factory.
        static void RegisterFactory(ObjectFactory* factory);
        /// Create an object by type hash. Return pointer to it or null if no factory found.
        static SharedPtr<Object> CreateObject(StringId32 objectType, const StringView& name);

        /// Register an object factory, template version.
        template <class T> static void RegisterFactory() { RegisterFactory(new ObjectFactoryImpl<T>()); }

        /// Register an object factory, template version.
        template <class T> static void RegisterCreateFactory() { RegisterFactory(new ObjectCreateFactoryImpl<T>()); }

        /// Create and return an object through a factory, template version.
        template <class T> static inline SharedPtr<T> CreateObject(const StringView& name = "")
        {
            return std::static_pointer_cast<T>(CreateObject(T::GetTypeStatic(), name));
        }

        /// Create and return an object through a factory, template version.
        template <class T> static inline SharedPtr<T> CreateObject(StringId32 objectType, const StringView& name = "")
        {
            return std::static_pointer_cast<T>(CreateObject(objectType, name));
        }

        /// Return a type name from hash, or empty if not known. Requires a registered object factory.
        static const String& GetTypeNameFromType(StringId32 type);
    };

    /// Base class for object factories.
    class ALIMER_API ObjectFactory
    {
    public:
        /// Destruct.
        virtual ~ObjectFactory() = default;

        /// Create an object.
        virtual SharedPtr<Object> Create(const StringView& name) = 0;

        /// Return type info of objects created by this factory.
        const TypeInfo* GetTypeInfo() const { return typeInfo; }

        /// Return type hash of objects created by this factory.
        StringId32 GetType() const { return typeInfo->GetType(); }

        /// Return type name of objects created by this factory.
        const String& GetTypeName() const { return typeInfo->GetTypeName(); }

    protected:
        /// Type info.
        const TypeInfo* typeInfo = nullptr;
    };

    /// Template implementation of the object factory.
    template <class T> class ObjectFactoryImpl : public ObjectFactory
    {
    public:
        /// Construct.
        ObjectFactoryImpl() { typeInfo = T::GetTypeInfoStatic(); }

        /// Create an object of the specific type.
        SharedPtr<Object> Create(const StringView& name) override { return std::make_shared<T>(name); }
    };

    /// Template implementation of the object factory.
    template <class T> class ObjectCreateFactoryImpl : public ObjectFactory
    {
    public:
        /// Construct.
        ObjectCreateFactoryImpl() { typeInfo = T::GetTypeInfoStatic(); }

        /// Create an object of the specific type.
        SharedPtr<Object> Create(const StringView& name) override { return T::Create(name); }
    };
}

#define ALIMER_OBJECT(typeName, baseTypeName) \
public: \
    using ClassName = typeName; \
    using Parent = baseTypeName; \
    virtual Alimer::StringId32 GetType() const override { return GetTypeInfoStatic()->GetType(); } \
    virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
    virtual const Alimer::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); }  \
    static Alimer::StringId32 GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
    static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
    static const Alimer::TypeInfo* GetTypeInfoStatic() \
    { \
        static const Alimer::TypeInfo typeInfoStatic(#typeName, Parent::GetTypeInfoStatic()); \
        return &typeInfoStatic; \
    }
