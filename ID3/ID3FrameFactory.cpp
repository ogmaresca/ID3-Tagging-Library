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
	//TODO: Process frame flags
	
	//Validate the file
	if(readpos + HEADER_BYTE_SIZE > ID3Size ||
	   musicFile == nullptr ||
	   !musicFile->is_open())
		return FramePtr(new UnknownFrame());
	
	FrameHeader header;
	long frameSize, endpos;
	std::string id;
	
	//Seek to the read position
	musicFile->seekg(readpos, std::ifstream::beg);
	if(musicFile->fail()) return FramePtr(new UnknownFrame());
	
	//Read the frame header
	musicFile->read(reinterpret_cast<char*>(&header), HEADER_BYTE_SIZE);
	
	//Get the size of the frame
	frameSize = uchar_arr_binary_num(header.size, 4, ID3Ver >= 4);
	endpos = readpos + frameSize + HEADER_BYTE_SIZE;
	
	//Validate the frame size
	if(frameSize == 0 || endpos > ID3Size)
		return FramePtr(new UnknownFrame());
	
	//Get the frame ID
	id = terminatedstring(header.id, 4);
	
	//Get the class the Frame should be
	FrameClass frameType = FrameFactory::frameType(id);
	
	//Create a ByteArray with the entire frame contents
	ByteArray frameBytes(frameSize + HEADER_BYTE_SIZE);
	musicFile->seekg(readpos, std::ifstream::beg);
	musicFile->read(&frameBytes.front(), frameSize + HEADER_BYTE_SIZE);
	
	//Return the Frame
	switch(frameType) {
		case FrameClass::TEXT: return FramePtr(new TextFrame(id, ID3Ver, frameBytes, endpos));
		case FrameClass::NUMERICAL: return FramePtr(new NumericalTextFrame(id, ID3Ver, frameBytes, endpos));
		case FrameClass::UNKNOWN: default: return FramePtr(new UnknownFrame(id, ID3Ver, frameBytes, endpos));
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
                              const std::string& description) const {
	if(frameType(frameName) != FrameClass::TEXT)
		return FramePtr(new UnknownFrame(frameName));
	return FramePtr(new TextFrame(frameName, ID3Ver, textContent, description));
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const Frames frameName,
                              const std::string& textContent,
                              const std::string& description) const {
	return create(getFrameName(frameName), textContent, description);
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const std::string& frameName,
                                   const std::string& textContent,
                                   const std::string& description) const {
	return FramePair(frameName, create(frameName, textContent, description));
}

///@pkg ID3FrameFactory.h
FramePair FrameFactory::createPair(const Frames frameName,
                                   const std::string& textContent,
                                   const std::string& description) const {
	FramePtr frame = create(frameName, textContent, description);
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
                              const std::string& description) {
	FrameFactory factory(version);
	return factory.create(frameName, textContent, description);
}

///@pkg ID3FrameFactory.h
///@static
FramePtr FrameFactory::create(const Frames frameName,
                              const unsigned long version,
                              const std::string& textContent,
                              const std::string& description) {
	return create(getFrameName(frameName), version, textContent, description);
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(const std::string& frameName,
                                   const unsigned long version,
                                   const std::string& textContent,
                                   const std::string& description) {
	return FramePair(frameName, create(frameName, version, textContent, description));
}

///@pkg ID3FrameFactory.h
///@static
FramePair FrameFactory::createPair(const Frames frameName,
                                   const unsigned long version,
                                   const std::string& textContent,
                                   const std::string& description) {
	FramePtr frame = create(frameName, version, textContent, description);
	return FramePair(frame->frame(), frame);
}

///@pkg ID3FrameFactory
///@static
FrameClass FrameFactory::frameType(const std::string& frameID) {
	if(frameID == "")
		return FrameClass::UNKNOWN;
	
	switch(frameID[0]) {
		case 'T': {
			//Numerical Text Frames:
			//Year, BPM, Date, Length, Playlist Delay, Time
			//NOTE: Track and Disc are not numerical values as they may contain
			//a slash to separate the total number of tracks/discs in the set.
			if(frameID == "TYER" ||
			   frameID == "TBPM" ||
			   frameID == "TDAT" ||
			   frameID == "TLEN" ||
			   frameID == "TDLY" ||
			   frameID == "TIME")
				return FrameClass::NUMERICAL;
			return FrameClass::TEXT;
		} case 'C': {
			if(frameID == "COMM")
				return FrameClass::TEXT;
			break;
		}
	}
	
	return FrameClass::UNKNOWN;
}
