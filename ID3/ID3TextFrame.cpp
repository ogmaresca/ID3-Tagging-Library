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

//#include <iostream> //For printing

#include "ID3.h"
#include "ID3Frame.h"
#include "ID3Functions.h"

using namespace ID3;

///@pkg ID3Frame.h
TextFrame::TextFrame(const std::string& frameName,
                     const unsigned short version,
					 ByteArray& frameBytes,
			         const unsigned long end) : Frame::Frame(frameName,
			                                                 version,
			                                                 frameBytes,
			                                                 end) {
	//If the frame content is only its header or not even that, then
	//it is null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
}

///@pkg ID3Frame.h
TextFrame::TextFrame(const std::string& frameName,
                     const unsigned short version,
			         const std::string& textContent,
			         const std::string& description) : Frame::Frame() {
	id = frameName;
	ID3Ver = version;
	this->textContent = textContent;
	isNull = false;
}

///@pkg ID3Frame.h
TextFrame::TextFrame() noexcept : Frame::Frame() {}

ByteArray TextFrame::write(unsigned short version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
}

///@pkg ID3Frame.h
void TextFrame::read(ByteArray& frameBytes) {
	//Make sure that there is enough room for text before reading the
	//frame bytes
	if(frameBytes.size() > HEADER_BYTE_SIZE + 1)
		textContent = readStringAsUTF8(frameBytes[HEADER_BYTE_SIZE],
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
void TextFrame::content(std::string newContent) {
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
	
	switch((int)encoding) {
		//UTF-16 case
		case FrameEncoding::UTF16BOM:
		case FrameEncoding::UTF16BE: return utf16toutf8(bytes, start, end);
		//UTF-8 case
		case FrameEncoding::UTF8: return std::string(bytes.begin()+start,
		                                             bytes.begin()+end);
		//LATIN-1 case
		case FrameEncoding::LATIN1: default: return latin1toutf8(bytes,
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
	return classID == FrameClass::TEXT;
}

///@pkg ID3Frame.h
TextFrame::operator FrameClass() const noexcept {
	return FrameClass::TEXT;
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
TextFrame& TextFrame::operator+=(std::string str) noexcept {
	textContent += str;
	return *this;
}
