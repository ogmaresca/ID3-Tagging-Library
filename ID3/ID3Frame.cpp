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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  F R A M E //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
Frame::Frame() noexcept : ID3Ver(MAX_SUPPORTED_VERSION),
                          endPosition(0),
                          isNull(true),
                          isEdited(false),
                          isFromFile(false) {}

///@pkg ID3Frame.h
Frame::Frame(const std::string& frameName,
		     const unsigned short version,
		     ByteArray& frameBytes,
		     const unsigned long end) : id(frameName),
		                                ID3Ver(version),
		                                frameContent(frameBytes),
		                                endPosition(end),
		                                isNull(true),
		                                isEdited(false),
		                                isFromFile(true) {}

///@pkg ID3Frame.h
Frame::~Frame() {}

///@pkg ID3Frame.h
bool Frame::operator==(bool boolean) const noexcept { return boolean == isNull; }

///@pkg ID3Frame.h
Frame::operator ByteArray() const noexcept { return frameContent; }

///@pkg ID3Frame.h
bool Frame::null() const { return isNull; }

///@pkg ID3Frame.h
unsigned long Frame::end() const { return endPosition; }

///@pkg ID3Frame.h
unsigned long Frame::size() const { return frameContent.size(); }

///@pkg ID3Frame.h
std::string Frame::frame() const { return id; }

///@pkg ID3Frame.h
void Frame::revert() { read(frameContent); }

///@pkg ID3Frame.h
bool Frame::edited() const { return isEdited; }

///@pkg ID3Frame.h
bool Frame::createdFromFile() const { return isFromFile; }

///@pkg ID3Frame.h
ByteArray Frame::bytes() const noexcept { return frameContent; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  U N K N O W N F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame(const std::string& frameName,
                           const unsigned short version,
			               ByteArray& frameBytes,
			               const unsigned long end) : Frame::Frame(frameName,
			                                                       version,
			                                                       frameBytes,
			                                                       end) {
	//If the frame is only its header, or not even that, then it's
	//invalid and thus null
	if(frameContent.size() > HEADER_BYTE_SIZE) {
		read(frameBytes);
		isNull = false;
	}
}

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame(const std::string& frameName) : Frame::Frame() {
	id = frameName;
}

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame() noexcept : Frame::Frame() {}

///@pkg ID3Frame.h
bool UnknownFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a UnknownFrame, and if it is compare the content
	const UnknownFrame* const castFrame = dynamic_cast<const UnknownFrame* const>(frame);
	//If it's not a UnknownFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : frameContent == castFrame->bytes();
}

///@pkg ID3Frame.h
bool UnknownFrame::operator==(const FrameClass classID) const noexcept {
	return classID == FrameClass::UNKNOWN;
}

///@pkg ID3Frame.h
ByteArray UnknownFrame::write(unsigned short version, bool minimize) {
	return frameContent;
}

///@pkg ID3Frame.h
UnknownFrame::operator FrameClass() const noexcept {
	return FrameClass::UNKNOWN;
}

///@pkg ID3Frame.h
void UnknownFrame::read(ByteArray& frameBytes) {
	frameContent = frameBytes;
	
	//Pictures are huge, don't want to print that
	//if(id != "APIC")
	//	std::cout << "Content for UnknownFrame " << id << ": " << std::string(frameBytes.begin()+HEADER_BYTE_SIZE, frameBytes.end()) << std::endl;
}
