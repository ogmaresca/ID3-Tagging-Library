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

#include "ID3.h"
#include "ID3Frame.h"
#include "ID3Functions.h"
#include "ID3Constants.h"

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
             const ushort version,
             const ByteArray& frameBytes) : id(frameName),
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
ulong Frame::size(bool header) const {
	const ulong FRAME_SIZE  = frameContent.size();
	const ulong HEADER_SIZE = headerSize();
	
	if(header)
		return FRAME_SIZE;
	
	return FRAME_SIZE < HEADER_SIZE ? 0 : FRAME_SIZE - HEADER_SIZE;
}

///@pkg ID3Frame.h
std::string Frame::frame() const { return id; }

///@pkg ID3Frame.h
ByteArray Frame::bytes(bool header) const noexcept {
	if(!header)
		return frameContent;
	const ushort HEADER_SIZE = headerSize();
	if(frameContent.size() < HEADER_SIZE)
		return ByteArray();
	return ByteArray(frameContent.begin() + HEADER_SIZE, frameContent.end());
}

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
	       ((frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN)    == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN) :
	       ((frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4) == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4);
}

///@pkg ID3Frame.h
bool Frame::discardUponAudioAlter() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       ((frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER)    == FLAG1_DISCARD_UPON_AUDIO_ALTER) :
	       ((frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER_V4) == FLAG1_DISCARD_UPON_AUDIO_ALTER_V4);
}

///@pkg ID3Frame.h
bool Frame::readOnly() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       ((frameContent[8] & FLAG1_READ_ONLY)    == FLAG1_READ_ONLY) :
	       ((frameContent[8] & FLAG1_READ_ONLY_V4) == FLAG1_READ_ONLY_V4);
}

///@pkg ID3Frame.h
bool Frame::compressed() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       ((frameContent[8] & FLAG2_COMPRESSED)    == FLAG2_COMPRESSED) :
	       ((frameContent[8] & FLAG2_COMPRESSED_V4) == FLAG2_COMPRESSED_V4);
}

///@pkg ID3Frame.h
bool Frame::encrypted() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       ((frameContent[8] & FLAG2_ENCRYPTED)    == FLAG2_ENCRYPTED) :
	       ((frameContent[8] & FLAG2_ENCRYPTED_V4) == FLAG2_ENCRYPTED_V4);
}

///@pkg ID3Frame.h
bool Frame::groupingIdentity() const {
	if(frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return ID3Ver <= 3 ?
	       ((frameContent[8] & FLAG2_GROUPING_IDENTITY)    == FLAG2_GROUPING_IDENTITY) :
	       ((frameContent[8] & FLAG2_GROUPING_IDENTITY_V4) == FLAG2_GROUPING_IDENTITY_V4);
}

///@pkg ID3Frame.h
bool Frame::unsynchronized() const {
	if(ID3Ver < 4 || frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return (frameContent[9] & FLAG2_UNSYNCHRONIZED_V4) == FLAG2_UNSYNCHRONIZED_V4;
}

///@pkg ID3Frame.h
bool Frame::dataLengthIndicator() const {
	if(ID3Ver < 4 || frameContent.size() < HEADER_BYTE_SIZE)
		return false;
	return (frameContent[9] & FLAG2_DATA_LENGTH_INDICATOR_V4) == FLAG2_DATA_LENGTH_INDICATOR_V4;
}

///@pkg ID3Frame.h
uint8_t Frame::groupIdentity() const {
	if(frameContent.size() < headerSize() || !groupingIdentity())
		return 0;
	//In ID3v2.3, the group identity is the last flag that adds bytes to the
	//header, so it's at the very end. In ID3v2.4 it's the first, so get the byte
	//right after the regular frame header.
	return ID3Ver <= 3 ? frameContent[headerSize() - 1] : frameContent[HEADER_BYTE_SIZE + 1];
}

///@pkg ID3Frame.h
ushort Frame::headerSize() const {
	return HEADER_BYTE_SIZE +
	       (compressed() ? 4 : 0) +
	       (encrypted() ? 1 : 0) +
	       (groupingIdentity() ? 1 : 0) +
	       (dataLengthIndicator() ? 4 : 0);
}

///@pkg ID3Frame.h
void Frame::print() const {
	const ushort HEADER_SIZE = headerSize();
	const ulong BODY_SIZE = size();
	const ulong FRAME_SIZE = size(true);
	
	std::cout << std::showbase << "Information about frame " << id << ": " << std::endl;
	std::cout << "Edited:         " << std::boolalpha << isEdited << std::endl;
	std::cout << "Read from file: " << std::boolalpha << isFromFile << std::endl;
	std::cout << "Null:           " << std::boolalpha << isNull << std::endl;
	
	if(isNull) {
		std::cout << std::noboolalpha << std::noshowbase;
		return;
	}
	
	std::cout << "Frame size:     " << FRAME_SIZE << std::endl;
	if(FRAME_SIZE > 0) {
		std::cout << "Flags:          ";
		if(discardUponTagAlterIfUnknown()) std::cout << " -discardIfUnknown";
		if(discardUponAudioAlter())        std::cout << " -discardUponAudioAlter";
		if(readOnly()) std::cout << " -readOnly";
		if(compressed()) std::cout << " -compressed";
		if(encrypted()) std::cout << " -encrypted";
		if(groupingIdentity()) std::cout << " -groupingIdentity";
		if(unsynchronized()) std::cout << " -unsynchronization";
		if(dataLengthIndicator()) std::cout << " -dataLengthIndicator";
		std::cout << std::endl;
		if(groupingIdentity())
			std::cout << "Group identity: " << std::boolalpha << groupIdentity() << std::endl;
		std::cout << "Header size:    " << std::dec << HEADER_SIZE << std::endl;
		std::cout << "Header bytes:  ";
		for(ulong i = 0; i < HEADER_SIZE && i < FRAME_SIZE; i++)
			std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
		std::cout << std::endl;
		
		std::cout << "Body size:      " << std::dec << BODY_SIZE << std::endl;
	}
	std::cout << "Empty:          " << std::boolalpha << empty() << std::endl;
	std::cout << "Body bytes:    ";
	if(BODY_SIZE <= 100) {
		for(ulong i = HEADER_SIZE; i < FRAME_SIZE; i++)
			std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
	} else {
		std::cout << " (only showing the first 100 bytes)";
		for(ulong i = HEADER_SIZE; i < HEADER_SIZE + 100UL && i < FRAME_SIZE; i++)
			std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
	}
	std::cout << std::endl << std::noboolalpha << std::dec << std::noshowbase;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  U N K N O W N F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame(const std::string& frameName,
                           const ushort version,
                           const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                       version,
                                                                       frameBytes) {}

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
void UnknownFrame::print() const {
	Frame::print();
	std::cout << "Frame class:    UnknownFrame" << std::endl;
}

///@pkg ID3Frame.h
ByteArray UnknownFrame::write(ushort version, bool minimize) {
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
