/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coþkun.
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ---------------------------------------------------------------------------------------
*/
#pragma once

#include <string>
#include <memory>
#include <set>
#include <vector>

namespace cbgui
{
#ifndef cbFORCEINLINE
#define cbFORCEINLINE __forceinline
#endif

// Functions that became constexpr in C++20
#if _MSVC_LANG >= 202002L
	#ifndef cbCONSTEXPR20
	#define cbCONSTEXPR20 constexpr
	#endif
#else
	#ifndef cbCONSTEXPR20
	#define cbCONSTEXPR20 inline
	#endif
#endif

#ifndef cbClassBody

#ifndef cbClassConstructor
/* Helper macro to create class. */
#define cbClassConstructor(Class)																											\
	public:																																	\
		template <typename... Args>																											\
		static inline Class* CreateNew(Args&&... other)																						\
		{																																	\
			auto pClass = new Class(std::forward<Args>(other)...);																			\
			return pClass;																													\
		}																																	\
		template <typename... Args>																											\
		static inline Class::SharedPtr Create(Args&&... other)																				\
		{																																	\
			auto pClass = std::make_shared<Class>(std::forward<Args>(other)...);															\
			return pClass;																													\
		}																																	\
		template <typename... Args>																											\
		static inline Class::UniquePtr CreateUnique(Args&&... other)																		\
		{																																	\
			auto pClass = std::make_unique<Class>(std::forward<Args>(other)...);															\
			return pClass;																													\
		}
#endif

#ifndef cbClassDefaultProtectedConstructor
/* Helper macro to Default Protected Constructor. */
#define cbClassDefaultProtectedConstructor(Class)		\
protected:									\
	Class() = default;						\
public:										\
	virtual ~Class() = default;
#endif

#ifndef cbClassNoDefaults
/* Empty macro. */
#define cbClassNoDefaults(Class)
#endif

#ifndef cbBaseClassBody
/* Macro for helper functions. */
#define cbBaseClassBody(ClassDefaults, ClassType)																							\
	public:																																	\
		using SharedPtr = std::shared_ptr<ClassType>;																						\
		using WeakPtr = std::weak_ptr<ClassType>;																							\
		using UniquePtr = std::unique_ptr<ClassType>;																						\
																																			\
		ClassDefaults(ClassType);																											\
																																			\
		static inline std::size_t GetStaticHashCode()																						\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		static inline std::set<std::size_t> GetStaticHashHierarchy()																		\
		{																																	\
			static const std::set<std::size_t> HASH_Hierarchy = { GetStaticHashCode() };													\
			return HASH_Hierarchy;																											\
		}																																	\
		static inline bool StaticHasMultipleInheritance()																					\
		{																																	\
			return false;																													\
		}																																	\
		static inline std::string GetStaticClassID()																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		static inline std::string GetStaticDerivedClassID()																					\
		{																																	\
			return std::string("");																											\
		}																																			\
		virtual std::size_t GetHashCode() const																								\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		virtual std::set<std::size_t> GetHashHierarchy()	const																			\
		{																																	\
			static const std::set<std::size_t> HASH_Hierarchy = { GetHashCode() };															\
			return HASH_Hierarchy;																											\
		}																																	\
		virtual bool IsA(std::size_t Hash) const																							\
		{																																	\
			static const std::set<std::size_t> HASH_Hierarchy = GetHashHierarchy();															\
			return HASH_Hierarchy.find(Hash) != HASH_Hierarchy.end();																		\
		}																																	\
		template<typename T>																												\
		cbFORCEINLINE bool IsA()																											\
		{																																	\
			return IsA(T::GetStaticHashCode());																								\
		}																																	\
		virtual bool HasMultipleInheritance()																								\
		{																																	\
			return false;																													\
		}																																	\
		virtual std::string GetClassID() const																								\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetDerivedClassID() const																						\
		{																																	\
			return std::string("");																											\
		}		
#endif

#ifndef cbClassBody
/* Macro for helper functions. */
#define cbClassBody(ClassDefaults, ClassType, Derived)																						\
	private:																																\
		typedef Derived Super;																												\
	public:																																	\
		using SharedPtr = std::shared_ptr<ClassType>;																						\
		using WeakPtr = std::weak_ptr<ClassType>;																							\
		using UniquePtr = std::unique_ptr<ClassType>;																						\
																																			\
		ClassDefaults(ClassType);																											\
																																			\
		static inline std::size_t GetStaticHashCode()																						\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		static inline std::set<std::size_t> GetStaticHashHierarchy()																		\
		{																																	\
			auto Append = [](const std::set<std::size_t>& v1, const std::size_t Hash) -> std::set<std::size_t>								\
			{																																\
				std::set<std::size_t> vr(std::begin(v1), std::end(v1));																		\
				vr.insert(Hash);																											\
				return vr;																													\
			};																																\
																																			\
			static const std::set<std::size_t> HASH_Hierarchy = Append(Derived::GetStaticHashHierarchy(), GetStaticHashCode());				\
			return HASH_Hierarchy;																											\
		}																																	\
		static inline bool StaticHasMultipleInheritance()																					\
		{																																	\
			return Derived::StaticHasMultipleInheritance();																					\
		}																																	\
		static inline std::string GetStaticClassID()																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		static inline std::string GetStaticDerivedClassID()																					\
		{																																	\
			return std::string(#Derived);																									\
		}																																	\
		virtual std::size_t GetHashCode() const override																					\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		virtual std::set<std::size_t> GetHashHierarchy() const override																		\
		{																																	\
			auto Append = [](const std::set<std::size_t>& v1, const std::size_t Hash) -> std::set<std::size_t>								\
			{																																\
				std::set<std::size_t> vr(std::begin(v1), std::end(v1));																		\
				vr.insert(Hash);																											\
				return vr;																													\
			};																																\
			static const std::set<std::size_t> HASH_Hierarchy = Append(Derived::GetStaticHashHierarchy(), GetStaticHashCode());				\
			return HASH_Hierarchy;																											\
		}																																	\
		virtual bool IsA(std::size_t Hash) const override																					\
		{																																	\
			static const std::set<std::size_t> HASH_Hierarchy = GetHashHierarchy();															\
			return HASH_Hierarchy.find(Hash) != HASH_Hierarchy.end();																		\
		}																																	\
		virtual bool HasMultipleInheritance() override																						\
		{																																	\
			return Derived::HasMultipleInheritance();																						\
		}																																	\
		virtual std::string GetClassID() const override																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetDerivedClassID() const override																				\
		{																																	\
			return std::string(#Derived);																									\
		}														
#endif

#ifndef sMultiClassBody
/* Macro for helper functions. */
#define sMultiClassBody(ClassDefaults, ClassType, Derived1, Derived2)																		\
	private:																																\
		typedef Derived1 Super;																												\
		typedef Derived1 MSuper1;																											\
		typedef Derived2 MSuper2;																											\
	public:																																	\
		using SharedPtr = std::shared_ptr<ClassType>;																						\
		using WeakPtr = std::weak_ptr<ClassType>;																							\
		using UniquePtr = std::unique_ptr<ClassType>;																						\
																																			\
		ClassDefaults(ClassType);																											\
																																			\
		static inline std::size_t GetStaticHashCode()																						\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		static inline std::set<std::size_t> GetStaticHashHierarchy()																		\
		{																																	\
			auto Append = [](const std::set<std::size_t>& v1, const std::set<std::size_t>& v2,												\
								const std::size_t Hash) -> std::set<std::size_t>															\
			{																																\
				std::set<std::size_t> vr(std::begin(v1), std::end(v1));																		\
				vr.insert(std::begin(v2), std::end(v2));																					\
				vr.insert(Hash);																											\
				return vr;																													\
			};																																\
																																			\
			static const std::set<std::size_t> HASH_Hierarchy = Append(Derived1::GetStaticHashHierarchy(),									\
				Derived2::GetStaticHashHierarchy(), GetStaticHashCode());																	\
			return HASH_Hierarchy;																											\
		}																																\
		static inline bool StaticHasMultipleInheritance()																					\
		{																																	\
			return true;																													\
		}																																	\
		static inline std::string GetStaticClassID()																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		static inline std::string GetStaticDerivedClassID()																					\
		{																																	\
			return std::string(#Derived1) + ":&&:" + std::string(#Derived2);																\
		}																																	\
		virtual std::size_t GetHashCode() const override																					\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		virtual std::set<std::size_t> GetHashHierarchy() const override																		\
		{																																	\
			auto Append = [](const std::set<std::size_t>& v1, const std::set<std::size_t>& v2,												\
					const std::size_t Hash) -> std::set<std::size_t>																		\
			{																																\
				std::set<std::size_t> vr(std::begin(v1), std::end(v1));																		\
				vr.insert(std::begin(v2), std::end(v2));																					\
				vr.insert(Hash);																											\
				return vr;																													\
			};																																\
			static const std::set<std::size_t> HASH_Hierarchy = Append(Derived1::GetHashHierarchy(),										\
								Derived2::GetHashHierarchy(), GetHashCode());																\
			return HASH_Hierarchy;																											\
		}																																	\
		virtual bool IsA(std::size_t Hash) const override																					\
		{																																	\
			static const std::set<std::size_t> HASH_Hierarchy = GetHashHierarchy();															\
			return HASH_Hierarchy.find(Hash) != HASH_Hierarchy.end();																		\
		}																																	\
		virtual bool HasMultipleInheritance() override																						\
		{																																	\
			return true;																													\
		}																																	\
		virtual std::string GetClassID() const override																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetDerivedClassID() const override																				\
		{																																	\
			return std::string(#Derived1) + ":&&:" + std::string(#Derived2);																\
		}			
#endif

#ifndef sStaticClassBody
/* Macro for helper functions. */
#define sStaticClassBody(ClassType)																											\
	public:																																	\
		using SharedPtr = std::shared_ptr<ClassType>;																						\
		using WeakPtr = std::weak_ptr<ClassType>;																							\
		using UniquePtr = std::unique_ptr<ClassType>;																						\
																																			\
		cbClassConstructor(ClassType);																										\
																																			\
		static inline std::size_t GetHashCode()																								\
		{																																	\
			static std::size_t HASH;																										\
			return reinterpret_cast<std::size_t>(&HASH);																					\
		}																																	\
		static inline std::string GetClassID()																								\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		static inline std::string GetDerivedClassID()																						\
		{																																	\
			return std::string("");																											\
		}
#endif

#endif

	template<typename To, typename From>
	cbFORCEINLINE To* cbCast(From* Src)
	{
		return Src->HasMultipleInheritance() ? dynamic_cast<To*>(Src) : Src->IsA(To::GetStaticHashCode()) ? (To*)Src : nullptr;
	}

	template<typename To, typename From>
	cbFORCEINLINE To* cbCast(From* Src, std::size_t Hash)
	{
		return Src->HasMultipleInheritance() ? dynamic_cast<To*>(Src) : Src->IsA(Hash) ? (To*)Src : nullptr;
	}
}
