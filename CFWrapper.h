/*
 *  Copyright (C) 2012, 2013, 2014, 2015 Stephen F. Booth <me@sbooth.org>
 *  All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <iostream>

#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#if !TARGET_OS_IPHONE
# include <Security/Security.h>
# include <ImageIO/ImageIO.h>
#endif

/*! @file CFWrapper.h @brief A wrapper around a Core Foundation object */

/*! @brief \c SFBAudioEngine's encompassing namespace */
namespace SFB {

	/*!
	 * @brief A wrapper around a Core Foundation object
	 *
	 * \c CFWrapper simplifies the use of CFTypes in C++ by wrapping a CF object, ensuring
	 * \c CFRelease will be called when the \c CFWrapper goes out of scope.
	 * @tparam T A \c CFType
	 */
	template <typename T>
	class CFWrapper
	{
	public:
		
		// ========================================
		/*! @name Creation and Destruction */
		//@{

		/*! @brief Create a new \c CFWrapper */
		inline CFWrapper()						: CFWrapper(nullptr)					{}

		/*!
		 * @brief Create a new \c CFWrapper
		 * @note The \c CFWrapper takes ownership of \c object
		 * @param object The object to wrap
		 */
		inline CFWrapper(T object)				: CFWrapper(object, true)				{}

		/*!
		 * @brief Create a new \c CFWrapper
		 * @param object The object to wrap
		 * @param release Whether this \c CFWrapper should take ownership of \c object
		 */
		CFWrapper(T object, bool release)		: mObject(object), mRelease(release)	{}


		/*! @brief Create a new \c CFWrapper */
		CFWrapper(CFWrapper&& rhs)
			: mObject(rhs.mObject), mRelease(rhs.mRelease)
		{
			rhs.mObject = nullptr;
		}

		/*! @brief Create a new \c CFWrapper */
		CFWrapper(const CFWrapper& rhs)
			: mObject(rhs.mObject), mRelease(rhs.mRelease)
		{
			if(mObject && mRelease)
				CFRetain(mObject);
		}

		/*! @brief Destroy this \c CFWrapper and ensure \c CFRelease() is called if necessary */
		~CFWrapper()
		{
			if(mObject && mRelease)
				CFRelease(mObject);
			mObject = nullptr;
		}

		//@}


		/*! @name Assignment */
		//@{

		/*!
		 * @brief Replace the wrapped object
		 * @note The \c CFWrapper takes ownership of \c rhs
		 * @param rhs The object to wrap
		 */
		CFWrapper& operator=(const T& rhs)
		{
			if(mObject != rhs) {
				if(mObject && mRelease)
					CFRelease(mObject);

				mObject = rhs;
				mRelease = true;
			}
			
			return *this;
		}

		/*! @brief Replace the wrapped object */
		CFWrapper& operator=(const CFWrapper& rhs)
		{
			if(mObject != rhs.mObject) {
				if(mObject && mRelease)
					CFRelease(mObject);

				mObject = rhs.mObject;
				mRelease = rhs.mRelease;

				if(mObject && mRelease)
					CFRetain(mObject);
			}
			
			return *this;
		}

		/*! @brief Replace the wrapped object */
		CFWrapper& operator=(CFWrapper&& rhs)
		{
			if(mObject != rhs.mObject) {
				if(mObject && mRelease)
					CFRelease(mObject);

				mObject = rhs.mObject;
				mRelease = rhs.mRelease;

				rhs.mObject = nullptr;
			}

			return *this;
		}

		//@}


		// ========================================
		/*! @name Pointer management */
		//@{

		/*! @brief Relinquish ownership of the wrapped object and return it */
		inline T Relinquish()
		{
			T object = mObject;
			mObject = nullptr;

			return object;
		}

		//@}


		// ========================================
		/*! @name Equality testing */
		//@{

		/*! @brief Test two \c CFWrapper objects for equality using \c CFEqual() */
		inline bool operator==(const CFWrapper& rhs) const
		{
			if(mObject == rhs.mObject)
				return true;

			// CFEqual doesn't handle nullptr
			if(!mObject || !rhs.mObject)
				return false;

			return CFEqual(mObject, rhs.mObject);
		}

		/*! @brief Test two \c CFWrapper objects for inequality */
		inline bool operator!=(const CFWrapper& rhs) const		{ return !operator==(rhs); }

		//@}


		// ========================================
		/*! @name CoreFoundation object access */
		//@{

		/*! @brief Check whether the wrapped object is not \c nullptr */
		inline explicit operator bool() const					{ return nullptr != mObject; }

		/*! @brief Check whether the wrapped object is \c nullptr */
		inline bool operator!() const							{ return nullptr == mObject; }

		/*! @brief Get the wrapped object */
		inline operator T() const								{ return mObject; }

		
		/*! @brief Get the wrapped object */
		inline T Object() const									{ return mObject; }

		//@}

	private:
		T mObject;				/*!< The Core Foundation object */
		bool mRelease;			/*!< Whether \c CFRelease should be called on destruction or reassignment */
	};

	// ========================================
	// Typedefs for common CoreFoundation types

	using CFType = CFWrapper<CFTypeRef>;										/*!< @brief A wrapped \c CFTypeRef */
	using CFData = CFWrapper<CFDataRef>;										/*!< @brief A wrapped \c CFDataRef */
	using CFMutableData = CFWrapper<CFMutableDataRef>;							/*!< @brief A wrapped \c CFMutableDataRef */
	using CFString = CFWrapper<CFStringRef>;									/*!< @brief A wrapped \c CFStringRef */
	using CFMutableString = CFWrapper<CFMutableStringRef>;						/*!< @brief A wrapped \c CFMutableStringRef */
	using CFAttributedString = CFWrapper<CFAttributedStringRef>;				/*!< @brief A wrapped \c CFAttributedStringRef */
	using CFMutableAttributedString = CFWrapper<CFMutableAttributedStringRef>;	/*!< @brief A wrapped \c CFMutableAttributedStringRef */
	using CFDictionary = CFWrapper<CFDictionaryRef>;							/*!< @brief A wrapped \c CFDictionaryRef */
	using CFMutableDictionary = CFWrapper<CFMutableDictionaryRef>;				/*!< @brief A wrapped \c CFMutableDictionaryRef */
	using CFArray = CFWrapper<CFArrayRef>;										/*!< @brief A wrapped \c CFArrayRef */
	using CFMutableArray = CFWrapper<CFMutableArrayRef>;						/*!< @brief A wrapped \c CFMutableArrayRef */
	using CFSet = CFWrapper<CFSetRef>;											/*!< @brief A wrapped \c CFSetRef */
	using CFMutableSet = CFWrapper<CFMutableSetRef>;							/*!< @brief A wrapped \c CFMutableSetRef */
	using CFBag = CFWrapper<CFBagRef>;											/*!< @brief A wrapped \c CFBagRef */
	using CFMutableBag = CFWrapper<CFMutableBagRef>;							/*!< @brief A wrapped \c CFMutableBagRef */
	using CFPropertyList = CFWrapper<CFPropertyListRef>;						/*!< @brief A wrapped \c CFPropertyListRef */
	using CFBitVector = CFWrapper<CFBitVectorRef>;								/*!< @brief A wrapped \c CFBitVectorRef */
	using CFMutableBitVector = CFWrapper<CFMutableBitVectorRef>;				/*!< @brief A wrapped \c CFMutableBitVectorRef */
	using CFCharacterSet = CFWrapper<CFCharacterSetRef>;						/*!< @brief A wrapped \c CFCharacterSetRef */
	using CFMutableCharacterSet = CFWrapper<CFMutableCharacterSetRef>;			/*!< @brief A wrapped \c CFMutableCharacterSetRef */
	using CFURL = CFWrapper<CFURLRef>;											/*!< @brief A wrapped \c CFURLRef */
	using CFUUID = CFWrapper<CFUUIDRef>;										/*!< @brief A wrapped \c CFUUIDRef */
	using CFNumber = CFWrapper<CFNumberRef>;									/*!< @brief A wrapped \c CFNumberRef */
	using CFBoolean = CFWrapper<CFBooleanRef>;									/*!< @brief A wrapped \c CFBooleanRef */
	using CFError = CFWrapper<CFErrorRef>;										/*!< @brief A wrapped \c CFErrorRef */
	using CFDate = CFWrapper<CFDateRef>;										/*!< @brief A wrapped \c CFDateRef */
	using CFReadStream = CFWrapper<CFReadStreamRef>;							/*!< @brief A wrapped \c CFReadStream */
	using CFWriteStream = CFWrapper<CFWriteStreamRef>;							/*!< @brief A wrapped \c CFWriteStream */
	using CFHTTPMessage = CFWrapper<CFHTTPMessageRef>;							/*!< @brief A wrapped \c CFHTTPMessageRef */
#if !TARGET_OS_IPHONE
	using SecKeychainItem = CFWrapper<SecKeychainItemRef>;						/*!< @brief A wrapped \c SecKeychainItemRef */
	using SecCertificate = CFWrapper<SecCertificateRef>;						/*!< @brief A wrapped \c SecCertificateRef */
	using SecTransform = CFWrapper<SecTransformRef>;							/*!< @brief A wrapped \c SecTransformRef */
	using CGImageSource = CFWrapper<CGImageSourceRef>;							/*!< @brief A wrapped \c CGImageSourceRef */
#endif

}

/*! @cond */

template <typename T>
std::ostream& operator<<(std::ostream& out, SFB::CFWrapper<T> obj)
{
	out << (T)obj;
	return out;
}

/*! @endcond */
