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

#include "ID3Exception.hpp" //For the class definition
#include "ID3Constants.hpp" //For MAX_TAG_SIZE

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  E X C E P T I O N //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
Exception::Exception() noexcept :
    message("ID3::Exception: generic exception.\n") {}

///@pkg ID3Exception.hpp
Exception::Exception(const std::exception& e) noexcept :
    message(std::string("std::exception: ") + e.what()) {}

///@pkg ID3Exception.hpp
Exception::Exception(const std::string& customError) noexcept :
    message("ID3::Exception: " + (customError.empty() ? "generic exception.\n" :
                                                        customError + '\n')) {}

///@pkg ID3Exception.hpp
Exception::~Exception() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////  F I L E N O T F O U N D E X C E P T I O N //////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
FileNotFoundException::FileNotFoundException() noexcept {
	message = "ID3::FileNotFoundException: file was not found!\n";
}

///@pkg ID3Exception.hpp
FileNotFoundException::FileNotFoundException(const std::exception& e) noexcept : Exception(e) {}

///@pkg ID3Exception.hpp
FileNotFoundException::FileNotFoundException(const std::string& customError) noexcept {
	message = "ID3::FileNotFoundException: " + customError.empty() ? "file stream was not found!\n" :
	                                                                 customError + '\n';
}

///@pkg ID3Exception.hpp
FileNotFoundException::~FileNotFoundException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////  F I L E N O T O P E N E X C E P T I O N ///////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
FileNotOpenException::FileNotOpenException() noexcept {
	message = "ID3::FileNotOpenException: file stream is not open!\n";
}

///@pkg ID3Exception.hpp
FileNotOpenException::FileNotOpenException(const std::exception& e) noexcept : Exception(e) {}

///@pkg ID3Exception.hpp
FileNotOpenException::FileNotOpenException(const std::string& customError) noexcept {
	message = "ID3::FileNotOpenException: " + (customError.empty() ? "file stream is not open!\n" :
	                                                                 customError + '\n');
}

///@pkg ID3Exception.hpp
FileNotOpenException::~FileNotOpenException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////  N O T M P 3 F I L E E X C E P T I O N ////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
NotMP3FileException::NotMP3FileException() noexcept {
	message = "ID3::NotMP3FileException: file is not an MP3 or MP4 file!\n";
}

///@pkg ID3Exception.hpp
NotMP3FileException::NotMP3FileException(const std::exception& e) noexcept : Exception(e) {}

///@pkg ID3Exception.hpp
NotMP3FileException::NotMP3FileException(const std::string& customError) noexcept {
	message = "ID3::NotMP3FileException: " + (customError.empty() ? "file is not an MP3 or MP4 file!\n" :
	                                                                customError + '\n');
}

///@pkg ID3Exception.hpp
NotMP3FileException::~NotMP3FileException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////  F I L E F O R M A T E X C E P T I O N ////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
FileFormatException::FileFormatException() noexcept {
	message = "ID3::FormatException: file format error!\n";
}

///@pkg ID3Exception.hpp
FileFormatException::FileFormatException(const std::exception& e) noexcept : Exception(e) {}

///@pkg ID3Exception.hpp
FileFormatException::FileFormatException(const std::string& customError) noexcept {
	message = "ID3::FileFormatException: " + (customError.empty() ? "file format error!\n" :
	                                                                 customError + '\n');
}

///@pkg ID3Exception.hpp
FileFormatException::~FileFormatException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////  S I Z E E X C E P T I O N //////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
SizeException::SizeException() noexcept {
	message = "ID3::SizeException: size error.\n";
}

///@pkg ID3Exception.hpp
SizeException::SizeException(const std::exception& e) noexcept : Exception(e) {}

///@pkg ID3Exception.hpp
SizeException::SizeException(const std::string& customError) noexcept {
	message = "ID3::SizeException: " + (customError.empty() ? "size error.\n" :
	                                                          customError + '\n');
}

///@pkg ID3Exception.hpp
SizeException::~SizeException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////  T A G S I Z E E X C E P T I O N //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
TagSizeException::TagSizeException() noexcept {
	message = "ID3::TagSizeException: tag exceeds maximum size of "+std::to_string(MAX_TAG_SIZE)+"!\n";
}

///@pkg ID3Exception.hpp
TagSizeException::TagSizeException(const std::exception& e) noexcept : SizeException(e) {}

///@pkg ID3Exception.hpp
TagSizeException::TagSizeException(const std::string& customError) noexcept {
	message = "ID3::TagSizeException: " + (customError.empty() ? "tag exceeds maximum size of "+std::to_string(MAX_TAG_SIZE)+"!\n" :
	                                                             customError + '\n');
}

///@pkg ID3Exception.hpp
TagSizeException::~TagSizeException() noexcept {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  F R A M E S I Z E E X C E P T I O N /////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Exception.hpp
FrameSizeException::FrameSizeException() noexcept {
	message = "ID3::FrameSizeException: frame exceeds maximum size of "+std::to_string(MAX_TAG_SIZE)+"!\n";
}

///@pkg ID3Exception.hpp
FrameSizeException::FrameSizeException(const std::exception& e) noexcept : SizeException(e) {}

///@pkg ID3Exception.hpp
FrameSizeException::FrameSizeException(const std::string& customError) noexcept {
	message = "ID3::FrameSizeException: " + (customError.empty() ? "frame exceeds maximum size of "+std::to_string(MAX_TAG_SIZE)+"!\n" :
	                                                               customError + '\n');
}

///@pkg ID3Exception.hpp
FrameSizeException::FrameSizeException(const std::string& frameID, const std::string& frameDescription) noexcept {
	message = "ID3::FrameSizeException: frame " + frameID + " (" + frameDescription +
	          ") exceeds maximum size of " + std::to_string(MAX_TAG_SIZE) + "!\n";
}

///@pkg ID3Exception.hpp
FrameSizeException::~FrameSizeException() noexcept {}
