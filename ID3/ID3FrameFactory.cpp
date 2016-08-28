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

#include "ID3.h"
#include "ID3FrameFactory.h"
#include "ID3TextFrame.h"
#include "ID3PictureFrame.h"
#include "ID3Functions.h"
#include "ID3Constants.h"

using namespace ID3;

///@pkg ID3FrameFactory.h
FrameFactory::FrameFactory(std::ifstream& file,
                           const ushort   version,
                           const ulong    tagEnd) : musicFile(&file),
                                                    ID3Ver(version),
                                                    ID3Size(tagEnd) {}

///@pkg ID3FrameFactory.h	                                              
FrameFactory::FrameFactory(const ushort version) : musicFile(nullptr),
                                                   ID3Ver(version),
                                                   ID3Size(0) {}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const ulong readpos) const {
	//Validate the file
	if(readpos + HEADER_BYTE_SIZE > ID3Size ||
	   musicFile == nullptr ||
	   !musicFile->is_open())
		return FramePtr(new UnknownFrame());
	
	//Seek to the read position
	musicFile->seekg(readpos, std::ifstream::beg);
	if(musicFile->fail()) return FramePtr(new UnknownFrame());
	
	//The Frame class that should be returned
	FrameClass frameType;
	
	//The ByteArray of the frame's bytes read from the file
	ByteArray frameBytes;
	
	//The ID3v2 frame ID that will be read from file
	//std::string id;
	FrameID id;
	
	//ID3v2.2 and below have a different frame header structure, so they need to
	//be read differently
	if(ID3Ver >= 3) {
		//Read the frame header
		FrameHeader header;
		musicFile->read(reinterpret_cast<char*>(&header), HEADER_BYTE_SIZE);
		
		//Get the size of the frame
		ulong frameSize = byteIntVal(header.size, 4, ID3Ver >= 4);
		
		//Validate the frame size
		if(frameSize == 0 || frameSize + HEADER_BYTE_SIZE > ID3Size)
			return FramePtr(new UnknownFrame());
		
		//Get the frame ID
		id = terminatedstring(header.id, 4);
		
		//Get the class the Frame should be
		frameType = FrameFactory::frameType(id);
		
		//Create the ByteArray with the entire frame contents
		frameBytes = ByteArray(frameSize + HEADER_BYTE_SIZE, '\0');
		musicFile->seekg(readpos, std::ifstream::beg);
		if(musicFile->fail())
			return FramePtr(new UnknownFrame(id));
		musicFile->read(reinterpret_cast<char*>(&frameBytes.front()), frameSize + HEADER_BYTE_SIZE);
	} else {
		//The ID3v2.2 frame header has 6 bytes instead of 10
		const ushort OLD_FRAME_HEADER_BYTE_SIZE = sizeof(V2FrameHeader);
		
		//Read the frame header
		V2FrameHeader header;
		musicFile->read(reinterpret_cast<char*>(&header), OLD_FRAME_HEADER_BYTE_SIZE);
		
		//Get the size of the frame
		ulong frameSize = byteIntVal(header.size, 3, false);
		
		//Validate the frame size
		if(frameSize == 0 || frameSize + OLD_FRAME_HEADER_BYTE_SIZE > ID3Size)
			return FramePtr(new UnknownFrame());
		
		//Get the ID3v2.2 frame ID, and then convert it to its ID3v2.4 equivalent
		id = FrameID(terminatedstring(header.id, 4), ID3Ver);
		
		//Create the ByteArray with room for the entire frame content, if it were
		//a new ID3v2 tag
		frameBytes = ByteArray(frameSize + HEADER_BYTE_SIZE, '\0');
		musicFile->seekg(readpos, std::ifstream::beg);
		if(musicFile->fail())
			return FramePtr(new UnknownFrame(id));
		
		//Get the frame bytes, reserving the first four bytes in the ByteArray
		musicFile->read(reinterpret_cast<char*>(&frameBytes.front()+4), frameSize + OLD_FRAME_HEADER_BYTE_SIZE);
		
		//===========================================
		//Reconstruct the header as an ID3v2.4 header
		//===========================================
		
		//Convert the ID to its ID3v2.4 equivalent, and save them to the currently
		//unused first four bytes of the frame
		for(ushort i = 0; i < 4; i++)
			frameBytes[i] = static_cast<std::string>(id)[i];
		
		//Convert the ID3v2.2 non-synchsafe 3-byte frame size to the ID3v2.4
		//synchsafe 4-byte frame size
		ByteArray v4Size = intToByteArray(frameSize, 4, true);
		
		//And save it to the frame bytes
		for(ushort i = 0; i < 4; i++)
			frameBytes[i+4] = v4Size[i];
		
		//The frame should have the Discard Frame Upon Tag Alter flag
		frameBytes[8] = Frame::FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4;
		frameBytes[9] = 0;
	}
	
	//Return the Frame
	switch(frameType) {
		case FrameClass::CLASS_TEXT:
			return FramePtr(new TextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_DESCRIPTIVE:
			return FramePtr(new DescriptiveTextFrame(id, ID3Ver, frameBytes, frameOptions(id)));
		case FrameClass::CLASS_URL:
			return FramePtr(new URLTextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_PICTURE:
			return FramePtr(new PictureFrame(ID3Ver, frameBytes));
		case FrameClass::CLASS_UNKNOWN: default:
			return FramePtr(new UnknownFrame(id, ID3Ver, frameBytes));
	}
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const ulong readpos) const {
	FramePtr frame = create(readpos);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const FrameID&     frameName,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) const {
	FrameClass frameType = FrameFactory::frameType(frameName);
	
	switch(frameType) {
		case FrameClass::CLASS_TEXT:
			return FramePtr(new TextFrame(frameName, ID3Ver, textContent));
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(frameName, ID3Ver, textContent));
		case FrameClass::CLASS_DESCRIPTIVE:
			return FramePtr(new DescriptiveTextFrame(frameName,
			                                         ID3Ver,
			                                         textContent,
			                                         description,
			                                         language,
			                                         frameOptions(frameName)));
		case FrameClass::CLASS_URL:
			return FramePtr(new URLTextFrame(frameName, ID3Ver, textContent));
		default:
			return FramePtr(new UnknownFrame(frameName));
	}
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const FrameID&     frameName,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) const {
	return FramePair(frameName, create(frameName, textContent, description, language));
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::createPicture(const ByteArray&   pictureByteArray,
			                            const std::string& mimeType,
			                            const std::string& description,
			                            const PictureType  type) const {
	return FramePtr(new PictureFrame(ID3Ver,
	                                 pictureByteArray,
	                                 mimeType,
	                                 description,
	                                 type));
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPicturePair(const ByteArray&   pictureByteArray,
			                                 const std::string& mimeType,
			                                 const std::string& description,
			                                 const PictureType  type) const {
	FramePtr frame = createPicture(pictureByteArray, mimeType, description, type);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(std::ifstream& file,
                              const ulong    readpos,
                              const ushort   version,
                              const ulong    tagEnd) {
	FrameFactory factory(file, version, tagEnd);
	return factory.create(readpos);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(std::ifstream& file,
                                   const ulong    readpos,
                                   const ushort   version,
                                   const ulong    tagEnd) {
	FramePtr frame = create(file, readpos, version, tagEnd);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(const FrameID&     frameName,
                              const ushort       version,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) {
	FrameFactory factory(version);
	return factory.create(frameName, textContent, description, language);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(const FrameID&     frameName,
                                   const ushort       version,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) {
	return FramePair(frameName, create(frameName, version, textContent, description, language));
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::createPicture(const ushort       version,
                                     const ByteArray&   pictureByteArray,
			                            const std::string& mimeType,
			                            const std::string& description,
			                            const PictureType  type) {
	FrameFactory factory(version);
	return factory.createPicture(pictureByteArray, mimeType, description, type);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPicturePair(const ushort       version,
                                          const ByteArray&   pictureByteArray,
			                                 const std::string& mimeType,
			                                 const std::string& description,
			                                 const PictureType  type) {
	FramePtr frame = createPicture(version, pictureByteArray, mimeType, description, type);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory
///@static
FrameClass FrameFactory::frameType(const std::string& frameID) {
	if(frameID == "")
		return FrameClass::CLASS_UNKNOWN;
	
	switch(frameID[0]) {
		  case 'A': {
			if(frameID == "APIC")
				return FrameClass::CLASS_PICTURE;
			break;
		} case 'C': {
			if(frameID == "COMM")
				return FrameClass::CLASS_DESCRIPTIVE;
			break;
		} case 'I': {
			if(frameID == "IPLS")
				return FrameClass::CLASS_TEXT;
			break;
		} case 'T': {
			//Numerical Text Frames:
			//Year, BPM, Date, Length, Playlist Delay, Time, and Original Release Year
			//NOTE: Track and Disc are not numerical values as they may contain
			//a slash to separate the total number of tracks/discs in the set.
			if(frameID == "TYER" ||
			   frameID == "TBPM" ||
			   frameID == "TDAT" ||
			   frameID == "TLEN" ||
			   frameID == "TDLY" ||
			   frameID == "TIME" ||
			   frameID == "TORY")
				return FrameClass::CLASS_NUMERICAL;
			else if(frameID == "TXXX")
				return FrameClass::CLASS_DESCRIPTIVE;
			return FrameClass::CLASS_TEXT;
		} case 'U': {
			if(frameID == "USLT" || frameID == "USER")
				return FrameClass::CLASS_DESCRIPTIVE;
			break;
		} case 'W': {
			if(frameID == "WXXX")
				return FrameClass::CLASS_DESCRIPTIVE;
			return FrameClass::CLASS_URL;
		}
	}
	
	return FrameClass::CLASS_UNKNOWN;
}

///@pkg ID3FrameFactory
///@static
short FrameFactory::frameOptions(const std::string& frameID) {
	//These frames have a language field
	if(frameID == "USLT" || frameID == "COMM")
		return DescriptiveTextFrame::OPTION_LANGUAGE;
	//This frame always encodes the text content as Latin-1
	else if(frameID == "WXXX")
		return DescriptiveTextFrame::OPTION_LATIN1_TEXT;
	//This frame has a language field and no description
	else if(frameID == "USER")
		return DescriptiveTextFrame::OPTION_LANGUAGE | DescriptiveTextFrame::OPTION_NO_DESCRIPTION;
	else
		return 0;
}
