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
#include "ID3Functions.h"

using namespace ID3;

///@pkg ID3FrameFactory.h
FrameFactory::FrameFactory(std::ifstream& file,
                           const unsigned int version,
                           const unsigned long tagSize) : musicFile(&file),
                                                          ID3Ver(version),
                                                          ID3Size(tagSize) {}

///@pkg ID3FrameFactory.h	                                              
FrameFactory::FrameFactory(const unsigned int version) : musicFile(nullptr),
                                                         ID3Ver(version),
                                                         ID3Size(0) {}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const unsigned long readpos) const {
	//Validate the file
	if(readpos + HEADER_BYTE_SIZE > ID3Size ||
	   musicFile == nullptr ||
	   !musicFile->is_open())
		return FramePtr(new UnknownFrame());
	
	FrameHeader header;
	long frameSize;
	std::string id;
	
	//Seek to the read position
	musicFile->seekg(readpos, std::ifstream::beg);
	if(musicFile->fail()) return FramePtr(new UnknownFrame());
	
	//Read the frame header
	musicFile->read(reinterpret_cast<char*>(&header), HEADER_BYTE_SIZE);
	
	//Get the size of the frame
	frameSize = byteIntVal(header.size, 4, ID3Ver >= 4);
	
	//Validate the frame size
	if(frameSize == 0 || frameSize + HEADER_BYTE_SIZE > ID3Size)
		return FramePtr(new UnknownFrame());
	
	//Get the frame ID
	id = terminatedstring(header.id, 4);
	
	//Get the class the Frame should be
	FrameClass frameType = FrameFactory::frameType(id);
	
	//Create a ByteArray with the entire frame contents
	ByteArray frameBytes(frameSize + HEADER_BYTE_SIZE);
	musicFile->seekg(readpos, std::ifstream::beg);
	if(musicFile->fail())
		return FramePtr(new UnknownFrame(id));
	musicFile->read(&frameBytes.front(), frameSize + HEADER_BYTE_SIZE);
	
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
		case FrameClass::CLASS_UNKNOWN: default:
			return FramePtr(new UnknownFrame(id, ID3Ver, frameBytes));
	}
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const unsigned long readpos) const {
	FramePtr frame = create(readpos);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const std::string& frameName,
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
FramePtr FrameFactory::create(const Frames frameName,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) const {
	return create(getFrameName(frameName), textContent, description, language);
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const std::string& frameName,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) const {
	return FramePair(frameName, create(frameName, textContent, description, language));
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const Frames frameName,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) const {
	FramePtr frame = create(frameName, textContent, description, language);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(std::ifstream& file,
                              const unsigned long readpos,
                              const unsigned int version,
                              const unsigned long tagSize) {
	FrameFactory factory(file, version, tagSize);
	return factory.create(readpos);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(std::ifstream& file,
                                   const unsigned long readpos,
                                   const unsigned int version,
                                   const unsigned long tagSize) {
	FramePtr frame = create(file, readpos, version, tagSize);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(const std::string& frameName,
                              const unsigned long version,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) {
	FrameFactory factory(version);
	return factory.create(frameName, textContent, description, language);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(const Frames frameName,
                              const unsigned long version,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) {
	return create(getFrameName(frameName), version, textContent, description, language);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(const std::string& frameName,
                                   const unsigned long version,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) {
	return FramePair(frameName, create(frameName, version, textContent, description, language));
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(const Frames frameName,
                                   const unsigned long version,
                                   const std::string& textContent,
                                   const std::string& description,
                                   const std::string& language) {
	FramePtr frame = create(frameName, version, textContent, description, language);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory
///@static
FrameClass FrameFactory::frameType(const std::string& frameID) {
	if(frameID == "")
		return FrameClass::CLASS_UNKNOWN;
	
	switch(frameID[0]) {
		case 'T': {
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
		} case 'C': {
			if(frameID == "COMM")
				return FrameClass::CLASS_DESCRIPTIVE;
			break;
		} case 'W': {
			if(frameID == "WXXX")
				return FrameClass::CLASS_DESCRIPTIVE;
			return FrameClass::CLASS_URL;
		} case 'I': {
			if(frameID == "IPLS")
				return FrameClass::CLASS_TEXT;
			break;
		}  case 'U': {
			if(frameID == "USLT" || frameID == "USER")
				return FrameClass::CLASS_DESCRIPTIVE;
			break;
		}
	}
	
	return FrameClass::CLASS_UNKNOWN;
}

///@pkg ID3FrameFactory
///@static
short FrameFactory::frameOptions(const std::string& frameID) {
	if(frameID == "USLT" || frameID == "COMM")
		return DescriptiveTextFrame::OPTION_LANGUAGE;
	else if(frameID == "WXXX")
		return DescriptiveTextFrame::OPTION_LATIN1_TEXT;
	else
		return 0;
}
