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
#include <algorithm>        //For std::reverse()

#include "ID3Functions.h"
#include "ID3Constants.h"

using namespace ID3;

//Private namespace
namespace {
	/**
	 * A vector that has a 1:1 correspondence with the Frames enum.
	 */
	static const std::vector<std::string> frames = {
		"AENC", //0
		"APIC", //1
		"ASPI", //2
		"COMM", //3
		"COMR", //4
		"ENCR", //5
		"EQU2", //6
		"EQUA", //7
		"ETCO", //8
		"GEOB", //9
		"GRID", //10
		"IPLS", //11
		"LINK", //12
		"MCDI", //13
		"MLLT", //14
		"OWNE", //15
		"PCNT", //16
		"POPM", //17
		"POSS", //18
		"PRIV", //19
		"RBUF", //20
		"RVA2", //21
		"RVAD", //22
		"RVRB", //23
		"SEEK", //24
		"SIGN", //25
		"SYLT", //26
		"SYTC", //27
		"TALB", //28
		"TBPM", //29
		"TCOM", //30
		"TCON", //31
		"TCOP", //32
		"TDAT", //33
		"TDEN", //34
		"TDLY", //35
		"TDOR", //36
		"TDRC", //37
		"TDRL", //38
		"TDTG", //39
		"TENC", //40
		"TEXT", //41
		"TFLT", //42
		"TIPL", //43
		"TIME", //44
		"TIT1", //45
		"TIT2", //46
		"TIT3", //47
		"TKEY", //48
		"TLAN", //49
		"TLEN", //50
		"TMCL", //51
		"TMED", //52
		"TMOO", //53
		"TOAL", //54
		"TOFL", //55
		"TOLY", //56
		"TOPE", //57
		"TORY", //58
		"TOWN", //59
		"TPE1", //60
		"TPE2", //61
		"TPE3", //62
		"TPE4", //63
		"TPOS", //64
		"TPRO", //65
		"TPUB", //66
		"TRCK", //67
		"TRDA", //68
		"TRSN", //69
		"TRSO", //70
		"TSO2", //71
		"TSOA", //72
		"TSOC", //73
		"TSOP", //74
		"TSOT", //75
		"TSIZ", //76
		"TSRC", //77
		"TSSE", //78
		"TSST", //79
		"TXXX", //80
		"TYER", //81
		"UFID", //82
		"USER", //83
		"USLT", //84
		"WCOM", //85
		"WCOP", //86
		"WOAF", //87
		"WOAR", //88
		"WOAS", //89
		"WORS", //90
		"WPAY", //91
		"WPUB", //92
		"WXXX", //93
		"XXXX", //94 - Unknown ID3v2.2 frame ID after being converted to ID3v2.4
	};
}

///@pkg ID3Functions.h
std::string ID3::V1::getGenreString(ushort genre) {
	if(genre < V1::GENRES.size())
		return V1::GENRES[genre];
	return "";
}

///@pkg ID3Functions.h
ulong ID3::byteIntVal(uint8_t* array, int size, bool synchsafe) {
	if(array == nullptr || size < 1) return 0;
	
	const short shiftSize = synchsafe ? 7 : 8;
	
	ulong value = *array++;
	
	for(int i = 1; i < size; i++)
		value = (value << shiftSize) + *array++;
	
	return value;
}

///@pkg ID3Functions.h
ByteArray ID3::intToByteArray(ulong val, ushort length, bool synchsafe) {
	const ushort shiftSize = synchsafe ? 7 : 8;
	const ushort modVal = synchsafe ? 0x80 : 0x100;
	
	//No length given
	if(length == 0) {
		ByteArray byteVector;
		while(val > 0) {
			byteVector.push_back(val % modVal);
			val >>= shiftSize;
		}
		//Reverse the vector, as it's currently little-endian
		std::reverse(byteVector.begin(), byteVector.end());
		return byteVector;
	} else {
		ByteArray byteVector(length);
		//If val is too big to fit in the given size, then make it the maximum
		//possible value that will fit
		if(val > (1UL << static_cast<ulong>(length * shiftSize)) - 1UL)
			val = (1UL << static_cast<ulong>(length * shiftSize)) - 1UL;
		for(int i = length - 1; i >= 0; i--) {
			byteVector[i] = val % modVal;
			val >>= shiftSize;
		}
		return byteVector;
	}
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
	if(end < 0 || static_cast<ulong>(end) > u16s.size())
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
	//If it's 0xFFFE then it uses little endian, and the bytes for each character
	//are flipped around. If it's 0xFEFF, then it uses big endian.
	//If there's no BOM, then it's assumed the string uses big endian.
	//ICU does not seem to processes the BOM or little endian encoding, so it has
	//to be manually done. This is automatically checked and processed in
	//utf16toutf8(), no need to mess with the BOM beforehand.
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
	if(end < 0 || static_cast<ulong>(end) > latin1s.size())
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
	
	for(long i = start; i < end; i++) {
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
std::string ID3::getUTF8String(uint8_t encoding,
                               const ByteArray& bytes,
                               long start,
                               long end) {
	//Set the start
	if(start < 0)
		start = 0;
	
	//Set the end 
	if(end < 0 || static_cast<ulong>(end) > bytes.size())
		end = bytes.size();
	
	//Empty string base case
	if(end <= start)
		return "";
	
	switch(encoding) {
		//UTF-16 case
		case FrameEncoding::ENCODING_UTF16BOM:
		case FrameEncoding::ENCODING_UTF16: return utf16toutf8(bytes, start, end);
		//UTF-8 case
		case FrameEncoding::ENCODING_UTF8: return std::string(bytes.begin()+start,
		                                                      bytes.begin()+end);
		//LATIN-1 case
		case FrameEncoding::ENCODING_LATIN1: default: return latin1toutf8(bytes,
		                                                                  start,
		                                                                  end);
	}
}

///@pkg ID3Functions.h
std::string ID3::getFrameName(const Frames frameID) {
	if(static_cast<ushort>(frameID) >= frames.size())
		return frames[frames.size()];
	return frames[static_cast<ushort>(frameID)];
}

///@pkg ID3Functions.h
Frames ID3::getFrameName(const std::string& frameID) {
	//A map that holds Frame ID string : Frame ID enum pairs
	//Initialize the map with a lambda function that goes through the frames
	//vector in the private namespace and adds a pair to the map of the frame ID
	//string and the integer location in the vector, casted to a Frames enum value.
	//Since a function's static variables are initialized upon the function's
	//first call, there's no need to worry about conversionMap getting
	//initialized after frames.
	static std::unordered_map<std::string, Frames> conversionMap = []() {
		std::unordered_map<std::string, Frames> conversionMap;
		conversionMap.reserve(frames.size());
		
		for(ushort i = 0; i < frames.size(); i++)
			conversionMap.emplace(frames[i], static_cast<Frames>(i));
		return conversionMap;
	}();
	
	return conversionMap.count(frameID) > 0 ?
	       //If the frame ID is found, then return the frame ID enum value
	       conversionMap[frameID] :
	       //If it is not found return the XXXX ID
	       Frames::FRAME_UNKNOWN_FRAME;
}

///@pkg ID3Functions.h
bool ID3::allowsMultipleFrames(const Frames frameID) {
	switch(frameID) {
		case FRAME_AUDIO_ENCRYPTION:
		case FRAME_ATTACHED_PICTURE:
		case FRAME_COMMENT:
		case FRAME_COMMERCIAL:
		case FRAME_ENCRYPTION_METHOD_REGISTRATION:
		case FRAME_EQUALIZATION_2:
		case FRAME_GENERAL_ENCAPSULATED_OBJECT:
		case FRAME_GROUP_IDENTIFICATION_REGISTRATION:
		case FRAME_LINKED_INFORMATION:
		case FRAME_POPULARIMETER:
		case FRAME_PRIVATE:
		case FRAME_RELATIVE_VOLUME_ADJUSTMENT_2:
		case FRAME_SIGNATURE:
		case FRAME_SYNCHRONIZED_LYRICS:
		case FRAME_CUSTOM_USER_INFORMATION:
		case FRAME_UNIQUE_FILE_IDENTIFIER:
		case FRAME_TERMS_OF_USE:
		case FRAME_UNSYNCHRONIZED_LYRICS:
		case FRAME_COMMERCIAL_INFORMATION_URL:
		case FRAME_OFFICIAL_ARTIST_URL:
		case FRAME_USER_DEFINED_URL:
			return true;
		default:
			return false;
	}
}

///@pkg ID3Functions.h
bool ID3::allowsMultipleFrames(const std::string& frameID) {
	return allowsMultipleFrames(getFrameName(frameID));
}

///@pkg ID3Functions.h
std::string ID3::convertOldFrameIDToNew(const std::string& v2FrameID) {
	//An unordered_map that holds string pairs. The key is the ID3v2.2 frame ID,
	//and the value is its ID3v2.4 equivalent. Note that for date frames the
	//ID3v2.3 frame IDs are used, as if not since these frames don't support
	//multiple instances of the frame on file the second and beyond date frames
	//would not get saved.
	static std::unordered_map<std::string, std::string> conversionMap = {
		{"BUF", "RBUF"},
		{"COM", "COMM"},
		{"CNT", "PCNT"},
		{"CRA", "AENC"},
		{"ETC", "ETCO"},
		{"EQU", "EQUA"},
		{"GEO", "GEOB"},
		{"IPL", "TIPL"},
		{"LNK", "LINK"},
		{"MLL", "MLLT"},
		{"PIC", "APIC"},
		{"POP", "POPM"},
		{"RVA", "RVAD"},
		{"REV", "RVRB"},
		{"STC", "SYTC"},
		{"SLT", "USLT"},
		{"TT1", "TIT1"},
		{"TT2", "TIT2"},
		{"TT3", "TIT3"},
		{"TP1", "TPE1"},
		{"TP2", "TPE2"},
		{"TP3", "TPE3"},
		{"TP4", "TPE4"},
		{"TCM", "TCOM"},
		{"TXT", "TOLY"},
		{"TLA", "TLAN"},
		{"TCO", "TCON"},
		{"TAL", "TALB"},
		{"TPA", "TPOS"},
		{"TRK", "TRCK"},
		{"TRC", "TSRC"},
		{"TYE", "TYER"},
		{"TDA", "TDAT"},
		{"TIM", "TIME"},
		{"TRD", "TRDA"},
		{"TMT", "TMED"},
		{"TBP", "TBPM"},
		{"TEN", "TENC"},
		{"TSS", "TSSE"},
		{"TOF", "TOFN"},
		{"TLE", "TLEN"},
		//TSIZ is completely deprecated in ID3v2.4, so don't check the TSI ID
		{"TDY", "TDLY"},
		{"TKE", "TKEY"},
		{"TOT", "TOAL"},
		{"TOA", "TOPE"},
		{"TOL", "TOLY"},
		{"TOR", "TDOR"},
		{"TXX", "TXXX"},
		{"ULT", "USLT"},
		{"WAF", "WOAF"},
		{"WAR", "WOAR"},
		{"WCM", "WCOM"},
		{"WCP", "WCOP"},
		{"WPB", "WPUB"},
		{"WXX", "WXXX"}
	};
	
	return conversionMap.count(v2FrameID) > 0 ?
	       //If the frame ID is found, then return the converted frame ID
	       conversionMap[v2FrameID] :
	       //If it is not found return the XXXX ID
	       getFrameName(Frames::FRAME_UNKNOWN_V2_2_FRAME);
}
