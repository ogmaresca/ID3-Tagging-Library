/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 **********************************************************************/

#include <iostream>

#include "ID3.h"
#include "ID3Frame.h"
#include "ID3Functions.h"

using namespace ID3;

///@pkg ID3Frame.h
Frame::Frame(std::ifstream& file,
             const unsigned long readpos,
             const unsigned int version,
             const unsigned long filesize) : Frame::Frame() {
	startpos = readpos;
	
	FrameHeader header;
	
	file.seekg(readpos, std::ifstream::beg);
	if(file.fail()) return;
	
	file.read(reinterpret_cast<char *>(&header), HEADER_BYTE_SIZE);
	frameSize = uchar_arr_binary_num(header.size, 4, version >= 4);
	endpos = startpos + frameSize + HEADER_BYTE_SIZE;
	
	id = terminatedstring(header.id, 4);
	
	if(frameSize == 0 || endpos > filesize) return;
	
	isNull = false;
	
	if(isTextFrame())
		getTextFrame(file);
}

///@pkg ID3Frame.h
Frame::Frame(const std::string& frameName,
             const std::string& text) : Frame::Frame() {
	id = frameName;
	if(isTextFrame()) {
		textContent = text;
		textContentOnFile = textContent;
		frameSize = textContent.size();
		isNull = false;
	}
}

///@pkg ID3Frame.h
Frame::Frame() : startpos(0),
                 endpos(0),
                 frameSize(0),
                 isNull(true) {}

///@pkg ID3Frame.h
bool Frame::null() const { return isNull; }

///@pkg ID3Frame.h
unsigned long Frame::start() const { return startpos; }

///@pkg ID3Frame.h
unsigned long Frame::end() const { return endpos; }

///@pkg ID3Frame.h
unsigned long Frame::size() const { return frameSize; }

///@pkg ID3Frame.h
std::string Frame::frame() const { return id; }

///@pkg ID3Frame.h
bool Frame::isTextFrame() const {
	return id[0] == 'T' || id == "COMM";
}

///@pkg ID3Frame.h
std::string Frame::text() const { return textContent; }

///@pkg ID3Frame.h
void Frame::text(std::string newValue) {
	if(isTextFrame())
		textContent = newValue;
}

///@pkg ID3Frame.h
void Frame::revert() {
	if(isTextFrame())
		textContent = textContentOnFile;
}

///@pkg ID3Frame.h
void Frame::save() {
	if(isTextFrame())
		textContentOnFile = textContent;
}

///@pkg ID3Frame.h
bool Frame::edited() const {
	if(isTextFrame())
		return textContent == textContentOnFile;
	
	return false;
}

///@pkg ID3Frame.h
bool Frame::createdFromFile() const {
	return startpos > 0 && endpos > 0 && frameSize > 0;
}

///@pkg ID3Frame.h
void Frame::getTextFrame(std::ifstream& file) {
	uint8_t encodingChar;
	FrameEncoding encoding(FrameEncoding::LATIN1);
	
	//Get the frame's encoding. Default to LATIN-1.
	//NOTE: LATIN-1 is not currently supported beyond ASCII characters.
	file.seekg(startpos + HEADER_BYTE_SIZE, std::ifstream::beg);
	file.read(reinterpret_cast<char *>(&encodingChar), 1);
	switch((int)encodingChar) {
		case FrameEncoding::UTF16BOM: { encoding = FrameEncoding::UTF16BOM; break; }
		case FrameEncoding::UTF16BE: { encoding = FrameEncoding::UTF16BE; break; }
		case FrameEncoding::UTF8: { encoding = FrameEncoding::UTF8; break; }
		case FrameEncoding::LATIN1: default: { break; }
	}
	
	std::vector<char> text(frameSize - 1);
	file.seekg(startpos + HEADER_BYTE_SIZE + 1, std::ifstream::beg);
	file.read(&text.front(), frameSize - 1);
	
	if(encoding == FrameEncoding::UTF16BOM || encoding == FrameEncoding::UTF16BE) {
		textContent = utf16toutf8(text);
	} else { //TODO: Differentiate between UTF-8 and LATIN-1.
		textContent = std::string(text.begin(), text.end());
	}
	
	std::cout << "Content for frame " << id << ": " << textContent << std::endl;
	
	textContentOnFile = textContent;
}
