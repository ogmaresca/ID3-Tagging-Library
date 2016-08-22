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

#include "ID3.h"
#include "ID3TextFrame.h"
#include "ID3PictureFrame.h"
#include "ID3Functions.h"
#include "ID3Constants.h"

using namespace ID3;

///@pkg ID3.h
Picture::Picture(const ByteArray&   pictureByteArray,
                 const std::string& mimeType,
                 const std::string& pictureDescription,
                 const PictureType  pictureType) : MIME(mimeType),
			                                          type(pictureType),
			                                          description(pictureDescription),
			                                          data(pictureByteArray),
			                                          null(!PictureFrame::allowedMIMEType(mimeType)) {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////  P I C T U R E F R A M E ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3PictureFrame.h
PictureFrame::PictureFrame(const unsigned short version,
                           const ByteArray& frameBytes) : Frame::Frame("APIC",
                                                                       version,
                                                                       frameBytes),
                                                          APICType(PictureType::OTHER) {
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3PictureFrame.h
PictureFrame::PictureFrame(const unsigned short version,
                           const ByteArray pictureBytes,
			                  const std::string& mimeType,
			                  const std::string& description,
			                  const PictureType type) : Frame::Frame(),
			                                            textMIME(mimeType),
									                          APICType(type),
									                          textDescription(description),
			                                            pictureData(pictureBytes) {
	id = "APIC";
	ID3Ver = version;
	isNull = false;
}

///@pkg ID3PictureFrame.h
PictureFrame::PictureFrame() noexcept : Frame::Frame(),
                                        APICType(PictureType::OTHER) {}


///@pkg ID3PictureFrame.h
FrameClass PictureFrame::type() const noexcept {
	return FrameClass::CLASS_PICTURE;
}

///@pkg ID3PictureFrame.h
bool PictureFrame::empty() const {
	return pictureData.size() == 0;
}

///@pkg ID3PictureFrame.h
std::string PictureFrame::mimeType() const {
	return textMIME;
}

///@pkg ID3PictureFrame.h
PictureType PictureFrame::pictureType() const {
	return APICType;
}

///@pkg ID3PictureFrame.h
void PictureFrame::pictureType(PictureType newType) {
	APICType = newType;
}

///@pkg ID3PictureFrame.h
std::string PictureFrame::description() const {
	return textDescription;
}

///@pkg ID3PictureFrame.h
void PictureFrame::description(const std::string& newDescription) {
	textDescription = newDescription;
}

///@pkg ID3PictureFrame.h
ByteArray PictureFrame::picture() const {
	return pictureData;
}

///@pkg ID3PictureFrame.h
void PictureFrame::picture(const ByteArray& newPictureData,
                           const std::string& newMIMEType) {
	isNull = !allowedMIMEType(newMIMEType);
	pictureData = newPictureData;
	textMIME = newMIMEType;
}

///@pkg ID3TextFrame.h
ByteArray PictureFrame::write(unsigned short version, bool minimize) {
	if(version >= MIN_SUPPORTED_VERSION && version <= MAX_SUPPORTED_VERSION)
		ID3Ver = version;
	
	//isEdited = false;
		
	//TODO: Encode the frame ID and picture information into a ByteArray
	return frameContent;
}

///@pkg ID3TextFrame.h
void PictureFrame::read() {
	const unsigned short HEADER_SIZE = headerSize();
	
	const unsigned long FRAME_SIZE = size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() > HEADER_SIZE) {
		//The encoding
		const char encoding = frameContent[HEADER_SIZE];
		//If the encoding uses 16-byte or 8-byte characters
		const bool wideChars = encoding == ENCODING_UTF16BOM || encoding == ENCODING_UTF16;
		
		unsigned long mimeEnd   = 0,                 //The end of the MIME type string
		                                             //It will actually be the null
		                                             //character after the string
		              descStart = 0,                 //The start of the description
		              descEnd   = 0,                 //The end of the description
		              descGap   = wideChars ? 2 : 1; //The number of bytes in the
		                                             //gap between the description
		                                             //and the picture data
		
		//Get the MIME type
		for(unsigned long i = HEADER_SIZE + 1; i < FRAME_SIZE; i++) {
			if(frameContent[i] == '\0') {
				mimeEnd = i;
				//The MIME string is always stored in LATIN-1
				textMIME = getUTF8String(ENCODING_LATIN1,
				                         frameContent,
				                         HEADER_SIZE+1,
				                         i);
				break;
			}
		}
		//If the frame is improperly encoded, or there's an illegal MIME type
		if(mimeEnd == 0 || !allowedMIMEType(textMIME)) {
			isNull = true;
			return;
		}
		
		//Get the picture type
		if(mimeEnd + 1 >= FRAME_SIZE) {
			isNull = true;
			return;
		} else {
			switch(frameContent[mimeEnd + 1]) {
				case static_cast<char>(PictureType::FILE_ICON):
				case static_cast<char>(PictureType::OTHER_FILE_ICON):
				case static_cast<char>(PictureType::FRONT_COVER):
				case static_cast<char>(PictureType::BACK_COVER):
				case static_cast<char>(PictureType::LEAFLET_PAGE):
				case static_cast<char>(PictureType::MEDIA):
				case static_cast<char>(PictureType::LEAD_ARTIST):
				case static_cast<char>(PictureType::ARTIST):
				case static_cast<char>(PictureType::CONDUCTOR):
				case static_cast<char>(PictureType::BAND):
				case static_cast<char>(PictureType::COMPOSER):
				case static_cast<char>(PictureType::LYRICIST):
				case static_cast<char>(PictureType::RECORDING_LOCATION):
				case static_cast<char>(PictureType::DURING_RECORDING):
				case static_cast<char>(PictureType::DURING_PERFORMANCE):
				case static_cast<char>(PictureType::MOVIE_CAPTURE):
				case static_cast<char>(PictureType::BRIGHT_FISH):
				case static_cast<char>(PictureType::ILLUSTRATION):
				case static_cast<char>(PictureType::ARTIST_LOGOTYPE):
				case static_cast<char>(PictureType::PUBLISHER_LOGOTYPE): {
					APICType = static_cast<PictureType>(frameContent[mimeEnd + 1]);
					break;
				} case static_cast<char>(PictureType::OTHER): default: {
					APICType = PictureType::OTHER;
				}
			}
		}
		
		//Get the description
		descStart = mimeEnd + 2;
		for(unsigned long i = descStart; i + descGap <= FRAME_SIZE; i += descGap) {
			//Prevent false positives in UTF-16 encodings
			if(frameContent[i] == '\0') {
				if(wideChars && frameContent[i+1] != '\0')
					continue;
				descEnd = i;
				textDescription = getUTF8String(encoding,
				                                frameContent,
				                                descStart,
				                                descEnd);
				break;
			}
		}
		//If the frame is improperly encoded
		if(descEnd == 0) {
			isNull = true;
			return;
		}
		
		//Get the picture data
		if(descEnd + descGap < FRAME_SIZE)
			pictureData = ByteArray(frameContent.begin() + descEnd + descGap,
			                        frameContent.end());
		else
			pictureData = ByteArray();
	} else {
		isNull = true;
		textMIME = "";
		APICType = PictureType::OTHER;
		textDescription = "";
		pictureData = ByteArray();
	}
	
	//std::cout << "Content for PictureFrame " << id << " " << (short)APICType << textDescription << " " << textMIME << ": " << std::string(pictureData.begin(), pictureData.end()) << std::endl;
	std::cout << "Content for PictureFrame " << id << ": " << (short)APICType << textDescription << " " << textMIME;
}

///@pkg ID3PictureFrame.h
///@static
bool PictureFrame::allowedMIMEType(const std::string& mimeType) {
	return mimeType == "png"       || mimeType == "jpeg" ||
	       mimeType == "image/png" || mimeType == "image/jpeg";
}

///@pkg ID3PictureFrame.h
bool PictureFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a PictureFrame, and if it is compare the content
	const PictureFrame* const castFrame = dynamic_cast<const PictureFrame* const>(frame);
	//If it's not a PictureFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : (textMIME == castFrame->textMIME &&
	                        pictureData == castFrame->pictureData);
}
