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
/////////////////////////  P L A Y C O U N T F R A M E /////////////////////////
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
	isEdited = true;
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
	
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is null don't write anything to file
	if(isNull) {
		frameContent = ByteArray();
	} else {
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
		for(ulong i = 0; i < playCountArr.size() && i+HEADER_BYTE_SIZE < NEW_FRAME_SIZE; i++)
			frameContent[HEADER_BYTE_SIZE + 1] = playCountArr[i];
	}
	
	isEdited = false;
	
	return frameContent;
}

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = frameContent.size();
	
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  P O P U L A R I M E T E R F R A M E /////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3PlayCountFrame.h
PopularimeterFrame::PopularimeterFrame(const ushort version,
                                       const ByteArray& frameBytes) : Frame::Frame(FRAME_POPULARIMETER,
                                                                                   version,
                                                                                   frameBytes) {
	count = 0ULL;
	
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3PlayCountFrame.h
PopularimeterFrame::PopularimeterFrame(const ushort version,
                                       const unsigned long long playCount,
                                       uint8_t rating,
                                       const std::string& email) : Frame::Frame(),
                                                                   emailAddress(email) {
	id = FRAME_POPULARIMETER;
	ID3Ver = version;
	isNull = false;
	count = 0ULL;
	//Take advantage of the already existing if statements
	this->rating(rating);
	isEdited = false;
}

///@pkg ID3PlayCountFrame.h
PopularimeterFrame::PopularimeterFrame() noexcept : Frame::Frame(),
                                                    fiveStarRating(0) {
	count = 0ULL;
}


///@pkg ID3PlayCountFrame.h
FrameClass PopularimeterFrame::type() const noexcept {
	return FrameClass::CLASS_POPULARIMETER;
}

///@pkg ID3PlayCountFrame.h
ushort PopularimeterFrame::rating() const { return fiveStarRating; }

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::rating(const uint8_t newRating) {
	     if(newRating <= 5)   fiveStarRating = newRating;
	else if(newRating <= 31)  fiveStarRating = 1;
	else if(newRating <= 95)  fiveStarRating = 2;
	else if(newRating <= 159) fiveStarRating = 3;
	else if(newRating <= 223) fiveStarRating = 4;
	else                      fiveStarRating = 5;
	
	isEdited = true;
}

///@pkg ID3PlayCountFrame.h
std::string PopularimeterFrame::email() const { return emailAddress; }

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::email(const std::string& newEmail) {
	emailAddress = newEmail;
	isEdited = true;
}

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::print() const {
	Frame::print();
	std::cout << "Play count:     " << count << std::endl;
	std::cout << "Rating:         " << (fiveStarRating == 0 ? "N/A" : std::to_string(fiveStarRating)) << std::endl;
	std::cout << "Email:          " << emailAddress << std::endl;
	std::cout << "Frame class:    PopularimeterFrame" << std::endl;
}

///@pkg ID3TextFrame.h
ByteArray PopularimeterFrame::write() {
	//TODO: Do something (like throw an exception) if the picture data is too
	//      large for the frame
	//TODO: Don't just assume that the email address isn't LATIN-1
	
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is null don't write anything to file
	if(isNull) {
		frameContent = ByteArray();
	} else {
		//A ByteArray of the play count encoded as a byte array
		ByteArray playCountArr = intToByteArray(count, 0, false);
		
		//The play count must be at least 32 bits (4 bytes) on file
		//If it's smaller, then insert 0s at the beginning
		if(playCountArr.size() < 4)
			playCountArr.insert(playCountArr.begin(), 4 - playCountArr.size(), 0);
		
		//The position that the email ends
		const ulong EMAIL_END_BYTE = HEADER_BYTE_SIZE + emailAddress.size();
		
		//Create a ByteArray that fits the header, email, email separating byte,
		//rating, and play count
		const ulong NEW_FRAME_SIZE = EMAIL_END_BYTE + 2 + playCountArr.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];
		
		//Save the email address
		for(ulong i = 0; i < emailAddress.size(); i++)
			frameContent[HEADER_BYTE_SIZE + i] = emailAddress[i];
		std::cout << "Wrote the email \"" << emailAddress << "\"" << std::endl;
		//Save the rating
		switch(fiveStarRating) {
			case 5: { frameContent[EMAIL_END_BYTE + 1] = 255; break; }
			case 4: { frameContent[EMAIL_END_BYTE + 1] = 196; break; }
			case 3: { frameContent[EMAIL_END_BYTE + 1] = 128; break; }
			case 2: { frameContent[EMAIL_END_BYTE + 1] = 64;  break; }
			case 1: case 0: default: frameContent[EMAIL_END_BYTE + 1] = fiveStarRating;
		}
		
		//Insert the play count into the frame content
		for(ulong i = 0; i < playCountArr.size() && i+EMAIL_END_BYTE+2 < NEW_FRAME_SIZE; i++)
			frameContent[EMAIL_END_BYTE + 2 + i] = playCountArr[i];
	}
	
	isEdited = false;
	
	return frameContent;
}

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = frameContent.size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() > HEADER_SIZE) {
		ulong emailEnd = HEADER_SIZE;
		
		for(ulong i = HEADER_SIZE; i < FRAME_SIZE - 2; i++) {
			if(frameContent[i] == '\0') {
				emailEnd = i;
				break;
			}
		}
		
		//Read the email address
		emailAddress = getUTF8String(ENCODING_LATIN1, //Email addresses are in LATIN-1, no encoding byte
		                             frameContent,
		                             HEADER_SIZE,
		                             emailEnd);
		
		//Read the rating on file
		const uint8_t POPM_RATING = frameContent[emailEnd + 1];
		     if(POPM_RATING <= 31)  fiveStarRating = 1;
		else if(POPM_RATING <= 95)  fiveStarRating = 2;
		else if(POPM_RATING <= 159) fiveStarRating = 3;
		else if(POPM_RATING <= 223) fiveStarRating = 4;
		else                        fiveStarRating = 5;
		
		//Read the play count on file
		count = byteIntVal(frameContent.data()+emailEnd + 2, FRAME_SIZE - emailEnd - 2, false);
	} else {
		isNull = true;
		count = 0ULL;
	}
}

///@pkg ID3PlayCountFrame.h
bool PopularimeterFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a PopularimeterFrame, and if it is compare the content
	const PopularimeterFrame* const castFrame = dynamic_cast<const PopularimeterFrame* const>(frame);
	//If it's not a PlayCountFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : (count == castFrame->count &&
	                        fiveStarRating == castFrame->fiveStarRating &&
	                        emailAddress == castFrame->emailAddress);
}
