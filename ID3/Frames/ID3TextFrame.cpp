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

#include <iostream>  //For printing
#include <algorithm> //For std::all_of

#include "../ID3.h"
#include "ID3TextFrame.h"
#include "../ID3Functions.h"
#include "../ID3Constants.h"

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  T E X T F R A M E //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
TextFrame::TextFrame(const FrameID&   frameName,
                     const ushort     version,
                     const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                 version,
                                                                 frameBytes) {
	//If the frame content isn't null, then get the text content
	if(!isNull) read();
}

///@pkg ID3TextFrame.h
TextFrame::TextFrame(const FrameID&     frameName,
                     const std::string& value) noexcept : Frame::Frame(frameName) {
	textContent = value;
}

///@pkg ID3TextFrame.h
FrameClass TextFrame::type() const noexcept {
	return FrameClass::CLASS_TEXT;
}

///@pkg ID3TextFrame.h
bool TextFrame::empty() const {
	return textContent == "";
}

///@pkg ID3TextFrame.h
void TextFrame::print() const {
	Frame::print();
	std::cout << "Content:        " << textContent << std::endl;
	if(frameContent.size() > headerSize()) {
		std::cout << "Encoding:       ";
		switch(frameContent[headerSize()]) {
			case FrameEncoding::ENCODING_UTF16: {
				std::cout << "UTF-16"; break;
			} case FrameEncoding::ENCODING_UTF16BOM: {
				std::cout << "UTF-16 BOM"; break;
			} case FrameEncoding::ENCODING_UTF8: {
				std::cout << "UTF-8"; break;
			} case FrameEncoding::ENCODING_LATIN1: default: {
				std::cout << "LATIN-1"; break;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "Frame class:    TextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
ByteArray TextFrame::write() {
	const char OLD_SEPARATOR = stringSeparator();
	
	if(OLD_SEPARATOR != '\0') {
		//Loop through the text and convert every slash to a null character
		for(char& curChar : textContent)
			if(curChar == OLD_SEPARATOR) curChar = '\0';
	}
	
	//Write the content
	return Frame::write();
}

///@pkg ID3TextFrame.h
void TextFrame::writeBody() {
	//TODO: If the LATIN-1 Text option is set, don't just automatically assume
	//      that the text content string is in ASCII.
	//TODO: Trim strings that are too long
	
	//Reserve space in the ByteArray to fit the header, encoding, and text content
	frameContent.reserve(frameContent.size() + 1 + textContent.size());
	
	//Check if the text content is pure ASCII or if it has to be encoded in UTF-8
	bool isASCII = true;
	for(const char currentChar : textContent) {
		if(static_cast<uint8_t>(currentChar) >= 0x80) {
			isASCII = false;
			break;
		}
	}
	
	//Set the encoding to LATIN-1 if it's pure ASCII, else UTF-8
	frameContent.push_back(isASCII ? FrameEncoding::ENCODING_LATIN1 : FrameEncoding::ENCODING_UTF8);
	
	//Write the text content to file
	frameContent.insert(frameContent.end(), textContent.begin(), textContent.end());
}

///@pkg ID3TextFrame.h
void TextFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() > HEADER_SIZE) {
		textContent = getUTF8String(frameContent[HEADER_SIZE], //Get the encoding byte
		                            frameContent,
		                            HEADER_SIZE+1);
	} else {
		isNull = true;
		textContent = "";
	}
}

///@pkg ID3TextFrame.h
std::string TextFrame::content() const { return textContent; }

///@pkg ID3TextFrame.h
void TextFrame::content(const std::string& newContent) {
	if(!flag(FrameFlag::READ_ONLY)) {
		textContent = newContent;
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
std::vector<std::string> TextFrame::contents() const {
	//A vector that contains a single string, for cases where there is no text
	//content.
	static std::vector<std::string> emptyString(1, "");
	
	//If the string is empty, no use continuing
	if(textContent == "") return emptyString;
	
	//A vector of strings to return
	std::vector<std::string> tokens;
	
	//The separating character
	const char SEPARATOR = stringSeparator();
	
	//Loop variables
	size_t tokenStart = 0, tokenEnd = textContent.find(SEPARATOR, 0);
	
	while((tokenEnd = textContent.find(SEPARATOR, tokenStart)) != textContent.npos) {
		//If the substring isn't an empty string
		if(tokenEnd > tokenStart)
			tokens.push_back(textContent.substr(tokenStart, tokenEnd - tokenStart));
		tokenStart = tokenEnd + 1;
	}
	
	//Add the last string token, if it isn't an empty string
	if(tokenStart < textContent.size())
		tokens.push_back(textContent.substr(tokenStart));
	
	//In the edge case that the string contains only divider characters, then
	//also return the empty string vector
	if(tokens.size() == 0) return emptyString;
	
	return tokens;
}

///@pkg ID3TextFrame.h
void TextFrame::contents(const std::vector<std::string>& newContent) {
	if(!flag(FrameFlag::READ_ONLY)) {
		isEdited = true;
		textContent = "";
	
		if(newContent.size() > 0) {	
			//The separating character
			const char SEPARATOR = stringSeparator();
			//Concatenate the strings
			for(const std::string& currentStr : newContent) {
				if(textContent != "") textContent += SEPARATOR;
				textContent != currentStr;
			}
		}
	}
}

///@pkg ID3TextFrame.h
char TextFrame::stringSeparator() const {
	if(ID3Ver <= 3) {
		switch(frame()) {
			case FRAME_COMPOSER:
			case FRAME_LYRICIST:
			case FRAME_ORIGINAL_LYRICIST:
			case FRAME_ORIGINAL_ARTIST:
			case FRAME_ARTIST:
				return '/';
			default:
				break;
		}
	}
	
	return '\0';
}

///@pkg ID3TextFrame.h
bool TextFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a TextFrame, and if it is compare the content
	const TextFrame* const castFrame = dynamic_cast<const TextFrame* const>(frame);
	//If it's not a TextFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : textContent == castFrame->content();
}

///@pkg ID3TextFrame.h
bool TextFrame::operator==(const std::string& str) const noexcept {
	return textContent == str;
}

///@pkg ID3TextFrame.h
TextFrame::operator std::string() const noexcept {
	return textContent;
}

///@pkg ID3TextFrame.h
TextFrame& TextFrame::operator+=(const std::string& str) noexcept {
	textContent += stringSeparator() + str;
	isEdited = true;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  N U M E R I C A L T E X T F R A M E /////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const FrameID&   frameName,
                                       const ushort     version,
                                       const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                                   version,
                                                                                   frameBytes) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const FrameID&     frameName,
                                       const std::string& value) : TextFrame::TextFrame(frameName,
                                                                                        value) {
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit))
		textContent = "";
}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const FrameID& frameName,
                                       const long     value) : TextFrame::TextFrame(frameName,
                                                                                    std::to_string(value)) {}

///@pkg ID3TextFrame.h
FrameClass NumericalTextFrame::type() const noexcept {
	return FrameClass::CLASS_NUMERICAL;
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::content(const std::string& newContent) {
	if(!flag(FrameFlag::READ_ONLY)) {
		if(std::all_of(newContent.begin(), newContent.end(), ::isdigit))
			textContent = newContent;
		else
			textContent = "";
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::content(long long newContent) {
	TextFrame::content(std::to_string(newContent));
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::contents(const std::vector<std::string>& newContent) {
	if(!flag(FrameFlag::READ_ONLY)) {
		//Get a string vector of only valid numerical strings
		std::vector<std::string> validNumericalContent;
		validNumericalContent.reserve(newContent.size());
		
		//Go through newContent and only save integer strings
		for(const std::string& currentStr : newContent) {
			if(std::all_of(currentStr.begin(), currentStr.end(), ::isdigit))
				validNumericalContent.push_back(currentStr);
		}
		
		//Use TextFrame's contents(std::vector<std::string>&) method
		TextFrame::contents(validNumericalContent);
	}
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::contents(const std::vector<long long>& newContent) {
	if(!flag(FrameFlag::READ_ONLY)) {
		isEdited = true;
		textContent = "";
		
		if(newContent.size() > 0) {
			//The separating character
			const char SEPARATOR = stringSeparator();
			
			for(const long currentStr : newContent) {
				if(textContent != "") textContent += SEPARATOR;
				textContent != std::to_string(currentStr);
			}
		}
	}
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::print() const {
	Frame::print();
	std::cout << "Content:        " << textContent << std::endl;
	std::cout << "Frame class:    NumericalTextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::read() {
	//Read the saved ByteArray
	TextFrame::read();
	
	//Use the contents() method to get a string vector of the text value, and
	//then use contents(std::vector<std::string>&) to verify that every string
	//value is a numerical integer value
	contents(TextFrame::contents());
	isEdited = false;
}

///@pkg ID3TextFrame.h
bool NumericalTextFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a NumericalTextFrame, and if it is compare the content
	const NumericalTextFrame* const castFrame = dynamic_cast<const NumericalTextFrame* const>(frame);
	//If it's not a NumericalTextFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : textContent == castFrame->TextFrame::content();
}

///@pkg ID3TextFrame.h
bool NumericalTextFrame::operator==(long long val) const noexcept {
	return textContent == std::to_string(val);
}

///@pkg ID3TextFrame.h
NumericalTextFrame::operator long long() const noexcept {
	return atoll(textContent.c_str());
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator+=(long long val) noexcept {
	textContent += stringSeparator() + std::to_string(val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator+=(const std::string& str) noexcept {
	if(std::all_of(str.begin(), str.end(), ::isdigit)) {
		textContent += stringSeparator() + str;
		isEdited = true;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////  D E S C R I P T I V E T E X T F R A M E ///////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame(const FrameID& frameName,
                                           const ushort version,
                                           const ByteArray& frameBytes,
                                           const ushort options) : Frame::Frame(frameName,
                                                                                version,
                                                                                frameBytes),
                                                                   optionLanguage((options & OPTION_LANGUAGE) == OPTION_LANGUAGE),
                                                                   optionLatin1((options & OPTION_LATIN1_TEXT)==OPTION_LATIN1_TEXT),
                                                                   optionNoDescription((options & OPTION_NO_DESCRIPTION)==OPTION_NO_DESCRIPTION) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame(const FrameID& frameName,
                                           const std::string& value,
                                           const std::string& description,
                                           const std::string& language,
                                           const ushort options) : TextFrame::TextFrame(frameName,
                                                                                        value),
                                                                   optionLanguage((options & OPTION_LANGUAGE) == OPTION_LANGUAGE),
                                                                   optionLatin1((options & OPTION_LATIN1_TEXT)==OPTION_LATIN1_TEXT),
                                                                   optionNoDescription((options & OPTION_NO_DESCRIPTION)==OPTION_NO_DESCRIPTION) {
	textDescription = description;
	if(language == "" || language.size() == LANGUAGE_SIZE)
		textLanguage = language;
}

///@pkg ID3TextFrame.h
FrameClass DescriptiveTextFrame::type() const noexcept {
	return FrameClass::CLASS_DESCRIPTIVE;
}

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::print() const {
	Frame::print();
	if(textLanguage != "")
		std::cout << "Language:       " << textLanguage << std::endl;
	std::cout << "Description:    " << textDescription << std::endl;
	std::cout << "Content:        " << textContent << std::endl;
	std::cout << "Frame class:    DescriptiveTextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::writeBody() {
	//TODO: If the LATIN-1 Text option is set, don't just automatically assume
	//      that the text content string is in ASCII.
	//TODO: Trim strings that are too long
	
	//Reserve space in the ByteArray to fit the header, encoding, language,
	//description, and text content
	frameContent.reserve(frameContent.size() + 1 + (optionLanguage ? LANGUAGE_SIZE : 0)
	                     + (optionNoDescription ? 0 : textDescription.size() + 1)
	                     + textContent.size());
	
	//Set the encoding to UTF-8
	frameContent.push_back(FrameEncoding::ENCODING_UTF8);
	
	//Write the language to file
	if(optionLanguage) {
		if(textLanguage.size() != LANGUAGE_SIZE) textLanguage = "xxx";
		frameContent.insert(frameContent.end(), textLanguage.begin(), textLanguage.end());
	}
		
	//Write the description and its null separator to file.
	if(!optionNoDescription) {
		frameContent.insert(frameContent.end(), textDescription.begin(), textDescription.end());
		frameContent.push_back('\0');
	}
	
	//Write the text content to file
	frameContent.insert(frameContent.end(), textContent.begin(), textContent.end());
}

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::content(const std::string& newContent,
                                   const std::string& newDescription) {
	TextFrame::content(newContent);
	description(newDescription);
}

///@pkg ID3TextFrame.h
std::string DescriptiveTextFrame::description() const { return textDescription; }

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::description(const std::string& newDescription) {
	if(!flag(FrameFlag::READ_ONLY)) {
		textDescription = newDescription;
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
std::string DescriptiveTextFrame::language() const { return textLanguage; }

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::language(const std::string& newLanguage) {
	if(!flag(FrameFlag::READ_ONLY) && optionLanguage) {
		textLanguage = newLanguage.size() == LANGUAGE_SIZE ? newLanguage : "";
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	//Make sure that there is enough room for text and language (if set)
	//before reading the frame bytes
	if(frameContent.size() <= HEADER_SIZE + (optionLanguage ? 4U : 1U)) {
		textContent = "";
		textDescription = "";
		textLanguage = "";
		isNull = true;
	} else {		
		ulong descriptionStart = HEADER_SIZE + 1U,//The start of the description
		      descriptionEnd   = 0U,              //The end of the description
		      descriptionGap   = 1U;              //The number of bytes in the gap
		                                          //between the description and
		                                          //the text content
		//The encoding
		const uint8_t encoding = frameContent[HEADER_SIZE];
		//If the encoding uses 16-byte or 8-byte characters
		const bool wideChars = encoding == ENCODING_UTF16BOM || encoding == ENCODING_UTF16;
		//If wide characters are used, then the gap will be 2 bytes long
		if(wideChars) descriptionGap++;
		//If the frame has a language set, then save it and increment the
		//description start by three bytes
		if(optionLanguage) {
			textLanguage = std::string(frameContent.begin() + descriptionStart,
			                           frameContent.begin() + descriptionStart + 3);
			descriptionStart += 3;
		} else { //Else remove any language
			textLanguage = "";
		}
		//Find the description end
		for(ulong i = descriptionStart; i + descriptionGap <= frameContent.size(); i+= descriptionGap) {
			if(frameContent[i] == '\0') {
				if(wideChars && frameContent[i+1] != '\0')
					continue;
				descriptionEnd = i;
				break;
			}
		}
		//If no null characters have been found, then treat it as having no description
		if(descriptionEnd == 0) {
			descriptionEnd = descriptionStart;
			textDescription = "";
		} else { //Save the description
			textDescription = getUTF8String(encoding,
		                                   frameContent,
		                                   descriptionStart,
		                                   descriptionEnd);
		}
		//Save the text content, taking care of the LATIN1_TEXT option
		textContent = getUTF8String(optionLatin1 ? ENCODING_LATIN1 : encoding,
		                            frameContent,
		                            descriptionEnd + descriptionGap);
	}
}

///@pkg ID3TextFrame.h
bool DescriptiveTextFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a DescriptiveTextFrame, and if it is compare the content
	const DescriptiveTextFrame* const castFrame = dynamic_cast<const DescriptiveTextFrame* const>(frame);
	//If it's not a DescriptiveTextFrame return false
	if(castFrame == nullptr) return false;
	//If neither are null, compare the text contents, descriptions, and languages
	return isNull ? true : (textContent == castFrame->TextFrame::content() &&
	                        textDescription == castFrame->description() &&
	                        textLanguage == castFrame->language());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  U R L T E X T F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
URLTextFrame::URLTextFrame(const FrameID&   frameName,
                           const ushort     version,
                           const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                       version,
                                                                       frameBytes) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
URLTextFrame::URLTextFrame(const FrameID&     frameName,
                           const std::string& value) : TextFrame::TextFrame(frameName,
                                                                            value) {}

///@pkg ID3TextFrame.h
FrameClass URLTextFrame::type() const noexcept {
	return FrameClass::CLASS_URL;
}

///@pkg ID3TextFrame.h
void URLTextFrame::print() const {
	Frame::print();
	std::cout << "URL:            " << textContent << std::endl;
	std::cout << "Frame class:    URLTextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
void URLTextFrame::writeBody() {
	//TODO: Trim strings that are too long
	//TODO: Actually encode the text content as LATIN-1, instead of just assuming
	//it is in ASCII.
	
	//Write the text content to file
	frameContent.insert(frameContent.end(), textContent.begin(), textContent.end());
}

///@pkg ID3TextFrame.h
void URLTextFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() - 1 > HEADER_SIZE)
		textContent = getUTF8String(ENCODING_LATIN1, //URL frames are in LATIN-1, no encoding byte
		                            frameContent,
		                            HEADER_SIZE);
	else
		textContent = "";
}

///@pkg ID3TextFrame.h
bool URLTextFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a URLTextFrame, and if it is compare the content
	const URLTextFrame* const castFrame = dynamic_cast<const URLTextFrame* const>(frame);
	//If it's not a URLTextFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : textContent == castFrame->content();
}
