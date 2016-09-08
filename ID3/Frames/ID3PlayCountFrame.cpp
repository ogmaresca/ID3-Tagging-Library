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

#include "ID3PlayCountFrame.hpp" //For the class definitions
#include "../ID3Functions.hpp"   //For intToByteArray(), byteIntVal(), and getUTF8String()

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
PlayCountFrame::PlayCountFrame(const unsigned long long playCount) noexcept : Frame::Frame(FRAME_PLAY_COUNT),
                                                                              count(playCount) {}

///@pkg ID3PlayCountFrame.h
PlayCountFrame::~PlayCountFrame() {}

///@pkg ID3PlayCountFrame.h
FrameClass PlayCountFrame::type() const noexcept { return FrameClass::CLASS_PLAY_COUNT; }

///@pkg ID3PlayCountFrame.h
bool PlayCountFrame::empty() const { return count == 0ULL; }

///@pkg ID3PlayCountFrame.h
unsigned long long PlayCountFrame::playCount() const { return count; }

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::playCount(const unsigned long long newPlayCount) {
	count = newPlayCount;
	isEdited = true;
}

///@pkg ID3PlayCountFrame.h
std::string PlayCountFrame::print() const {
	return Frame::print() +
	       "Play count:     " + std::to_string(count) +
	       "\nFrame class:    PlayCountFrame\n";
}

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::writeBody() {
	//A ByteArray of the play count encoded as a byte array
	//The play count must be at least 32 bits (4 bytes) on file
	//If the size parameter is 0, then the ByteArray will be as big as it needs to
	ByteArray playCountArr = intToByteArray(count, count >= (1ULL << 32) ? 0 : 4, false);
	
	//Write the play count to file
	frameContent.insert(frameContent.end(), playCountArr.begin(), playCountArr.end());
}

///@pkg ID3PlayCountFrame.h
void PlayCountFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = frameContent.size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(FRAME_SIZE > HEADER_SIZE) {
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
PopularimeterFrame::PopularimeterFrame(const unsigned long long playCount,
                                       uint8_t rating,
                                       const std::string& email) noexcept : Frame::Frame(FRAME_POPULARIMETER),
                                                                            emailAddress(email) {
	count = playCount;
	//Take advantage of the already existing if statements
	this->rating(rating);
	isEdited = false;
}

///@pkg ID3PlayCountFrame.h
PopularimeterFrame::~PopularimeterFrame() {}

///@pkg ID3PlayCountFrame.h
FrameClass PopularimeterFrame::type() const noexcept { return FrameClass::CLASS_POPULARIMETER; }

///@pkg ID3PlayCountFrame.h
bool PopularimeterFrame::empty() const { return count == 0ULL &&
	                                             fiveStarRating == 0 &&
	                                             emailAddress.empty(); }

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
std::string PopularimeterFrame::print() const {
	return Frame::print() +
	       "Play count:     " + std::to_string(count) +
	       "\nRating:         " + (fiveStarRating == 0 ? "N/A" : std::to_string(fiveStarRating)) +
	       "\nEmail:          " + emailAddress +
	       "\nFrame class:    PopularimeterFrame\n";
}

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::writeBody() {
	//A ByteArray of the play count encoded as a byte array
	//The play count must be at least 32 bits (4 bytes) on file
	//If the size parameter is 0, then the ByteArray will be as big as it needs to
	ByteArray playCountArr = intToByteArray(count, count >= (1ULL << 32) ? 0 : 4, false);
	
	//Write the email address to file and its null separator
	frameContent.insert(frameContent.end(), emailAddress.begin(), emailAddress.end());
	frameContent.push_back('\0');
	
	//Add the rating to file
	switch(fiveStarRating) {
		case 5: { frameContent.push_back(255); break; }
		case 4: { frameContent.push_back(196); break; }
		case 3: { frameContent.push_back(128); break; }
		case 2: { frameContent.push_back(64);  break; }
		case 1: case 0: default: frameContent.push_back(fiveStarRating);
	}
	
	//Write the play count to file
	frameContent.insert(frameContent.end(), playCountArr.begin(), playCountArr.end());
}

///@pkg ID3PlayCountFrame.h
void PopularimeterFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = frameContent.size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(FRAME_SIZE > HEADER_SIZE) {
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
