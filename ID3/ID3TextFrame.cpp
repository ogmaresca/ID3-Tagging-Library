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
TextFrame::TextFrame(const FrameID&   frameName,
                     const ushort     version,
                     const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                 version,
                                                                 frameBytes) {
	//If the frame content isn't null, then get the text content
	if(!isNull) read();
}

///@pkg ID3TextFrame.h
TextFrame::TextFrame(const FrameID&    frameName,
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
ByteArray TextFrame::write() {
	//TODO: Trim strings that are too long
	
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	//Convert the separating character if necessary
	convertSeparator();
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is empty, then don't write anything to file
	if(empty()) {
		frameContent = ByteArray();
	} else if(isEdited || isFromFile || OLD_VERSION == 3) {
		//Create a ByteArray that fits the header, encoding, and text content size
		const ulong NEW_FRAME_SIZE = HEADER_BYTE_SIZE + 1 + textContent.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, ID3Ver >= 4);
		for(ushort i = 0; i < 4 && i < size.size(); i++)
			frameContent[i+4] = size[i];
		
		//Set the encoding to UTF-8
		frameContent[HEADER_BYTE_SIZE] = FrameEncoding::ENCODING_UTF8;
		
		//Write the text string to file
		for(ulong i = 0; i < textContent.size() && i + HEADER_BYTE_SIZE + 1 < NEW_FRAME_SIZE; i++)
			frameContent[i + HEADER_BYTE_SIZE + 1] = textContent[i];
	}
	
	isEdited = false;
	
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

void TextFrame::convertSeparator() {
	///@pkg ID3TextFrame.h
	
	const char OLD_SEPARATOR = stringSeparator();
	
	if(OLD_SEPARATOR != '\0') {
		//Loop through the text and convert every slash to a null character
		for(char& curChar : textContent) {
			if(curChar == OLD_SEPARATOR)
				curChar = '\0';
		}
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
                                       const ushort       version,
                                       const std::string& value) : TextFrame::TextFrame(frameName,
                                                                                        version,
                                                                                        value) {
	if(!std::all_of(textContent.begin(), textContent.end(), ::isdigit))
		textContent = "";
}

///@pkg ID3TextFrame.h
NumericalTextFrame::NumericalTextFrame(const FrameID& frameName,
                                       const ushort   version,
                                       const long     value) : TextFrame::TextFrame(frameName,
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
                                           const ushort version,
                                           const std::string& value,
                                           const std::string& description,
                                           const std::string& language,
                                           const ushort options) : TextFrame::TextFrame(frameName,
                                                                                        version,
                                                                                        value),
                                                                   optionLanguage((options & OPTION_LANGUAGE) == OPTION_LANGUAGE),
                                                                   optionLatin1((options & OPTION_LATIN1_TEXT)==OPTION_LATIN1_TEXT),
                                                                   optionNoDescription((options & OPTION_NO_DESCRIPTION)==OPTION_NO_DESCRIPTION) {
	textDescription = description;
	//Have ID3::DescriptiveTextFrame::language(std::string&) check if a language
	//can be set, but if it can then isEdited must be set back to false.
	this->language(language);
	isEdited = false;
}

///@pkg ID3TextFrame.h
DescriptiveTextFrame::DescriptiveTextFrame() noexcept : TextFrame::TextFrame(),
                                                        optionLanguage(false),
                                                        optionLatin1(false),
                                                        optionNoDescription(false) {}


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
ByteArray DescriptiveTextFrame::write() {
	//TODO: If the LATIN-1 Text option is set, don't just automatically assume
	//      that the text content string is in ASCII.
	//TODO: Trim strings that are too long
	
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	//Convert the separating character if necessary
	convertSeparator();
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is empty, then don't write anything to file
	if(empty()) {
		frameContent = ByteArray();
	} else if(isEdited || isFromFile || OLD_VERSION == 3) {
		//The description starts after the header, encoding, and language (if it
		//has one)
		const ulong DESCRIPTION_START = HEADER_BYTE_SIZE + 1 + (optionLanguage ? LANGUAGE_SIZE : 0);
		
		//The text starts after where the description starts, plus the length of
		//the description in bytes and a null character if there is a description
		const ulong TEXT_START = DESCRIPTION_START + (optionNoDescription ? 0 : textDescription.size() + 1);
		
		//Create a ByteArray that fits the header, encoding, language, description
		//size, and text content size
		const ulong NEW_FRAME_SIZE = TEXT_START + textContent.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];		
		
		//Set the encoding to UTF-8
		frameContent[HEADER_BYTE_SIZE] = FrameEncoding::ENCODING_UTF8;
		
		//Write the language to file
		if(optionLanguage) {
			if(textLanguage.size() != LANGUAGE_SIZE) textLanguage = "xxx";
			
			for(ushort i = 0; i < LANGUAGE_SIZE; i++)
				frameContent[HEADER_BYTE_SIZE + 1 + i] = textLanguage[i];
		}
		
		//Write the description to file. Since frameContent was initialized with
		//null characters, the null separator is already in the array.
		if(!optionNoDescription) {
			for(ulong i = DESCRIPTION_START; i < textDescription.size(); i++)
				frameContent[DESCRIPTION_START + i] = textDescription[i];
		}
		
		//Write the text string to file
		for(ulong i = 0; i < textContent.size() && TEXT_START + i < NEW_FRAME_SIZE; i++)
			frameContent[TEXT_START + 1] = textContent[i];
	}
	
	isEdited = false;
	
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
	if(!readOnly() && optionLanguage &&
	   (newLanguage == "" || newLanguage.size() == LANGUAGE_SIZE)) {
		textLanguage = newLanguage;
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
ByteArray URLTextFrame::write() {
	//TODO: Trim strings that are too long
	//TODO: Actually encode the text content as LATIN-1, instead of just assuming
	//it is in ASCII.
	
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is empty, then don't write anything to file
	if(empty()) {
		frameContent = ByteArray();
	} else if(isEdited || isFromFile || OLD_VERSION == 3) {
		//Create a ByteArray that fits the header and text content size
		const ulong NEW_FRAME_SIZE = HEADER_BYTE_SIZE + textContent.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, ID3Ver >= 4);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];
		
		//Write the text string to file
		for(ulong i = 0; i < textContent.size() && i + HEADER_BYTE_SIZE < NEW_FRAME_SIZE; i++)
			frameContent[i + HEADER_BYTE_SIZE] = textContent[i];
	}
	
	isEdited = false;
	
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
