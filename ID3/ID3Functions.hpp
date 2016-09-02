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

#ifndef ID3_FUNCTIONS_HPP
#define ID3_FUNCTIONS_HPP

#include <string> //For std::string
#include <vector> //For std::vector

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
	 * @see ID3.h
	 */
	typedef std::vector<uint8_t> ByteArray;
	
	/**
	 * ID3v1 Functions.
	 */
	namespace V1 {
		/**
		 * A function to get the genre of a song, from an ID3v1 genre int.
		 * @param genre The integer ID for the ID3v1 genre.
		 * @returns The genre if the genre was found, and a blank string otherwise.
		 */
		std::string getGenreString(ushort genre);
	}
	
	/**
	 * Receives a char array, and calculates the multi-byte unsigned integer that
	 * the char array encodes.
	 * 
	 * NOTE: The size of the array is not checked, segmentation faults will
	 *       not be prevented if given a length longer the length of the array.
	 * 
	 * @param array The char array.
	 * @param length The length of the char array.
	 * @param synchsafe If the char array contains a "synchsafe" number,
	 *        where the first bit of each byte is always zeroed.
	 * @return The summed value of the char array's bits.
	 */
	unsigned long long byteIntVal(uint8_t* array, int length, bool synchsafe=false);
	
	/**
	 * Given an unsigned integer value, receive a ByteArray that encodes the
	 * integer value as its bit value.
	 * 
	 * @param val The integer value to encode.
	 * @param length The length of the ByteArray to return. If the given value
	 *        does not fit in the length of the ByteArray, then the maximum
	 *        possible value that fits in the bytes is supported. The maximum
	 *        value is ((1 << (length * x)) - 1), where x is 7 if synchsafe is
	 *        true and 8 if it is false.
	 * @param synchsafe If the ByteArray should contain a "synchsafe" number,
	 *        where the first bit of each byte is always zeroed.
	 * @return The integer value encoded as a ByteArray.
	 */
	ByteArray intToByteArray(unsigned long long val, ushort length=0, bool synchsafe=false);
	
	/**
	 * Create a std::string object with the call std::string(const char* s, size_type n).
	 * The difference is that the string will be trimmed to the position
	 * of the first null character if n is beyong that position.
	 * Useful for C strings that are not null-terminated.
	 * 
	 * @param str The C string char array.
	 * @param maxlength The maximum length of the string.
	 * @return The resulting string.
	 */
	std::string terminatedstring(const char* str, std::string::size_type maxlength);
	
	/**
	 * utf16toutf8() takes a char vector of a string encoded in UTF-16 (with
	 * or without the BOM) created from ID3::Frame::getTextFrame(), and
	 * returns the string encoded in UTF-8.
	 * 
	 * @param u16s The char vector that contains a UTF-16 encoded string.
	 * @param start The byte position in the ByteArray to start reading (optional).
	 *              If given a negative value, it will default to 0.
	 *              If not given, it will default to 0.
	 * @param end The byte position + 1 in the ByteArray to end reading (optional).
	 *            If not given or given a negative value, it will default to the
	 *            end of the ByteArray. If it is longer than the length of the
	 *            ByteArray, the function will stop at the end of the ByteArray.
	 * @return The UTF-8 encoded string.
	 */
	std::string utf16toutf8(const ByteArray& u16s, long start=-1, long end=-1);
	
	/**
	 * utf16toutf8() takes a char vector of a string encoded in LATIN-1 created
	 * from ID3::Frame::getTextFrame(), and returns the string encoded in UTF-8.
	 * 
	 * @param u16s The char vector that contains a LATIN-1 encoded string.
	 * @param start The byte position in the ByteArray to start reading (optional).
	 *              If given a negative value, it will default to 0.
	 *              If not given, it will default to 0.
	 * @param end The byte position + 1 in the ByteArray to end reading (optional).
	 *            If not given or given a negative value, it will default to the
	 *            end of the ByteArray. If it is longer than the length of the
	 *            ByteArray, the function will stop at the end of the ByteArray.
	 * @return The UTF-8 encoded string.
	 */
	std::string latin1toutf8(const ByteArray& ulatin1s, long start=-1, long end=-1);
	
	/**
	 * Get a ByteArray encoded in either LATIN-1, UTF-8, or UTF-16, and return
	 * the string as a UTF-8 encoded string.
	 * This function uses utf16toutf8() to encode UTF-16 strings to UTF-8, and
	 * ID3::latin1toutf8() to encode LATIN-1 strings to UTF-8.
	 * 
	 * @param encoding A char whose int values are represented by the enum
	 *                 ID3::FrameEncoding. If the encoding is unknown it will
	 *                 default to LATIN-1.
	 * @param bytes The char vector that contains the string you wish to encode.
	 * @param start The byte position in the ByteArray to start reading
	 *              (optional). If not given a value or given a negative value,
	 *              it will default to 0.
	 * @param end The byte position + 1 in the ByteArray to end reading
	 *            (optional). If not given or given a negative value, it will
	 *            default to the end of the ByteArray. If it is longer than the
	 *            length of the ByteArray, the function will stop at the end of
	 *            the ByteArray.
	 * @return The UTF-8 encoded string.
	 */
	std::string getUTF8String(uint8_t encoding,
	                          const ByteArray& bytes,
	                          long start=-1,
	                          long end=-1);
}

#endif
