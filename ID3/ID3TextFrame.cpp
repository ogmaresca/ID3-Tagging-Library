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
#include "ID3TextFrame.h"
#include "ID3Functions.h"
#include "ID3Constants.h"

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  T E X T F R A M E //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
TextFrame::TextFrame(const std::string& frameName,
                     const ushort       version,
                     const ByteArray&   frameBytes) : Frame::Frame(frameName,
                                                                   version,
                                                                   frameBytes) {
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
TextFrame::TextFrame(const std::string& frameName,
                     const ushort       version,
                     const std::string& value) : Frame::Frame() {
	id = frameName;
	ID3Ver = version;
	textContent = value;
	isNull = false;
}

///@pkg ID3TextFrame.h
TextFrame::TextFrame() noexcept : Frame::Frame() {}


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
				std::cout << "UTF-16"; break;
			} case FrameEncoding::ENCODING_LATIN1: default: {
				std::cout << "LATIN-1"; break;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "Frame class:    TextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
ByteArray TextFrame::write(ushort version, bool minimize) {
	//TODO: Support ID3v2.3 text encodings
	//TODO: Trim strings that are too long
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	if(isEdited || frameContent.size() < headerSize()) {
		isEdited = false;
		
		//Create a ByteArray that fits the header, encoding, and size
		const ulong NEW_FRAME_SIZE = HEADER_BYTE_SIZE + 1 + textContent.size();
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, ID3Ver >= 4);
		
		//Save the frame size
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];
		
		//Set the encoding to UTF-8
		frameContent[HEADER_BYTE_SIZE] = FrameEncoding::ENCODING_UTF8;
		
		//Write the text string to file
		for(ulong i = 0; i < textContent.size() && i < NEW_FRAME_SIZE; i++)
			frameContent[i + HEADER_BYTE_SIZE + 1] = textContent[i];
	}
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
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
	if(!readOnly()) {
		textContent = newContent;
		isEdited = true;
	}
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
	textContent += str;
	isEdited = true;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  N U M E R I C A L T E X T F R A M E /////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const ushort version,
                                       const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                                   version,
                                                                                   frameBytes) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const ushort       version,
                                       const std::string& value) : TextFrame::TextFrame(frameName,
                                                                                        version,
                                                                                        value) {
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit))
		textContent = "";
}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const std::string& frameName,
                                       const ushort version,
                                       const long value) : TextFrame::TextFrame(frameName,
                                                                                version,
                                                                                std::to_string(value)) {}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame() noexcept : TextFrame::TextFrame() {}


///@pkg ID3TextFrame.h
FrameClass NumericalTextFrame::type() const noexcept {
	return FrameClass::CLASS_NUMERICAL;
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::content(const std::string& newContent) {
	if(!readOnly()) {
		if(std::all_of(newContent.begin(), newContent.end(), ::isdigit))
			textContent = newContent;
		else
			textContent = "";
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::content(long newContent) {
	TextFrame::content(std::to_string(newContent));
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::print() const {
	Frame::print();
	std::cout << "Content:        " << textContent << std::endl;
	std::cout << "Frame class:    NumericalTextFrame" << std::endl;
}

///@pkg ID3TextFrame.h
void NumericalTextFrame::read() {
	TextFrame::read();
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit))
		textContent = "";
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
bool NumericalTextFrame::operator==(long val) const noexcept {
	return textContent == std::to_string(val);
}

///@pkg ID3TextFrame.h
NumericalTextFrame::operator long() const noexcept {
	return atol(textContent.c_str());
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator+=(long val) noexcept {
	textContent += std::to_string(atol(textContent.c_str()) + val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator-=(long val) noexcept {
	textContent += std::to_string(atol(textContent.c_str()) - val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator*=(long val) noexcept {
	textContent += std::to_string(atol(textContent.c_str()) * val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator/=(long val) noexcept {
	textContent += std::to_string(atol(textContent.c_str()) / val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
NumericalTextFrame& NumericalTextFrame::operator%=(long val) noexcept {
	textContent += std::to_string(atol(textContent.c_str()) % val);
	isEdited = true;
	return *this;
}

///@pkg ID3TextFrame.h
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

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame(const std::string& frameName,
                                           const ushort version,
                                           const ByteArray& frameBytes,
                                           const short options) : Frame::Frame(frameName,
                                                                               version,
                                                                               frameBytes),
                                                                  frameOptions(options) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame(const std::string& frameName,
                                           const ushort version,
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

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame() noexcept : TextFrame::TextFrame(),
                                                        frameOptions(0) {}


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
ByteArray DescriptiveTextFrame::write(ushort version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
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
	if(!readOnly()) {
		textDescription = newDescription;
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
std::string DescriptiveTextFrame::language() const { return textLanguage; }

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::language(const std::string& newLanguage) {
	if(!readOnly() &&
	   (frameOptions & OPTION_LANGUAGE) == OPTION_LANGUAGE &&
	   (newLanguage.size() == 0 || newLanguage.size() == 3)) {
		textLanguage = newLanguage;
		isEdited = true;
	}
}

///@pkg ID3TextFrame.h
void DescriptiveTextFrame::read() {
	const bool hasLanguage = (frameOptions & OPTION_LANGUAGE) == OPTION_LANGUAGE;
	const ushort HEADER_SIZE = headerSize();
	
	//Make sure that there is enough room for text and language (if set)
	//before reading the frame bytes
	if(frameContent.size() <= HEADER_SIZE + (hasLanguage ? 4U : 1U)) {
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
		if((frameOptions & OPTION_LANGUAGE) == OPTION_LANGUAGE) {
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
		textContent = getUTF8String((frameOptions & OPTION_LATIN1_TEXT)==OPTION_LATIN1_TEXT ?
			                         ENCODING_LATIN1 : encoding,
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
URLTextFrame::URLTextFrame(const std::string& frameName,
                           const ushort version,
                           const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                       version,
                                                                       frameBytes) {
	//If the frame content is not null
	if(!isNull)
		read();
}

///@pkg ID3TextFrame.h
URLTextFrame::URLTextFrame(const std::string& frameName,
                           const ushort       version,
                           const std::string& value) : TextFrame::TextFrame() {}

///@pkg ID3TextFrame.h
URLTextFrame::URLTextFrame() noexcept : TextFrame::TextFrame() {}


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
ByteArray URLTextFrame::write(ushort version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
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
