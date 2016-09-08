/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 * @copyright Gerard Godone-Maresca, 2016, GNU Public License v3       *
 * @link https://github.com/ggodone-maresca/ID3-Tagging-Library        *
 **********************************************************************/

#ifndef ID3_EXCEPTION_HPP
#define ID3_EXCEPTION_HPP

#include <exception> //For std::exception
#include <string>    //For strings
#include <vector>    //For vectors

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * ID3v2.3.0 standard: http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: http://id3.org/id3v2.4.0-structure
 * 
 * @see ID3.h
 */
namespace ID3 {
	/**
	 * The base exception class.
	 */
	class Exception : virtual public std::exception {
		public:
			/**
			 * The empty constructor.
			 */
			Exception() noexcept;
			
			/**
			 * Create an Exception from another exception.
			 * 
			 * @param e Another exception object.
			 */
			Exception(const std::exception& e) noexcept;
			
			/**
			 * Create an Exception with a custom message.
			 * 
			 * @param customError The custom message.
			 */
			Exception(const std::string& customError) noexcept;
			
			/**
			 * The destructor.
			 */
			virtual ~Exception();
			
			/**
			 * Return a null-terminated character sequence that is used to identify
			 * the exception.
			 * 
			 * @return The exception message.
			 * @see std::exception::what()
			 */
			virtual inline const char* what() const noexcept final { return message.c_str(); }
		
		protected:
			/**
			 * The message that gets returned upon calling what().
			 * 
			 * @see ID3::Exception::what()
			 */
			std::string message;			
	};
	
	/**
	 * An exception that is thrown when trying to open a file that does not exist.
	 */
	class FileNotFoundException : virtual public Exception {
		public:
			/** @see ID3::Exception::Exception() */
			FileNotFoundException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			FileNotFoundException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			FileNotFoundException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~FileNotFoundException();
	};
	
	/**
	 * An exception that is thrown when trying to read/write tags from/to a file
	 * that is not an MP3 or MP4 file.
	 */
	class NotMP3FileException : virtual public Exception {
		public:
			/** @see ID3::Exception::Exception() */
			NotMP3FileException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			NotMP3FileException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			NotMP3FileException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~NotMP3FileException();
	};
	
	/**
	 * An exception that is thrown when the tag size on file is bigger than the
	 * file itself.
	 */
	class FileFormatException : virtual public Exception {
		public:
			/** @see ID3::Exception::Exception() */
			FileFormatException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			FileFormatException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			FileFormatException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~FileFormatException();
	};
	
	/**
	 * The base size exception class.
	 */
	class SizeException : virtual public Exception {
		public:
			/** @see ID3::Exception::Exception() */
			SizeException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			SizeException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			SizeException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~SizeException();
	};
	
	/**
	 * An exception that is thrown when writing to the file, when the size of the
	 * tag would exceed the maximum allowed size (28 bits).
	 * 
	 * @see ID3::MAX_TAG_SIZE
	 */
	class TagSizeException : virtual public SizeException {
		public:
			/** @see ID3::Exception::Exception() */
			TagSizeException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			TagSizeException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			TagSizeException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~TagSizeException();
	};
	
	/**
	 * An exception that is thrown when writing a frame, when its size would
	 * exceed the maximum allowed size (28 bits).
	 * 
	 * @see ID3::MAX_TAG_SIZE
	 */
	class FrameSizeException : virtual public SizeException {
		public:
			/** @see ID3::Exception::Exception() */
			FrameSizeException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			FrameSizeException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			FrameSizeException(const std::string& customError) noexcept;
			
			/**
			 * Create an FrameSizeException with the frame ID and description in
			 * the exception message.
			 * 
			 * @param frameID          The ID3v2 frame ID.
			 * @param frameDescription The ID3v2 frame description.
			 */
			FrameSizeException(const std::string& frameID, const std::string& frameDescription) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~FrameSizeException();
	};
	
	/**
	 * An exception that is thrown when the an error occurs when writing to the
	 * file, but one of the above exceptions don't cover the issue.
	 */
	class WriteException : virtual public Exception {
		public:
			/** @see ID3::Exception::Exception() */
			WriteException() noexcept;
			
			/** @see ID3::Exception::Exception(std::exception&) */
			WriteException(const std::exception& e) noexcept;
			
			/** @see ID3::Exception::Exception(std::string&) */
			WriteException(const std::string& customError) noexcept;
			
			/** @see ID3::Exception::~Exception() */
			virtual ~WriteException();
	};
}

#endif
