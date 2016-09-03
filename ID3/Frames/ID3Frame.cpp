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

#include "ID3Frame.hpp" //For the class definitions
#include "../ID3Functions.hpp" //For intToByteArray
#include "../ID3Constants.hpp" //For HEADER_BYTE_SIZE and WRITE_VERSION

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  F R A M E //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
Frame::Frame(const FrameID& frameName) noexcept : id(frameName),
                                                  ID3Ver(WRITE_VERSION),
                                                  isNull(id == Frames::FRAME_UNKNOWN_FRAME),
                                                  isEdited(false),
                                                  isFromFile(false) {}

///@pkg ID3Frame.h
Frame::Frame(const FrameID&   frameName,
             const ushort     version,
             const ByteArray& frameBytes) : id(frameName),
                                            ID3Ver(version),
                                            frameContent(frameBytes),
                                            isNull(frameBytes.size() <= HEADER_BYTE_SIZE),
                                            isEdited(false),
                                            isFromFile(true) {
	if(!isNull && (flag(FrameFlag::COMPRESSED) || flag(FrameFlag::ENCRYPTED)))
		isNull = true;
	else
		unsynchronise();
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
FrameID Frame::frame() const { return id; }

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

bool Frame::flag(const FrameFlag flag) const {
	//Verify that the frame is valid
	if(frameContent.size() < HEADER_BYTE_SIZE || ID3Ver <= 3)
		return false;
	
	const bool V4 = ID3Ver >= 4;
	
	switch(flag) {
		case FrameFlag::DISCARD_UPON_TAG_ALTER_IF_UNKNOWN:
			return V4 ?
			       ((frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4) == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4) :
	             ((frameContent[8] & FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V3) == FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V3);
		case FrameFlag::DISCARD_UPON_AUDIO_ALTER:
			return V4 ?
			       ((frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER_V4) == FLAG1_DISCARD_UPON_AUDIO_ALTER_V4) :
	             ((frameContent[8] & FLAG1_DISCARD_UPON_AUDIO_ALTER_V3) == FLAG1_DISCARD_UPON_AUDIO_ALTER_V3);
	   case FrameFlag::READ_ONLY:
			return V4 ?
			       ((frameContent[8] & FLAG1_READ_ONLY_V4) == FLAG1_READ_ONLY_V4) :
	             ((frameContent[8] & FLAG1_READ_ONLY_V3) == FLAG1_READ_ONLY_V3);
	   case FrameFlag::COMPRESSED:
			return V4 ?
			       ((frameContent[8] & FLAG2_COMPRESSED_V4) == FLAG2_COMPRESSED_V4) :
	             ((frameContent[8] & FLAG2_COMPRESSED_V3) == FLAG2_COMPRESSED_V3);
	   case FrameFlag::ENCRYPTED:
			return V4 ?
			       ((frameContent[8] & FLAG2_ENCRYPTED_V4) == FLAG2_ENCRYPTED_V4) :
	             ((frameContent[8] & FLAG2_ENCRYPTED_V3) == FLAG2_ENCRYPTED_V3);
	   case FrameFlag::GROUPING_IDENTITY:
			return V4 ?
			       ((frameContent[8] & FLAG2_GROUPING_IDENTITY_V4) == FLAG2_GROUPING_IDENTITY_V4) :
	             ((frameContent[8] & FLAG2_GROUPING_IDENTITY_V3) == FLAG2_GROUPING_IDENTITY_V3);
	   case FrameFlag::UNSYNCHRONISED:
			return V4 ? ((frameContent[8] & FLAG2_UNSYNCHRONISED_V4) == FLAG2_UNSYNCHRONISED_V4) : false;
	   case FrameFlag::DATA_LENGTH_INDICATOR:
			return V4 ? ((frameContent[8] & FLAG2_DATA_LENGTH_INDICATOR_V4) == FLAG2_DATA_LENGTH_INDICATOR_V4) : false;
		default:
			return false;
	}
}

///@pkg ID3Frame.h
uint8_t Frame::groupIdentity() const {
	if(frameContent.size() < headerSize() || !flag(FrameFlag::GROUPING_IDENTITY))
		return 0;
	//In ID3v2.3, the group identity is the last flag that adds bytes to the
	//header, so it's at the very end. In ID3v2.4 it's the first, so get the byte
	//right after the regular frame header.
	return ID3Ver <= 3 ? frameContent[headerSize() - 1] : frameContent[HEADER_BYTE_SIZE + 1];
}

///@pkg ID3Frame.h
ushort Frame::headerSize() const {
	return HEADER_BYTE_SIZE +
	       (flag(FrameFlag::COMPRESSED) ? 4 : 0) +
	       (flag(FrameFlag::ENCRYPTED) ? 1 : 0) +
	       (flag(FrameFlag::GROUPING_IDENTITY) ? 1 : 0) +
	       (flag(FrameFlag::DATA_LENGTH_INDICATOR) ? 4 : 0);
}

///@pkg ID3Frame.h
void Frame::print() const {
	const ushort HEADER_SIZE = headerSize();
	const ulong BODY_SIZE = size();
	const ulong FRAME_SIZE = size(true);
	
	std::cout << std::showbase << "Information about " << id.description() << " frame " << id << ": \n";
	std::cout << "Edited:         " << std::boolalpha << isEdited << '\n';
	std::cout << "Read from file: " << std::boolalpha << isFromFile << '\n';
	std::cout << "Null:           " << std::boolalpha << isNull << '\n';
	
	if(isNull) {
		std::cout << std::noboolalpha << std::noshowbase;
		return;
	}
	
	std::cout << "Frame size:     " << FRAME_SIZE << '\n';
	if(FRAME_SIZE == 0) return;
	
	std::cout << "Flags:          ";
	if(flag(FrameFlag::DISCARD_UPON_TAG_ALTER_IF_UNKNOWN)) std::cout << " -discardIfUnknown";
	if(flag(FrameFlag::DISCARD_UPON_AUDIO_ALTER)) std::cout << " -discardUponAudioAlter";
	if(flag(FrameFlag::READ_ONLY))  std::cout << " -readOnly";
	if(flag(FrameFlag::COMPRESSED)) std::cout << " -compressed";
	if(flag(FrameFlag::ENCRYPTED))  std::cout << " -encrypted";
	if(flag(FrameFlag::GROUPING_IDENTITY)) std::cout << " -groupingIdentity";
	if(flag(FrameFlag::UNSYNCHRONISED)) std::cout << " -unsynchronisation";
	if(flag(FrameFlag::DATA_LENGTH_INDICATOR)) std::cout << " -dataLengthIndicator";
	std::cout << '\n';
	if(flag(FrameFlag::GROUPING_IDENTITY))
		std::cout << "Group identity: " << std::boolalpha << groupIdentity() << '\n';
	std::cout << "Header size:    " << std::dec << HEADER_SIZE << '\n';
	std::cout << "Header bytes:  ";
	for(ulong i = 0; i < HEADER_SIZE && i < FRAME_SIZE; i++)
		std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
	std::cout << '\n';
	
	std::cout << "Empty:          " << std::boolalpha << empty() << '\n';
	std::cout << "Body size:      " << std::dec << BODY_SIZE << '\n';
	std::cout << "Body bytes:    ";
	if(BODY_SIZE <= 100) {
		for(ulong i = HEADER_SIZE; i < FRAME_SIZE; i++)
			std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
	} else {
		std::cout << " (only showing the first 100 bytes)";
		for(ulong i = HEADER_SIZE; i < HEADER_SIZE + 100UL && i < FRAME_SIZE; i++)
			std::cout << std::hex << ' ' << static_cast<short>(frameContent[i]);
	}
	std::cout << '\n' << std::noboolalpha << std::dec << std::noshowbase;
}

///@pkg ID3Frame.h
ByteArray Frame::write() {
	const bool GROUPING_IDENTITY = flag(FrameFlag::GROUPING_IDENTITY);
	const uint8_t GROUP_IDENTITY = groupIdentity();
	
	//Some frames have the Discard Upon Audio Alter flag set by default
	bool discardUponAudioAlter;
	switch(id) {
		case FRAME_AUDIO_ENCRYPTION:
		case FRAME_AUDIO_SEEK_POINT_INDEX:
		case FRAME_EVENT_TIMING_CODES:
		case FRAME_EQUALISATION:
		case FRAME_EQUALISATION_2:
		case FRAME_MPEG_LOCATION_LOOKUP_TABLE:
		case FRAME_POSITION_SYNCHRONISATION:
		case FRAME_SEEK:
		case FRAME_SYNCHRONISED_LYRICS:
		case FRAME_SYNCHRONISED_TEMPO_CODES:
		case FRAME_RELATIVE_VOLUME_ADJUSTMENT:
		case FRAME_RELATIVE_VOLUME_ADJUSTMENT_2:
		case FRAME_ENCODED_BY:
		case FRAME_LENGTH:
		case FRAME_SIZE: {
			discardUponAudioAlter = true; break;
		} default: {
			discardUponAudioAlter = false;
		}
	}
	
	//The new header size has to fit the grouping identity if necessary
	const ushort HEADER_SIZE = HEADER_BYTE_SIZE + (GROUPING_IDENTITY ? 1 : 0);
	
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	if(isNull || empty()) {
		//If null or empty, clear the frame
		frameContent = ByteArray();
	} else {
		//Recreate the frame content to fit the new header
		//This automatically clears any flags
		frameContent = ByteArray(HEADER_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the discard upon audio alter flag
		if(discardUponAudioAlter)
			frameContent[9] = FLAG1_DISCARD_UPON_AUDIO_ALTER_V4;
		
		//Save the grouping identity
		if(GROUPING_IDENTITY) {
			frameContent[9] = FLAG2_GROUPING_IDENTITY_V4;
			frameContent[HEADER_SIZE - 1] = GROUP_IDENTITY;
		}
		
		//Call the abstract method to write the body
		writeBody();
		
		//Save the frame size
		ByteArray size = intToByteArray(frameContent.size() - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];
	}
	
	isEdited = false;
	
	return frameContent;
}

///@pkg ID3Frame.h
void Frame::unsynchronise() {
	if(!flag(FrameFlag::UNSYNCHRONISED))
		return;
	
	//The current frame size
	const ulong FRAME_SIZE = frameContent.size();
	
	//The new byte vector
	ByteArray newFrameContent;
	
	//Reserve space in the new vector to prevent internal array re-allocations
	newFrameContent.reserve(FRAME_SIZE);
	
	//Loop through the frame bytes, starting after the standard header
	for(ulong i = HEADER_BYTE_SIZE; i + 2 < FRAME_SIZE; i++) {
		newFrameContent.push_back(frameContent[i]);
		
		//If the byte pattern 0b11111111 0b00000000 0b111XXXXX is found, then the
		//0 byte has been inserted to prevent false synchronisations and should
		//be skipped over.
		if(frameContent[i] == 0xFF && frameContent[i+1] == '\0' && frameContent[i+2] > 0xE0)
			i++;
	}
	
	frameContent = newFrameContent;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  U N K N O W N F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame(const FrameID&   frameName,
                           const ushort     version,
                           const ByteArray& frameBytes) : Frame::Frame(frameName,
                                                                       version,
                                                                       frameBytes) {}

///@pkg ID3Frame.h
UnknownFrame::UnknownFrame(const FrameID& frameName) noexcept : Frame::Frame(frameName) {}

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
	std::cout << "Frame class:    UnknownFrame\n";
}

///@pkg ID3Frame.h
ByteArray UnknownFrame::write() {
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//Clear the isEdited variable
	isEdited = false;
	
	//If the frame is invalid, or the Discard Frame Upon Tag Alter flag is set,
	//then clear the frame
	if(flag(FrameFlag::DISCARD_UPON_TAG_ALTER_IF_UNKNOWN) || isNull || empty() ||
	   frameContent.size() < HEADER_BYTE_SIZE) {
		frameContent = ByteArray();
		isNull = true;
	} else if(OLD_VERSION <= 3) {
		//Whether a frame size is synchsafe has been changed from ID3v2.3 to
		//ID3v2.4, so it must be updated to report the correct frame size
		ByteArray frameSize = intToByteArray(frameContent.size() - HEADER_BYTE_SIZE, 4, true);
		for(short i = 0; i < 4; i++)
			frameContent[i+4] = frameSize[i];
	}
	return frameContent;
}

///@pkg ID3Frame.h
void UnknownFrame::writeBody() {}

///@pkg ID3Frame.h
void UnknownFrame::read() {}
