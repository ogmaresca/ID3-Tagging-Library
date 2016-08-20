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

#include <iostream> //For printing

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
                          isNull(true),
                          isEdited(false),
                          isFromFile(false) {}

///@pkg ID3Frame.h
Frame::Frame(const std::string& frameName,
             const unsigned short version,
             ByteArray& frameBytes) : id(frameName),
                                      ID3Ver(version),
                                      frameContent(frameBytes),
                                      isNull(frameBytes.size() <= HEADER_BYTE_SIZE),
                                      isEdited(false),
                                      isFromFile(true) {
	if(!isNull && (compressed() || encrypted() || unsynchronized()))
		isNull = true;
}

///@pkg ID3Frame.h
Frame::~Frame() {}

///@pkg ID3Frame.h
bool Frame::operator==(const FrameClass classID) const noexcept { return type() == classID; }

///@pkg ID3Frame.h
Frame::operator FrameClass() const noexcept { return type(); }

///@pkg ID3Frame.h
bool Frame::operator==(bool boolean) const noexcept { return boolean == isNull; }

///@pkg ID3Frame.h
Frame::operator ByteArray() const noexcept { return frameContent; }

///@pkg ID3Frame.h
bool Frame::null() const { return isNull; }

///@pkg ID3Frame.h
unsigned long Frame::size() const { return frameContent.size(); }

///@pkg ID3Frame.h
std::string Frame::frame() const { return id; }

///@pkg ID3Frame.h
void Frame::revert() { read(); isEdited = false; }

///@pkg ID3Frame.h
bool Frame::edited() const { return isEdited; }

///@pkg ID3Frame.h
bool Frame::createdFromFile() const { return isFromFile; }

///@pkg ID3Frame.h
bool Frame::discardUponTagAlterIfUnknown() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN    == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN) :
	       (frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4 == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4);
}

///@pkg ID3Frame.h
bool Frame::discardUponAudioAlter() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER    == FLAG1_DISCARD_UPON_AUDIO_ALTER) :
	       (frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER_V4 == FLAG1_DISCARD_UPON_AUDIO_ALTER_V4);
}

///@pkg ID3Frame.h
bool Frame::readOnly() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG1_READ_ONLY    == FLAG1_READ_ONLY) :
	       (frameContent[8] & FLAG1_READ_ONLY_V4 == FLAG1_READ_ONLY_V4);
}

///@pkg ID3Frame.h
bool Frame::compressed() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG2_COMPRESSED    == FLAG2_COMPRESSED) :
	       (frameContent[8] & FLAG2_COMPRESSED_V4 == FLAG2_COMPRESSED_V4);
}

///@pkg ID3Frame.h
bool Frame::encrypted() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG2_ENCRYPTED    == FLAG2_ENCRYPTED) :
	       (frameContent[8] & FLAG2_ENCRYPTED_V4 == FLAG2_ENCRYPTED_V4);
}

///@pkg ID3Frame.h
bool Frame::groupingIdentity() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       (frameContent[8] & FLAG2_GROUPING_IDENTITY    == FLAG2_GROUPING_IDENTITY) :
	       (frameContent[8] & FLAG2_GROUPING_IDENTITY_V4 == FLAG2_GROUPING_IDENTITY_V4);
}

///@pkg ID3Frame.h
bool Frame::unsynchronized() const {
	if(ID3Ver < 4 || frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return frameContent[9] & FLAG2_UNSYNCHRONIZED_V4 == FLAG2_UNSYNCHRONIZED_V4;
}

///@pkg ID3Frame.h
bool Frame::dataLengthIndicator() const {
	if(ID3Ver < 4 || frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return frameContent[9] & FLAG2_DATA_LENGTH_INDICATOR_V4 == FLAG2_DATA_LENGTH_INDICATOR_V4;
}

///@pkg ID3Frame.h
uint8_t Frame::groupIdentity() const {
	if(frameContent.size() < headerSize() || !groupingIdentity())
		return 0;
	return ID3Ver <= 3 ? frameContent[headerSize()] : frameContent[HEADER_BYTE_SIZE + 1];
}

///@pkg ID3Frame.h
short Frame::headerSize() const {
	return HEADER_BYTE_SIZE +
	       (compressed() ? 4 : 0) +
	       (encrypted() ? 1 : 0) +
	       (groupingIdentity() ? 1 : 0) +
	       (dataLengthIndicator() ? 4 : 0);
}

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
                           ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                 version,
                                                                 frameBytes) {
	//Pictures are huge, don't want to print that
	if(!isNull) {
		if(id == "APIC")
			std::cout << "Content for UnknownFrame " << id << ": <frame content too large to print>" << std::endl;
		else
			std::cout << "Content for UnknownFrame " << id << ": " << std::string(frameBytes.begin()+HEADER_BYTE_SIZE, frameBytes.end()) << std::endl;
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
FrameClass UnknownFrame::type() const noexcept {
	return FrameClass::CLASS_UNKNOWN;
}

///@pkg ID3Frame.h
bool UnknownFrame::empty() const {
	return frameContent.size() <= HEADER_BYTE_SIZE;
}

///@pkg ID3Frame.h
ByteArray UnknownFrame::write(unsigned short version, bool minimize) {
	if(discardUponTagAlterIfUnknown()) {
		frameContent = ByteArray();
	} else if(frameContent.size() >= HEADER_BYTE_SIZE &&
	          version != ID3Ver && version >= MIN_SUPPORTED_VERSION &&
	          version <= MAX_SUPPORTED_VERSION) {
		//Whether a frame size is synchsafe has been changed in different ID3
		//versions, so it must be updated to report the correct frame size
		ByteArray frameSize = intToByteArray(frameContent.size() - HEADER_BYTE_SIZE, 4, ID3Ver >= 4);
		for(short i = 0; i < 4; i++)
			frameContent[i+4] = frameSize[i];
	}
	return frameContent;
}

///@pkg ID3Frame.h
void UnknownFrame::read() {}
