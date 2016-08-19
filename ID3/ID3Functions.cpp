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

#include <cstring>          //For ::strlen()
#include <unicode/unistr.h> //For icu::UnicodeString

#include "ID3.h"
#include "ID3Functions.h"

using namespace ID3;

///@pkg ID3Functions.h
std::string ID3::V1::getGenreString(int genre) {
	if(genre >= 0 && genre < V1::GENRES.size())
		return V1::GENRES[genre];
	return "";
}

///@pkg ID3Functions.h
long ID3::char_arr_binary_num(char array[], int size, bool synchsafe) {
	if(array == nullptr || size < 1) return 0;
	
	short shiftSize = synchsafe ? 7 : 8;
	long value = (int)array[0];
	
	for(int i = 1; i < size; i++) {
		value = (value << shiftSize) + (int)array[i];
	}
	return value;
}

///@pkg ID3Functions.h
long ID3::uchar_arr_binary_num(unsigned char array[], int size, bool synchsafe) {
	if(array == nullptr || size < 1) return 0;
	
	short shiftSize = synchsafe ? 7 : 8;
	long value = (int)array[0];
	
	for(int i = 1; i < size; i++) {
		value = (value << shiftSize) + (int)array[i];
	}
	return value;
}

///@pkg ID3Functions.h
std::string ID3::terminatedstring(const char* str, std::string::size_type maxlength) {
	std::string::size_type nullcharpos = ::strlen(str);
	return std::string(str, nullcharpos < maxlength ? nullcharpos : maxlength);
}

///@pkg ID3Functions.h
std::string ID3::utf16toutf8(const ByteArray& u16s,
                             long start,
                             long end) {	
	//Set the start
	if(start < 0)
		start = 0;
	
	//Set the end
	if(end < 0 || end > u16s.size())
		end = u16s.size();
	
	//UTF-16 uses 2-byte character widths. If there's 0 bytes then
	//it's an empty string anyways, and if there's 1 byte then it's
	//not valid UTF-16 so an empty string should be returned.
	if(end - start < 2)
		return "";
	
	const int u16sSize = end - start;
	
	//The byte offset to start processing the UTF-16 string
	int offset = 0;
	
	//Create an array of 2-byte characters that is encoded in UTF-16
	uint16_t* utf16CharArr = new uint16_t[u16sSize / 2];
	
	//The string to return
	std::string toReturn;
	
	//If it has the BOM, it checks the first character in the string.
	//If it's 0xFFFE then it uses little endian, and the bytes for each
	//character are flipped around.
	//If it's 0xFEFF, then it uses big endian.
	//If there's no BOM, then it's assumed the string uses big endian.
	//ICU does not seem to processes the BOM or little endian encoding,
	//so this has to be manually done.
	//This is automatically checked and processed in utf16toutf8(),
	//no need to mess with the BOM beforehand.
	if((uint8_t)u16s[start] == 0xFF && (uint8_t)u16s[start+1] == 0xFE) { //Has Little Endian BOM
		offset = 2; //Don't want to include the BOM in the returned string
		
		//Since this is in the case of little endian-ness, the byte with
		//the most significant values are in the second byte.
		for(int i = 0; i < u16sSize; i += 2)
			utf16CharArr[i/2] = ((uint16_t)u16s[start + offset + i + 1] << 8) + (uint16_t)u16s[start + offset + i];
	} else { //May or may not have BOM, is Big Endian
		//Checks if the first character is 0xFEFF. If it is, then increment the
		//starting position by two to remove the BOM from the returned string
		if((uint8_t)u16s[start] == 0xFE && (uint8_t)u16s[start+1] == 0xFF)
			offset = 2;
		
		for(int i = 0; i < u16sSize; i += 2)
			utf16CharArr[i/2] = ((uint16_t)u16s[start + offset + i] << 8) + (uint16_t)u16s[start + i + offset + 1];
	}
	
	//Create a UnicodeString from the UTF-16 character array
	//If there was a BOM, the string will be one character shorter
	//than the char vector.
	icu::UnicodeString icuStr(utf16CharArr, (u16sSize - offset) / 2);
	
	//Have the UnicodeString converted to UTF-8 and store the result
	//in toReturn.
	icuStr.toUTF8String(toReturn);
	
	//Prevent memory leaks.
	delete utf16CharArr;
	
	return toReturn;
}

///@pkg ID3Functions.h
std::string ID3::latin1toutf8(const ByteArray& latin1s, long start, long end) {
	//0x80 (128) is the first character beyond ASCII
	static const uint8_t BEYOND_ASCII = 0x80;
	
	//In UTF-8, if the first byte starts with "110" then it will be a two byte character
	static const uint8_t UTF8_TWO_BYTE_MASK = 0b11000000;
	
	//In UTF-8, bytes of characters beyond the first byte don't have the first
	//two bits usable, since these bytes will always be 0b10XXXXXX
	static const uint8_t VARIABLE_UTF8_CHAR_USABLE_BITS = 6;
	
	//A mask to apply on the LATIN-1 character to store its value on the second byte
	static const uint8_t UTF8_BYTE_TWO_MASK = 0b00111111;
	
	//Set the start
	if(start < 0)
		start = 0;
	
	//Set the end 
	if(end < 0 || end > latin1s.size())
		end = latin1s.size();
	
	//Empty string base case
	if(end <= start)
		return "";
	
	const int latin1sSize = end - start;
	
	int curPos = 0;
		
	//Create a char array to hold the translated UTF-8 string.
	//In the worst case utf8CharArr's byte size will have to be twice the LATIN-1
	//string's size, which will happen if the LATIN-1 string contains no ASCII
	//characters.
	char* utf8CharArr = new char[latin1sSize * 2];
	
	for(size_t i = start; i < end; i++) {
		uint8_t curChar = latin1s[i];
		
		if(curChar < BEYOND_ASCII) {
			//If curChar <= 127, it is an ASCII character that is identical in UTF-8
			utf8CharArr[curPos] = curChar;
			curPos++;
		} else {
			//Translate the LATIN-1 character to a UTF-8 character
			utf8CharArr[curPos] = UTF8_TWO_BYTE_MASK | (curChar >> VARIABLE_UTF8_CHAR_USABLE_BITS);
			utf8CharArr[curPos+1] = BEYOND_ASCII | (curChar & UTF8_BYTE_TWO_MASK);
			curPos += 2;
		}
	}
	
	//Create a string from the array
	std::string toReturn(utf8CharArr, curPos);
	
	//Prevent memory leaks
	delete utf8CharArr;
	
	//Return the string
	return toReturn;
}

///@pkg ID3Functions.h
std::string ID3::getFrameName(const Frames frameID) {
	//A vector that has a 1:1 correspondence with the Frames enum.
	static std::vector<std::string> frames = {
		"TALB", //0  - ALBUM
		"TPE2", //2  - ALBUMARTIST
		"TPE1", //1  - ARTIST
		"TBPM", //3  - BPM
		"COMM", //4  - COMMENT
		"TCOM", //5  - COMPOSER
		"TCOP", //6  - COPYRIGHT
		"TPOS", //7  - DISC
		"TCON", //8  - GENRE
		"TEXT", //9  - LYRICIST
		"TPE4", //10 - MODIFIEDBY
		"TIT2", //11 - TITLE
		"TRCK", //12 - TRACK
		"TYER", //13 - YEAR
		"TPE3", //14 - CONDUCTOR
		"TDAT", //15 - DATE
		"TENC", //16 - ENCODEDBY
		"TDLY", //17 - PLAYLISTDELAY
		"TFLT", //18 - FILETYPE
		"TIME", //19 - TIME
		"TIT1", //20 - GROUPING
		"TIT3", //21 - DESCRIPTION
		"TKEY", //22 - MUSICALKEY
		"TLEN", //23 - LENGTH
		"TMED", //24 - MEDIATYPE
		"TOAL", //25 - ORIGINALALBUM
		"TOPE", //26 - ORIGINALARTIST
		"TOFN", //27 - ORIGINALFILENAME
		"TOLY", //28 - ORIGINALLYRICIST
		"TORY", //29 - ORIGINALYEAR
		"TOWN", //30 - FILEOWNER
		"TRSN", //31 - RADIOSTATION
		"TRSO", //32 - RADIOSTATIONOWNER
		"TDRA", //33 - RECORDINGDATES
		"TSSE", //34 - ENCODINGSETTINGS
		"TSRC", //35 - ISRC
		"TSIZ", //36 - SIZE
		"TXXX", //37 - USERINFO
		"IPLS", //38 - INVOLVEDPEOPLE
	};
	
	if((unsigned int)frameID > frames.size())
		return "";
	return frames[(unsigned int)frameID];
}

//bool ID3::allowsMultipleFrames(const Frames frameID) {
//		return true if the ID3v2 spec allows multiple of the same frame type, false otherwise
//}
