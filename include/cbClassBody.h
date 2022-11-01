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
		static inline std::string GetStaticClassID()																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetClassID() const																								\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetDerivedClassID() const																						\
		{																																	\
			return std::string("");																											\
		}																																	\
		static inline std::string GetStaticDerivedClassID()																					\
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
		virtual std::string GetClassID() const override																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		static inline std::string GetStaticClassID()																						\
		{																																	\
			return std::string(#ClassType);																									\
		}																																	\
		virtual std::string GetDerivedClassID() const override																				\
		{																																	\
			return std::string(#Derived);																									\
		}																																	\
		static inline std::string GetStaticDerivedClassID()																					\
		{																																	\
			return std::string(#Derived);																									\
		}
#endif

#endif

}
