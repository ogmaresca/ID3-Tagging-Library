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

#include "ID3PlayCountFrame.h"
#include "../ID3Functions.h"
#include "../ID3Constants.h"

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  P I C T U R E F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3PlayCountFrame.h
PlayCountFrame::PlayCountFrame(const ushort version,
                               const ByteArray& frameBytes) : Frame::Frame(FRAME_PLAY_COUNT,
                                                                           version,
                                                                           frameBytes),
                                                              count(0ULL) {
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3PlayCountFrame.h
PlayCountFrame::PlayCountFrame(const ushort version,
                           const unsigned long long playCount) : Frame::Frame(),
                                                                 count(playCount) {
	id = FRAME_PLAY_COUNT;
	ID3Ver = version;
	isNull = false;
}

///@pkg ID3PlayCountFrame.h
PlayCountFrame::PlayCountFrame() noexcept : Frame::Frame(),
                                            count(0ULL) {}


///@pkg ID3PlayCountFrame.h
FrameClass PlayCountFrame::type() const noexcept {
	return FrameClass::CLASS_PLAY_COUNT;
}

///@pkg ID3PlayCountFrame.h
bool PlayCountFrame::empty() const { return false; }

///@pkg ID3PlayCountFrame.h
unsigned long long PlayCountFrame::playCount() const { return count; }

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::playCount(const unsigned long long newPlayCount) {
	count = newPlayCount;
}

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::print() const {
	Frame::print();
	std::cout << "Play count:     " << count << std::endl;
	std::cout << "Frame class:    PlayCountFrame" << std::endl;
}

///@pkg ID3TextFrame.h
ByteArray PlayCountFrame::write() {
	//TODO: Do something (like throw an exception) if the picture data is too
	//      large for the frame
	
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is null don't write anything to file
	if(isNull) {
		frameContent = ByteArray();
	} else if(isEdited || isFromFile || OLD_VERSION == 3) {
		//A ByteArray of the play count encoded as a byte array
		ByteArray playCountArr = intToByteArray(count, 0, false);
		
		//The play count must be at least 32 bits (4 bytes) on file
		//If it's smaller, then insert 0s at the beginning
		if(playCountArr.size() < 4)
			playCountArr.insert(playCountArr.begin(), 4 - playCountArr.size(), 0);
		
		//Create a ByteArray that fits the header and play count
		const ulong NEW_FRAME_SIZE = HEADER_BYTE_SIZE + playCountArr.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];		
		
		//Insert the play count into the frame content
		
		for(ulong i = 0; i < playCountArr.size() && i+HEADER_BYTE_SIZE < frameContent.size(); i++)
			frameContent[HEADER_BYTE_SIZE + 1] = playCountArr[i];
	}
	
	isEdited = false;
	
	return frameContent;
}

///@pkg ID3TextFrame.h
void PlayCountFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() > HEADER_SIZE) {
		//Read the play count on file
		count = byteIntVal(frameContent.data()+HEADER_SIZE, FRAME_SIZE - HEADER_SIZE, false);
	} else {
		isNull = true;
		count = 0ULL;
	}
}

///@pkg ID3PlayCountFrame.h
bool PlayCountFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a PlayCountFrame, and if it is compare the content
	const PlayCountFrame* const castFrame = dynamic_cast<const PlayCountFrame* const>(frame);
	//If it's not a PlayCountFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : count == castFrame->count;
}
