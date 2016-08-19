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

#include "ID3.h"
#include "ID3Frame.h"
#include "ID3Functions.h"

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  T E X T F R A M E //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
TextFrame::TextFrame(const std::string& frameName,
                     const unsigned short version,
                     ByteArray& frameBytes,
                     const unsigned long end) : Frame::Frame(frameName,
                                                             version,
                                                             frameBytes,
                                                             end) {
	//If the frame content is only its header or not even that, then it is null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
}

///@pkg ID3Frame.h
TextFrame::TextFrame(const std::string& frameName,
                     const unsigned short version,
                     const std::string& value) : Frame::Frame() {
	id = frameName;
	ID3Ver = version;
	textContent = value;
	isNull = false;
}

///@pkg ID3Frame.h
TextFrame::TextFrame() noexcept : Frame::Frame() {}

///@pkg ID3Frame.h
bool TextFrame::empty() const {
	return textContent == "";
}

///@pkg ID3Frame.h
ByteArray TextFrame::write(unsigned short version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
}

///@pkg ID3Frame.h
void TextFrame::read(ByteArray& frameBytes) {
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameBytes.size() > HEADER_BYTE_SIZE + 1)
		textContent = readStringAsUTF8(frameBytes[HEADER_BYTE_SIZE], //Get the encoding byte
		                               frameBytes,
		                               HEADER_BYTE_SIZE+1);
	else
		textContent = "";
	
	//std::cout << "Content for TextFrame " << id << ": " << textContent << std::endl;
	
	frameContent = frameBytes;
}

///@pkg ID3Frame.h
std::string TextFrame::content() const { return textContent; }

///@pkg ID3Frame.h
void TextFrame::content(const std::string& newContent) {
	textContent = newContent;
	isEdited = true;
}

///@pkg ID3Frame.h
///@static
std::string TextFrame::readStringAsUTF8(char encoding,
                                        ByteArray bytes,
                                        long start,
                                        long end) {
	//Set the start
	if(start < 0)
		start = 0;
	
	//Set the end 
	if(end < 0 || end > bytes.size())
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

///@pkg ID3Frame.h
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

///@pkg ID3Frame.h
bool TextFrame::operator==(const FrameClass classID) const noexcept {
	return classID == FrameClass::CLASS_TEXT;
}

///@pkg ID3Frame.h
TextFrame::operator FrameClass() const noexcept {
	return FrameClass::CLASS_TEXT;
}

///@pkg ID3Frame.h
bool TextFrame::operator==(const std::string& str) const noexcept {
	return textContent == str;
}

///@pkg ID3Frame.h
TextFrame::operator std::string() const noexcept {
	return textContent;
}

///@pkg ID3Frame.h
TextFrame& TextFrame::operator+=(const std::string& str) noexcept {
	textContent += str;
	isEdited = true;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  N U M E R I C A L T E X T F R A M E /////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const unsigned short version,
                                       ByteArray& frameBytes,
                                       const unsigned long end) : Frame::Frame(frameName,
                                                                               version,
                                                                               frameBytes,
                                                                               end) {
	//If the frame content is only its header or not even that, then it is null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
	
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit)) {
		//std::cout << "Frame " << id << ": Content \"" << textContent << "\" is not numerical!" << std::endl;
		textContent = "";
	}
}

///@pkg ID3Frame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const unsigned short version,
                                       const std::string& value) : TextFrame::TextFrame(frameName,
                                                                                        version,
                                                                                        value) {
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit)) {
		//std::cout << "Frame " << id << ": Content \"" << textContent << "\" is not numerical!" << std::endl;
		textContent = "";
	}
}

///@pkg ID3Frame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const unsigned short version,
                                       const long value) : TextFrame::TextFrame(frameName,
                                                                                version,
                                                                                std::to_string(value)) {}

///@pkg ID3Frame.h
NumericalTextFrame::NumericalTextFrame() noexcept : TextFrame::TextFrame() {}

///@pkg ID3Frame.h
void NumericalTextFrame::content(const std::string& newContent) {
	TextFrame::content(newContent);
	if(std::all_of(newContent.begin(), newContent.end(), ::isdigit))
		textContent = newContent;
	else
		textContent = "";
	isEdited = true;
}

///@pkg ID3Frame.h
void NumericalTextFrame::content(long newContent) {
	TextFrame::content(std::to_string(newContent));
}

///@pkg ID3Frame.h
void NumericalTextFrame::read(ByteArray& frameBytes) {
	TextFrame::read(frameBytes);
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit))
		textContent = "";
}

///@pkg ID3Frame.h
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

///@pkg ID3Frame.h
bool NumericalTextFrame::operator==(const FrameClass classID) const noexcept {
	return classID == FrameClass::CLASS_NUMERICAL;
}

///@pkg ID3Frame.h
NumericalTextFrame::operator FrameClass() const noexcept {
	return FrameClass::CLASS_NUMERICAL;
}

///@pkg ID3Frame.h
bool NumericalTextFrame::operator==(long val) const noexcept {
	return textContent == std::to_string(val);
}

///@pkg ID3Frame.h
NumericalTextFrame::operator long() const noexcept {
	return atoi(textContent.c_str());
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator+=(long val) noexcept {
	textContent += std::to_string(atoi(textContent.c_str()) + val);
	isEdited = true;
	return *this;
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator-=(long val) noexcept {
	textContent += std::to_string(atoi(textContent.c_str()) - val);
	isEdited = true;
	return *this;
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator*=(long val) noexcept {
	textContent += std::to_string(atoi(textContent.c_str()) * val);
	isEdited = true;
	return *this;
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator/=(long val) noexcept {
	textContent += std::to_string(atoi(textContent.c_str()) / val);
	isEdited = true;
	return *this;
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator%=(long val) noexcept {
	textContent += std::to_string(atoi(textContent.c_str()) % val);
	isEdited = true;
	return *this;
}

///@pkg ID3Frame.h
NumericalTextFrame& NumericalTextFrame::operator+=(const std::string& str) noexcept {
	if(std::all_of(str.begin(), str.end(), ::isdigit)) {
		textContent += str;
		isEdited = true;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////  D E S C R I P T I V E T E X T F R A M E ///////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
DescriptiveTextFrame::DescriptiveTextFrame(const std::string& frameName,
                                           const unsigned short version,
                                           ByteArray& frameBytes,
                                           const unsigned long end,
                                           const short options) : Frame::Frame(frameName,
                                                                               version,
                                                                               frameBytes,
                                                                               end),
                                                                  frameOptions(options) {
	//If the frame content is only its header or not even that, then it is null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
}

///@pkg ID3Frame.h
DescriptiveTextFrame::DescriptiveTextFrame(const std::string& frameName,
                                           const unsigned short version,
                                           const std::string& value,
                                           const std::string& description,
                                           const std::string& language,
                                           const short options) : TextFrame::TextFrame(frameName,
                                                                                       version,
                                                                                       value),
                                                                  frameOptions(options) {
	textDescription = description;
	//Have ID3::DescriptiveTextFrame::language(std::string&) check if a language
	//can be set, but if it can then isEdited must be set back to false.
	this->language(language);
	isEdited = false;
}

///@pkg ID3Frame.h
DescriptiveTextFrame::DescriptiveTextFrame() noexcept : TextFrame::TextFrame(),
                                                        frameOptions(0) {}

///@pkg ID3Frame.h
ByteArray DescriptiveTextFrame::write(unsigned short version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
}

///@pkg ID3Frame.h
void DescriptiveTextFrame::content(const std::string& newContent,
                                   const std::string& newDescription) {
	TextFrame::content(newContent);
	textDescription = newDescription;
}

///@pkg ID3Frame.h
std::string DescriptiveTextFrame::description() const { return textDescription; }

///@pkg ID3Frame.h
void DescriptiveTextFrame::description(const std::string& newDescription) {
	textDescription = newDescription;
	isEdited = false;
}

///@pkg ID3Frame.h
std::string DescriptiveTextFrame::language() const { return textLanguage; }

///@pkg ID3Frame.h
void DescriptiveTextFrame::language(const std::string& newLanguage) {
	if((frameOptions & OPTION_LANGUAGE) == OPTION_LANGUAGE && (newLanguage.size() == 0 ||
	                                                           newLanguage.size() == 3)) {
		textLanguage = newLanguage;
		isEdited = true;
	}
}

///@pkg ID3Frame.h
void DescriptiveTextFrame::read(ByteArray& frameBytes) {
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameBytes.size() <= HEADER_BYTE_SIZE + 1) {
		textContent = "";
	} else {		
		long descriptionStart = HEADER_BYTE_SIZE + 1,//The start of the description
		     descriptionEnd = 0,                     //The end of the description
		     descriptionGap = 1;                     //The number of bytes in the
		                                             //gap between the description
		                                             //and the text content
		//The encoding
		char encoding = frameBytes[HEADER_BYTE_SIZE];
		//If the encoding uses 16-byte or 8-byte characters
		bool wideChars = encoding == ENCODING_UTF16BOM || encoding == ENCODING_UTF16;
		//If wide characters are used, then the gap will be 2 bytes long
		if(wideChars) descriptionGap++;
		//If the frame has a language set, then save it and increment the
		//description start by three bytes
		if((frameOptions & OPTION_LANGUAGE) == OPTION_LANGUAGE) {
			textLanguage = std::string(frameBytes.begin() + descriptionStart,
			                           frameBytes.begin() + descriptionStart + 3);
			descriptionStart += 3;
		} else { //Else remove any language
			textLanguage = "";
		}
		//Find the description end
		for(long i = descriptionStart; i + descriptionGap < frameBytes.size(); i+= descriptionGap) {
			if(frameBytes[i] == '\0') {
				if(wideChars && frameBytes[i+1] != '\0')
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
			textDescription = readStringAsUTF8(encoding,
		                                      frameBytes,
		                                      descriptionStart,
		                                      descriptionEnd);
		}
		//Save the text content, taking care of the LATIN1_TEXT option
		textContent = readStringAsUTF8((frameOptions & OPTION_LATIN1_TEXT)==OPTION_LATIN1_TEXT ?
			                            ENCODING_LATIN1 : encoding,
		                               frameBytes,
		                               descriptionEnd + descriptionGap);
	}
	
	//std::cout << "Content for DescriptiveTextFrame " << id << " " << textDescription << " (" << textLanguage << "): " << textContent << std::endl;
	
	frameContent = frameBytes;
}

///@pkg ID3Frame.h
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

///@pkg ID3Frame.h
bool DescriptiveTextFrame::operator==(const FrameClass classID) const noexcept {
	return classID == FrameClass::CLASS_DESCRIPTIVE;
}

///@pkg ID3Frame.h
DescriptiveTextFrame::operator FrameClass() const noexcept {
	return FrameClass::CLASS_DESCRIPTIVE;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  U R L T E X T F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
URLTextFrame::URLTextFrame(const std::string& frameName,
                           const unsigned short version,
                           ByteArray& frameBytes,
                           const unsigned long end) : Frame::Frame(frameName,
                                                                   version,
                                                                   frameBytes,
                                                                   end) {
	//If the frame content is only its header or not even that, then it is null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
}

///@pkg ID3Frame.h
URLTextFrame::URLTextFrame(const std::string& frameName,
                           const unsigned short version,
                           const std::string& value) : TextFrame::TextFrame() {}

///@pkg ID3Frame.h
URLTextFrame::URLTextFrame() noexcept : TextFrame::TextFrame() {}

///@pkg ID3Frame.h
ByteArray URLTextFrame::write(unsigned short version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
}

///@pkg ID3Frame.h
void URLTextFrame::read(ByteArray& frameBytes) {
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameBytes.size() > HEADER_BYTE_SIZE + 1)
		textContent = readStringAsUTF8(ENCODING_LATIN1, //URL frames are in LATIN-1, no encoding byte
		                               frameBytes,
		                               HEADER_BYTE_SIZE);
	else
		textContent = "";
	
	//std::cout << "Content for URLTextFrame " << id << ": " << textContent << std::endl;
	
	frameContent = frameBytes;
}

///@pkg ID3Frame.h
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

///@pkg ID3Frame.h
bool URLTextFrame::operator==(const FrameClass classID) const noexcept {
	return classID == FrameClass::CLASS_URL;
}

///@pkg ID3Frame.h
URLTextFrame::operator FrameClass() const noexcept {
	return FrameClass::CLASS_URL;
}
