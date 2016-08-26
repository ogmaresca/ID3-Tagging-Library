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
PictureFrame::PictureFrame(const ushort version,
                           const ByteArray& frameBytes) : Frame::Frame("APIC",
                                                                       version,
                                                                       frameBytes),
                                                          APICType(PictureType::OTHER) {
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3PictureFrame.h
PictureFrame::PictureFrame(const ushort version,
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

///@pkg ID3PictureFrame.h
void PictureFrame::print() const {
	Frame::print();
	std::cout << "Picture type:   " << (short)APICType << std::endl;
	std::cout << "MIME type:      " << textMIME << std::endl;
	std::cout << "Description:    " << textDescription << std::endl;
	std::cout << "Frame class:    PictureFrame" << std::endl;
}

///@pkg ID3TextFrame.h
ByteArray PictureFrame::write() {
	//TODO: Do something (like throw an exception) if the picture data is too
	//      large for the frame
	
	//Save the old version to take synchsafe-ness into account
	const ushort OLD_VERSION = ID3Ver;
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	//If the Frame is empty or null, then don't write anything to file
	if(empty() || isNull) {
		frameContent = ByteArray();
	} else if(isEdited || isFromFile || OLD_VERSION == 3) {
		//The description starts after the header, encoding, MIME type, and
		//picture type.
		const ulong DESCRIPTION_START = HEADER_BYTE_SIZE + 3 + textMIME.length();
		
		//The picture starts after where the description starts plus a null byte.
		const ulong PIC_START = DESCRIPTION_START + textDescription.size() + 1;
		
		//Create a ByteArray that fits the header, encoding, MIME type,
		//description text size, picture type, and picture data size
		const ulong NEW_FRAME_SIZE = PIC_START + pictureData.size();
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(NEW_FRAME_SIZE, '\0');
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];		
		
		//Set the encoding to UTF-8
		frameContent[HEADER_BYTE_SIZE] = FrameEncoding::ENCODING_UTF8;
		
		//Write the MIME type to file
		for(ulong i = 0; i < textMIME.size(); i++)
			frameContent[HEADER_BYTE_SIZE + 1 + i] = textMIME[i];
		
		//Write the picture type to file
		frameContent[DESCRIPTION_START - 1] = static_cast<uint8_t>(APICType);
		
		//Write the description to file. Since frameContent was initialized with
		//null characters, the null separator is already in the array.
		for(ulong i = DESCRIPTION_START; i < textDescription.size(); i++)
			frameContent[DESCRIPTION_START + i] = textDescription[i];
		
		//Write the picture data to file
		for(ulong i = 0; i < pictureData.size() && PIC_START + i < NEW_FRAME_SIZE; i++)
			frameContent[PIC_START + 1] = pictureData[i];
	}
	
	isEdited = false;
	
	//TODO: Encode the frame ID and textContent contents into a ByteArray
	return frameContent;
}

///@pkg ID3TextFrame.h
void PictureFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = size();
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(frameContent.size() > HEADER_SIZE) {
		//The encoding
		const uint8_t encoding = frameContent[HEADER_SIZE];
		//If the encoding uses 16-byte or 8-byte characters
		const bool wideChars = encoding == ENCODING_UTF16BOM || encoding == ENCODING_UTF16;
		
		ulong mimeEnd   = 0,                 //The end of the MIME type string
		                                     //It will actually be the null
		                                     //character after the string
		      descStart = 0,                 //The start of the description
		      descEnd   = 0,                 //The end of the description
		      descGap   = wideChars ? 2 : 1; //The number of bytes in the gap
		                                     //between the description and the
		                                     //picture data
		
		//Get the MIME type
		for(ulong i = HEADER_SIZE + 1; i < FRAME_SIZE; i++) {
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
				case static_cast<uint8_t>(PictureType::FILE_ICON):
				case static_cast<uint8_t>(PictureType::OTHER_FILE_ICON):
				case static_cast<uint8_t>(PictureType::FRONT_COVER):
				case static_cast<uint8_t>(PictureType::BACK_COVER):
				case static_cast<uint8_t>(PictureType::LEAFLET_PAGE):
				case static_cast<uint8_t>(PictureType::MEDIA):
				case static_cast<uint8_t>(PictureType::LEAD_ARTIST):
				case static_cast<uint8_t>(PictureType::ARTIST):
				case static_cast<uint8_t>(PictureType::CONDUCTOR):
				case static_cast<uint8_t>(PictureType::BAND):
				case static_cast<uint8_t>(PictureType::COMPOSER):
				case static_cast<uint8_t>(PictureType::LYRICIST):
				case static_cast<uint8_t>(PictureType::RECORDING_LOCATION):
				case static_cast<uint8_t>(PictureType::DURING_RECORDING):
				case static_cast<uint8_t>(PictureType::DURING_PERFORMANCE):
				case static_cast<uint8_t>(PictureType::MOVIE_CAPTURE):
				case static_cast<uint8_t>(PictureType::BRIGHT_FISH):
				case static_cast<uint8_t>(PictureType::ILLUSTRATION):
				case static_cast<uint8_t>(PictureType::ARTIST_LOGOTYPE):
				case static_cast<uint8_t>(PictureType::PUBLISHER_LOGOTYPE): {
					APICType = static_cast<PictureType>(frameContent[mimeEnd + 1]);
					break;
				} case static_cast<uint8_t>(PictureType::OTHER): default: {
					APICType = PictureType::OTHER;
				}
			}
		}
		
		//Get the description
		descStart = mimeEnd + 2;
		for(ulong i = descStart; i + descGap <= FRAME_SIZE; i += descGap) {
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
