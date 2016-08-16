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
	if(frameBytes.size() > HEADER_BYTE_SIZE + 1) {
		uint8_t encodingChar;
		FrameEncoding encoding(FrameEncoding::LATIN1);
	
		//Get the frame's encoding. Default to LATIN-1.
		encodingChar = frameBytes[HEADER_BYTE_SIZE];
		switch((int)encodingChar) {
			case FrameEncoding::UTF16BOM: { encoding = FrameEncoding::UTF16BOM;
				                            break; }
			case FrameEncoding::UTF16BE: { encoding = FrameEncoding::UTF16BE;
				                           break; }
			case FrameEncoding::UTF8: { encoding = FrameEncoding::UTF8;
				                        break; }
			case FrameEncoding::LATIN1: default: break;
		}
	
		if(encoding == FrameEncoding::UTF16BOM ||
		   encoding == FrameEncoding::UTF16BE) {
			//Read only the text content
			textContent = utf16toutf8(frameBytes, HEADER_BYTE_SIZE+1);
		} else if(encoding == FrameEncoding::LATIN1) {
			//Read only the text content
			textContent = latin1toutf8(frameBytes, HEADER_BYTE_SIZE+1);
		} else {
			//Read only the text content
			textContent = std::string(frameBytes.begin()+HEADER_BYTE_SIZE+1,
			                          frameBytes.end());
		}
	} else {
		textContent = "";
	}
	
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
