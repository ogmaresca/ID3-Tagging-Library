/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 **********************************************************************/

#include <cstring>          //For ::strlen()
#include <unicode/unistr.h> //For icu::UnicodeString

#include "ID3.h"
#include "ID3Functions.h"

using namespace ID3;

///@pkg ID3Functions.h
std::string ID3::getGenreString(int genre) {
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
std::string ID3::utf16toutf8(const std::vector<char>& u16s) {
	const int u16sSize = u16s.size();
	
	//UTF-16 uses 2-byte character widths. If there's 0 bytes then
	//it's an empty string anyways, and if there's 1 byte then it's
	//not valid UTF-16 so an empty string should be returned.
	if(u16sSize < 2) return "";
	
	//The byte position to start processing the UTF-16 string
	int startingPos = 0;
	
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
	if((short)u16s[0] == -1 && (short)u16s[1] == -2) { //Has LE BOM
		startingPos = 2; //Don't want to include the BOM in the
						 //returned string
		
		//Since this is in the case of little endian-ness, the byte with
		//the most significant values are in the second byte.
		for(int i = 0; i < u16sSize; i += 2)
			utf16CharArr[i/2] = ((short)u16s[i + startingPos + 1] << 8) + (short)u16s[i + startingPos];
	} else { //May or may not have BE BOM
		//Checks if the first character is 0xFEFF. If it is, then
		//increment the starting position by two to remove the BOM
		//from the returned string
		if((short)u16s[0] == -2 && (short)u16s[1] == -1)
			startingPos = 2;
		
		for(int i = 0; i < u16sSize; i += 2)
			utf16CharArr[i/2] = ((short)u16s[i + startingPos] << 8) + (short)u16s[i + startingPos + 1];
	}
	
	//Create a UnicodeString from the UTF-16 character array
	//If there was a BOM, the string will be one character shorter
	//than the char vector.
	icu::UnicodeString icuStr(utf16CharArr, (u16sSize - startingPos) / 2);
	
	//Have the UnicodeString converted to UTF-8 and store the result
	//in toReturn.
	icuStr.toUTF8String(toReturn);
	
	//Prevent memory leaks.
	delete utf16CharArr;
	
	return toReturn;
}

///@pkg ID3Functions.h
std::string ID3::getFrameName(Frames frameID) {
	//A vector that has a 1:1 correspondence with the Frames enum.
	static std::vector<std::string> frames = {
		"TALB", //ALBUM
		"TPE1", //ARTIST
		"TPE2", //BAND
		"TBPM", //BPM
		"COMM", //COMMENT
		"TCOM", //COMPOSER
		"TCOP", //COPYRIGHT
		"TPOS", //DISC
		"TCON", //GENRE
		"TEXT", //LYRICIST
		"TIT2", //TITLE
		"TRCK", //TRACK
		"TYET"  //YEAR
	};
	
	if((unsigned int)frameID > frames.size())
		return "";
	return frames[(unsigned int)frameID];
}
