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
#include <exception> //For exceptions
#include <cstring>   //For memcmp()
#include <regex>     //For regular expressions
#include <algorithm> //For std::all_of()

#include "ID3.h"
#include "ID3Functions.h"
#include "ID3FrameFactory.h"
#include "ID3Frame.h"

using namespace ID3;

///@pkg ID3.h
Tag::Tag(std::ifstream& file) : Tag::Tag() {
	if(!file.is_open())
		return;
	file.seekg(0, std::ifstream::end);
	filesize = file.tellg();
	readFile(file, false);
	
	const std::function<std::string (Frames)> test = std::bind(&Tag::textContent, this, Frames::TITLE);
}

///@pkg ID3.h
Tag::Tag(const std::string& fileLoc) : Tag::Tag() {
	std::ifstream file;
	
	//Check if the file is an MP3 file
	if(!std::regex_search(fileLoc, std::regex("\\.mp3$", std::regex::icase |
	                                                     std::regex::ECMAScript)))
		return;
	
	try {
		file.open(fileLoc, std::ios::binary | std::ios::ate);
		filesize = file.tellg();
		readFile(file);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag(const Glib::ustring& fileLoc): " << e.what() << std::endl;
	}
	
	std::cout << "ID3 version: " << getVersionString(true) << std::endl;
}

///@pkg ID3.h
Tag::Tag() : isNull(true) {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////  S T A R T   F R A M E ////////////////////////////
//////////////////////  G E T T E R S   &   S E T T E R S //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3.h
std::string Tag::textContent(Frames frameName) const {
	//Get the frame ID
	const std::string frameIDStr = getFrameName(frameName);
	
	//Check if the Frame is in the map
	const FrameMap::const_iterator result = frames.find(frameIDStr);
	if(result == frames.end()) return "";
	
	//If the frame is in the map, get it
	FramePtr frameObj = result->second;
	
	//If the frame is "null" or not a TextFrame then return an empty string
	if(frameObj->null())
		return "";
	
	//Get the text frame
	TextFrame* textFrameObj = dynamic_cast<TextFrame*>(frameObj.get());
	
	return textFrameObj == nullptr ? "" : textFrameObj->content();
}

///@pkg ID3.h
std::string Tag::title() const {
	return textContent(Frames::TITLE);
}

///@pkg ID3.h
std::string Tag::genre(bool process) const {
	std::string genreString = textContent(Frames::GENRE);
	if(process) {
		//This regex matches any digit surrounded by a single pair of
		//parenthesis at the start of a string
		std::regex findV1Genre("^\\(\\d+\\)");
		std::smatch v1Genre;
		
		//If a ID3v1 genre is found
		if(std::regex_search(genreString, v1Genre, findV1Genre)) {
			//Get the match
			std::string genreIntStr = v1Genre.str();
			//Get the int value of the ID3v1 genre
			int genreInt = atoi(genreIntStr.substr(1, genreIntStr.length() - 1).c_str());
			//Remove the string from the tag string
			genreString = std::regex_replace(genreString, findV1Genre, "");
			//If there's nothing else in the tag string, then return
			//the ID3v1 genre
			if(genreString.length() <= 0)
				genreString = V1::getGenreString(genreInt);
		}
	}
	return genreString;
}

///@pkg ID3.h
std::string Tag::artist() const {
	return textContent(Frames::ARTIST);
}

///@pkg ID3.h
std::string Tag::albumArtist() const {
	return textContent(Frames::ALBUMARTIST);
}

///@pkg ID3.h
std::string Tag::album() const {
	return textContent(Frames::ALBUM);
}

///@pkg ID3.h
std::string Tag::year() const {
	return textContent(Frames::YEAR);
}

///@pkg ID3.h
std::string Tag::track(bool process) const {
	std::string trackString = textContent(Frames::TRACK);
	if(process) {
		size_t slashPos = trackString.find_first_of('/');
		if(slashPos != std::string::npos) {
			trackString = trackString.substr(0, slashPos);
		}
		if(!std::all_of(trackString.begin(), trackString.end(), ::isdigit))
			return "";
	}
	return trackString;
}

///@pkg ID3.h
std::string Tag::trackTotal(bool process) const {
	std::string trackString = textContent(Frames::TRACK);
	size_t slashPos = trackString.find_first_of('/');
	if(slashPos == std::string::npos)
		return "";
	trackString = trackString.substr(slashPos + 1);
	if(process && !std::all_of(trackString.begin(), trackString.end(), ::isdigit))
			return "";
	return trackString;
}

///@pkg ID3.h
std::string Tag::disc(bool process) const {
	std::string discString = textContent(Frames::DISC);
	if(process) {
		size_t slashPos = discString.find_first_of('/');
		if(slashPos != std::string::npos) {
			discString = discString.substr(0, slashPos);
		}
		if(!std::all_of(discString.begin(), discString.end(), ::isdigit))
			return "";
	}
	return discString;
}

///@pkg ID3.h
std::string Tag::discTotal(bool process) const {
	std::string discString = textContent(Frames::DISC);
	size_t slashPos = discString.find_first_of('/');
	if(slashPos == std::string::npos)
		return "";
	discString = discString.substr(slashPos + 1);
	if(process && !std::all_of(discString.begin(), discString.end(), ::isdigit))
			return "";
	return discString;
}

///@pkg ID3.h
std::string Tag::composer() const {
	return textContent(Frames::COMPOSER);
}

///@pkg ID3.h
std::string Tag::bpm() const {
	return textContent(Frames::BPM);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  E N D   F R A M E //////////////////////////////
//////////////////////  G E T T E R S   &   S E T T E R S //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3.h
std::string Tag::getVersionString(bool verbose) const {
	std::string versionString;
	
	if(tagsSet.v1) versionString = "v1";
	else if(tagsSet.v1_1) versionString = "v1.1";
	if(tagsSet.v1Extended) versionString += " v1Extended";
	if(tagsSet.v2) {
		versionString += versionString.size() ? " " : "";
		versionString += "v2." + std::to_string(v2TagInfo.majorVer) + ".";
		versionString += std::to_string(v2TagInfo.minorVer);
		if(verbose) {
			versionString += " (" + std::to_string(v2TagInfo.size) + "B";
			if(v2TagInfo.flagUnsynchronisation) versionString += " -unsynchronisation";
			if(v2TagInfo.flagExtHeader) versionString += " -extendedheader";
			if(v2TagInfo.flagExperimental) versionString += " -experimental";
			if(v2TagInfo.flagFooter) versionString += " -footer";
			versionString += ")";
		}
	}
	
	return versionString;
}

///@pkg ID3.h
const bool Tag::null() const {
	return isNull;
}

///@pkg ID3.h
void Tag::readFile(std::ifstream& file, bool close) {
	if(!file.good())
		return;
	
	isNull = false;
	
	readFileV2(file);
	readFileV1(file);
	
	try {
		if(close) file.close();
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFile(const std::ifstream& file) closing the file: " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::readFileV1(std::ifstream& file) {
	V1::Tag tags;
	V1::ExtendedTag extTags;
	bool extTagsSet;
	
	if(filesize < V1::BYTE_SIZE)
		return;
	
	try {
		file.seekg(-V1::BYTE_SIZE, std::ifstream::end);
		if(file.fail())
			return;
		
		file.read(reinterpret_cast<char *>(&tags), V1::BYTE_SIZE);
		
		if(memcmp(tags.header, "TAG", 3) != 0)
			return;
		
		//Get the bytes for the extended tags
		if(filesize > V1::BYTE_SIZE + V1::EXTENDED_BYTE_SIZE) {
			file.seekg(-V1::BYTE_SIZE-V1::EXTENDED_BYTE_SIZE, std::ifstream::end);
			extTagsSet = !file.fail();
			if(extTagsSet) file.read(reinterpret_cast<char *>(&extTags), V1::EXTENDED_BYTE_SIZE);
			extTagsSet = memcmp(extTags.header, "TAG+", 4) == 0;
		}
		
		setTags(tags);
		if(extTagsSet) setTags(extTags);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV1(const std::ifstream& file): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::readFileV2(std::ifstream& file) {
	Header tagsHeader;
	
	if(filesize < HEADER_BYTE_SIZE)
		return;
	
	try {
		file.seekg(0, std::ifstream::beg);
		if(file.fail())
			return;
		
		file.read(reinterpret_cast<char *>(&tagsHeader), HEADER_BYTE_SIZE);
		
		if(memcmp(tagsHeader.header, "ID3", 3) != 0)
			return;
		
		v2TagInfo.majorVer = tagsHeader.majorVer;
		v2TagInfo.minorVer = tagsHeader.minorVer;
		
		//Make sure the ID3v2 version is supported
		if(v2TagInfo.majorVer < MIN_SUPPORTED_VERSION ||
		   v2TagInfo.majorVer > MAX_SUPPORTED_VERSION ||
		   v2TagInfo.minorVer != SUPPORTED_MINOR_VERSION)
			return;
		
		v2TagInfo.size = uchar_arr_binary_num(tagsHeader.size, 4, true);
		if((unsigned int)tagsHeader.flags & FLAG_UNSYNCHRONISATION == FLAG_UNSYNCHRONISATION)
			v2TagInfo.flagUnsynchronisation = true;
		if((unsigned int)tagsHeader.flags & FLAG_EXT_HEADER == FLAG_EXT_HEADER)
			v2TagInfo.flagExtHeader = true;
		if((unsigned int)tagsHeader.flags & FLAG_EXPERIMENTAL == FLAG_EXPERIMENTAL)
			v2TagInfo.flagExperimental = true;
		if((unsigned int)tagsHeader.flags & FLAG_FOOTER == FLAG_FOOTER)
			v2TagInfo.flagFooter = true;
			
		if(v2TagInfo.size > filesize)
			return;
			
		tagsSet.v2 = true;
		
		//The position to start reading from the file
		int frameStartPos = HEADER_BYTE_SIZE;
		
		//Skip over the extended header
		if(v2TagInfo.flagExtHeader) {
			//Verify that there space in the file for an extended header
			if(frameStartPos + HEADER_BYTE_SIZE > filesize)
				return;
			
			ExtHeader extHeader;
			file.seekg(frameStartPos, std::ifstream::beg);
			if(!file.fail()) {
				file.read(reinterpret_cast<char *>(&extHeader), HEADER_BYTE_SIZE);
				//Only the ID3v2.4.0 standard page says that the extended header's size is synchsafe.
				//I do not know if that is accurate or not, but I will take their word for it.
				frameStartPos += HEADER_BYTE_SIZE + uchar_arr_binary_num(extHeader.size, 4, v2TagInfo.majorVer >= 4);
			}
		}
		
		//The byte position on the file when the ID3v2 tags end
		const long ID3EndPos = frameStartPos + v2TagInfo.size;
		
		//Validate the size of the ID3v2 tag
		if(ID3EndPos > filesize)
			return;
			
		//A FrameFactory to read the frames off the file
		FrameFactory factory(file, v2TagInfo.majorVer, ID3EndPos);
		
		//Loop over the ID3 tags, and stop once all ID3 frames have been
		//reached or a frame is null. Add every frame to the frames map.
		while(frameStartPos + HEADER_BYTE_SIZE < ID3EndPos) {
			//Create a new Frame at this position
			FramePtr frame = factory.create(frameStartPos);
			//Get the byte that it ends at
			frameStartPos = frame->end();
			//Break at a null Frame
			if(frame->null()) break;
			//Add the Frame to the map
			frames.emplace(frame->frame(), frame);
		}
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV2(const std::ifstream& file): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::Tag& tags, bool zeroCheck) {
	//Check if this isn't actually a ID3v1.1 tag
	if(zeroCheck && tags.comment[28] == '\0') {
		V1::P1Tag correctID3VerTags;
		std::memcpy(&correctID3VerTags, &tags, sizeof correctID3VerTags);
		setTags(correctID3VerTags, false);
		return;
	}
	
	tagsSet.v1 = true;
	
	//Save the V1 tags as Frame objects.
	//Since I'm using std::unordered_map.emplace(), these will not
	//overwrite any V2 tags.
	try {
		frames.emplace(FrameFactory::createPair(Frames::TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 30)));
		frames.emplace(FrameFactory::createPair(Frames::ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 30)));
		frames.emplace(FrameFactory::createPair(Frames::ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 30)));
		frames.emplace(FrameFactory::createPair(Frames::YEAR,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.year, 4)));
		/*frames.emplace(FrameFactory::createPair(Frames::COMMENT,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.comment, 30)));*/
		frames.emplace(FrameFactory::createPair(Frames::GENRE,
		                                        v2TagInfo.majorVer,
		                                        V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1Tag& tags, bool zeroCheck): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::P1Tag& tags, bool zeroCheck) {
	//Check that this isn't actually an ID3v1 tag
	if(zeroCheck && tags.zero != '\0') {
		V1::Tag correctID3VerTags;
		std::memcpy(&correctID3VerTags, &tags, sizeof correctID3VerTags);
		setTags(correctID3VerTags, false);
		return;
	}
	
	tagsSet.v1_1 = true;
	
	//Save the V1.1 tags as Frame objects.
	//Since I'm using std::unordered_map.emplace(), these will not
	//overwrite any V2 tags.
	try {
		frames.emplace(FrameFactory::createPair(Frames::TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 30)));
		frames.emplace(FrameFactory::createPair(Frames::ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 30)));
		frames.emplace(FrameFactory::createPair(Frames::ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 30)));
		frames.emplace(FrameFactory::createPair(Frames::YEAR,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.year, 4)));
		/*frames.emplace(FrameFactory::createPair(Frames::COMMENT,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.comment, 28)));*/
		frames.emplace(FrameFactory::createPair(Frames::TRACK,
		                                        v2TagInfo.majorVer,
		                                        std::to_string(tags.trackNum)));
		frames.emplace(FrameFactory::createPair(Frames::GENRE,
		                                        v2TagInfo.majorVer,
		                                        V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1_1Tag& tags, bool zeroCheck): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::ExtendedTag& tags) {
	//Save the V1 Extended tags as Frame objects.
	//Since I'm using std::unordered_map.emplace(), these will not
	//overwrite any V2 tags.
	try {
		tagsSet.v1Extended = true;
		
		frames.emplace(FrameFactory::createPair(Frames::TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 60)));
		frames.emplace(FrameFactory::createPair(Frames::ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 60)));
		frames.emplace(FrameFactory::createPair(Frames::ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 60)));
		frames.emplace(FrameFactory::createPair(Frames::GENRE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.genre, 30)));
		/*
		 * Placeholder comment for when I add playback speed support and
		 * support for start and end times.
		unsigned int speed;
		startTime = atoi(tags.startTime);
		endTime = atoi(tags.endTime);
		speed = tags.speed;
		
		switch(speed) {
			case 1: { playbackSpeed = V1::ExtendedSpeeds::SLOW; break; }
			case 2: { playbackSpeed = V1::ExtendedSpeeds::MEDIUM; break; }
			case 3: { playbackSpeed = V1::ExtendedSpeeds::FAST; break; }
			case 4: { playbackSpeed = V1::ExtendedSpeeds::HARDCORE; break; }
			case 0: default: { playbackSpeed = V1::ExtendedSpeeds::UNSET; break; }
		}*/
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1ExtendedTag& tags): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
Tag::TagsOnFile::TagsOnFile() : v1(false),
                                v1_1(false),
                                v1Extended(false),
                                v2(false) {}

///@pkg ID3.h
Tag::TagInfo::TagInfo() : majorVer(-1),
                          minorVer(-1),
                          flagUnsynchronisation(false),
                          flagExtHeader(false),
                          flagExperimental(false),
                          flagFooter(false),
                          size(-1) {}
