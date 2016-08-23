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
#include "ID3TextFrame.h"
#include "ID3PictureFrame.h"
#include "ID3Constants.h"

using namespace ID3;

//Private namespace
namespace {
	static std::string processGenre(const std::string& genre) {
		if(genre == "")
			return "";
		
		std::string genreString;
		
		if(std::all_of(genre.begin(), genre.end(), ::isdigit)) {
			genreString = V1::getGenreString(atoi(genre.c_str()));
		} else {
			//This regex matches any digit surrounded by a single pair of
			//parenthesis at the start of a string
			std::regex findV1Genre("^\\(\\d+\\)");
			std::smatch v1Genre;
			
			//If a ID3v1 genre is found
			if(std::regex_search(genre, v1Genre, findV1Genre)) {
				//Get the match
				std::string genreIntStr = v1Genre.str();
				//Get the int value of the ID3v1 genre
				int genreInt = atoi(genreIntStr.substr(1, genreIntStr.length() - 1).c_str());
				//Remove the string from the tag string
				genreString = std::regex_replace(genre, findV1Genre, "");
				//If there's nothing else in the tag string, then return
				//the ID3v1 genre
				if(genreString == "")
					genreString = V1::getGenreString(genreInt);
			} else {
				genreString = genre;
			}
		}
		
		return genreString;
	}
}

///@pkg ID3.h
Tag::Tag(std::ifstream& file) : isNull(true) {
	if(!file.is_open())
		return;
	file.seekg(0, std::ifstream::end);
	filesize = file.tellg();
	readFile(file);
}

///@pkg ID3.h
Tag::Tag(const std::string& fileLoc) : filename(fileLoc), isNull(true) {
	std::ifstream file;
	
	//Check if the file is an MP3 file
	if(!std::regex_search(fileLoc, std::regex("\\.(?:mp3|tag|mp4)$", std::regex::icase |
	                                                     std::regex::ECMAScript)))
		return;
	
	try {
		file.open(fileLoc, std::ios::binary | std::ios::ate);
		filesize = file.tellg();
		readFile(file);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::Tag(std::string&): " << e.what() << std::endl;
	}
	
	try {
		file.close();
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::Tag(std::string&) closing the file: " << e.what() << std::endl;
	}
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
bool Tag::frameExists(Frames frameName) const {
	return frames.count(getFrameName(frameName)) > 0;
}

///@pkg ID3.h
std::string Tag::textContent(Frames frameName) const {
	//Get the frame ID
	const std::string frameIDStr = getFrameName(frameName);
	
	//Check if the Frame is in the map
	const FrameMap::const_iterator result = frames.find(frameIDStr);
	if(result == frames.end()) return "";
	
	//If the frame is in the map, get it
	FramePtr frameObj = result->second;
	
	//If the frame is "null" then return an empty string
	if(frameObj->null())
		return "";
	
	//Get the text frame
	TextFrame* textFrameObj = dynamic_cast<TextFrame*>(frameObj.get());
	
	return textFrameObj == nullptr ? "" : textFrameObj->content();
}

///@pkg ID3.h
std::vector<std::string> Tag::textContents(Frames frameName) const {
	//A vector that contains a single string, for cases where there is no text
	//content.
	static std::vector<std::string> emptyString(1, "");
	
	//Check if the Frame is in the map
	const FrameMap::const_iterator result = frames.find(getFrameName(frameName));
	if(result == frames.end()) return emptyString;
	
	//If the frame is in the map, get it
	FramePtr frameObj = result->second;
	
	//If the frame is "null then return emptyString
	if(frameObj->null()) return emptyString;
	
	//Get the text frame
	TextFrame* textFrameObj = dynamic_cast<TextFrame*>(frameObj.get());
	
	//If the Frame is not a TextFrame then return an empty vector
	if(textFrameObj == nullptr) return emptyString;
	
	//The content string
	std::string textContent = textFrameObj->content();
	
	//If the string is empty, no use continuing
	if(textContent == "") return emptyString;
	
	//A vector of strings to return
	std::vector<std::string> tokens;
	
	//In ID3v2.3, only some frames are allowed to have multiple values. If the
	//requested value does not belong to one of those frames, then do not split
	//the text content.
	if(v2TagInfo.majorVer < 4) {
		switch(frameName) {
			case FRAME_COMPOSER:
			case FRAME_LYRICIST:
			case FRAME_ORIGINAL_LYRICIST:
			case FRAME_ORIGINAL_ARTIST:
			case FRAME_ARTIST: {
				break;
			} default: {
				tokens.push_back(textContent);
				return tokens;
			}
		}
	}
	
	//The string divider char
	const char DIVIDER = v2TagInfo.majorVer >= 4 ? '\0' : '/';
	
	//Loop variables
	std::size_t tokenStart = 0, tokenEnd = textContent.find(DIVIDER, 0);
	
	while((tokenEnd = textContent.find(DIVIDER, tokenStart)) != std::string::npos) {
		//If the substring isn't an empty string
		if(tokenEnd > tokenStart)
			tokens.push_back(textContent.substr(tokenStart, tokenEnd - tokenStart));
		tokenStart = tokenEnd + 1;
	}
	
	//Add the last string token, if it isn't an empty string
	if(tokenStart < textContent.size())
		tokens.push_back(textContent.substr(tokenStart));
	
	//In the edge case that the string contains only divider characters, then
	//also return the empty string vector
	if(tokens.size() == 0)
		return emptyString;
	
	return tokens;
}

///@pkg ID3.h
std::string Tag::title() const {
	return textContent(Frames::FRAME_TITLE);
}

///@pkg ID3.h
std::string Tag::genre(bool process) const {
	std::string genreString = textContent(Frames::FRAME_GENRE);
	if(process)
		genreString = processGenre(genreString);
	return genreString;
}

///@pkg ID3.h
std::vector<std::string> Tag::genres(bool process) const {
	std::vector<std::string> genreStrings = textContents(Frames::FRAME_GENRE);
	if(process)
		for(std::size_t i = 0; i < genreStrings.size(); i++)
			genreStrings[i] = processGenre(genreStrings[i]);
	return genreStrings;
}

///@pkg ID3.h
std::string Tag::artist() const {
	return textContent(Frames::FRAME_ARTIST);
}

///@pkg ID3.h
std::vector<std::string> Tag::artists() const {
	return textContents(Frames::FRAME_ARTIST);
}

///@pkg ID3.h
std::string Tag::albumArtist() const {
	return textContent(Frames::FRAME_ALBUM_ARTIST);
}

///@pkg ID3.h
std::vector<std::string> Tag::albumArtists() const {
	return textContents(Frames::FRAME_ALBUM_ARTIST);
}

///@pkg ID3.h
std::string Tag::album() const {
	return textContent(Frames::FRAME_ALBUM);
}

///@pkg ID3.h
std::vector<std::string> Tag::albums() const {
	return textContents(Frames::FRAME_ALBUM);
}

///@pkg ID3.h
std::string Tag::year() const {
	return textContent(Frames::FRAME_YEAR);
}

///@pkg ID3.h
std::string Tag::track(bool process) const {
	std::string trackString = textContent(Frames::FRAME_TRACK);
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
	std::string trackString = textContent(Frames::FRAME_TRACK);
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
	std::string discString = textContent(Frames::FRAME_DISC);
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
	std::string discString = textContent(Frames::FRAME_DISC);
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
	return textContent(Frames::FRAME_COMPOSER);
}

///@pkg ID3.h
std::vector<std::string> Tag::composers() const {
	return textContents(Frames::FRAME_COMPOSER);
}

///@pkg ID3.h
std::string Tag::bpm() const {
	return textContent(Frames::FRAME_BPM);
}

///@pkg ID3.h
Picture Tag::picture() const {
	try {
		//Get an iterator from the FrameMap
		FrameMap::const_iterator pictureItr = frames.find(getFrameName(FRAME_PICTURE));
		
		//If the Frame does not exist in the FrameMap
		if(pictureItr == frames.end()) return Picture();
		
		//Get the FramePtr
		FramePtr pictureFrameBase = pictureItr->second;
		
		//Cast it as a PictureFrame
		PictureFrame* picture = dynamic_cast<PictureFrame*>(pictureFrameBase.get());
		
		//Return a Picture struct
		return picture == nullptr ? Picture() :
		                            Picture(picture->picture(),
		                                    picture->mimeType(),
		                                    picture->description(),
		                                    picture->pictureType());
	} catch(std::out_of_range) {
		//If there is no Frames::FRAME_PICTURE Frame stored, then return an empty
		//Picture
		return Picture();
	}
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
			std::string flagString;
			if(v2TagInfo.flagUnsynchronisation) versionString += " -unsynchronisation";
			if(v2TagInfo.flagExtHeader)         versionString += " -extendedheader";
			if(v2TagInfo.flagExperimental)      versionString += " -experimental";
			if(v2TagInfo.flagFooter)            versionString += " -footer";
		}
	}
	
	return versionString;
}

///@pkg ID3.h
const bool Tag::null() const {
	return isNull;
}

///@pkg ID3.h
void Tag::print() const {
	std::cout << std::endl << "......................" << std::endl;
	if(filename == "")
		std::cout << "Printing information about ID3 File:" << std::endl;
	else
		std::cout << "Printing information about file " << filename << ":" << std::endl;
	std::cout << "Filesize:                 " << filesize << std::endl;
	std::cout << "Tag size:                 " << v2TagInfo.size << std::endl;
	std::cout << "Null:                     " << std::boolalpha << isNull << std::endl;
	
	if(isNull) {
		std::cout << ".........................." << std::endl << std::noboolalpha;
		return;
	}
	
	std::cout << "ID3 version(s) and flags: " << getVersionString(true) << std::endl;
	
	for(FramePair currentFramePair : frames) {
		std::cout << "--------------------------" << std::endl;
		currentFramePair.second->print();
	}
	
	std::cout << ".........................." << std::endl << std::noboolalpha;
}

///@pkg ID3.h
void Tag::readFile(std::ifstream& file) {
	if(!file.good())
		return;
	
	isNull = false;
	
	readFileV2(file);
	readFileV1(file);
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
		
		file.read(reinterpret_cast<char*>(&tags), V1::BYTE_SIZE);
		
		if(memcmp(tags.header, "TAG", 3) != 0)
			return;
		
		//Get the bytes for the extended tags
		if(filesize > V1::BYTE_SIZE + V1::EXTENDED_BYTE_SIZE) {
			file.seekg(-V1::BYTE_SIZE-V1::EXTENDED_BYTE_SIZE, std::ifstream::end);
			extTagsSet = !file.fail();
			if(extTagsSet) file.read(reinterpret_cast<char*>(&extTags), V1::EXTENDED_BYTE_SIZE);
			extTagsSet = memcmp(extTags.header, "TAG+", 4) == 0;
		}
		
		if(extTagsSet) setTags(extTags);
		setTags(tags);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV1(std::ifstream&): " << e.what() << std::endl;
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
		
		file.read(reinterpret_cast<char*>(&tagsHeader), HEADER_BYTE_SIZE);
		
		if(memcmp(tagsHeader.header, "ID3", 3) != 0)
			return;
		
		v2TagInfo.majorVer = tagsHeader.majorVer;
		v2TagInfo.minorVer = tagsHeader.minorVer;
		
		//Make sure the ID3v2 version is supported
		if(v2TagInfo.majorVer < MIN_SUPPORTED_VERSION ||
		   v2TagInfo.majorVer > MAX_SUPPORTED_VERSION ||
		   v2TagInfo.minorVer != SUPPORTED_MINOR_VERSION)
			return;
		
		//Get the tag flags
		v2TagInfo.size = byteIntVal(tagsHeader.size, 4, true);
		if((tagsHeader.flags & FLAG_UNSYNCHRONISATION) == FLAG_UNSYNCHRONISATION)
			v2TagInfo.flagUnsynchronisation = true;
		if((tagsHeader.flags & FLAG_EXT_HEADER) == FLAG_EXT_HEADER)
			v2TagInfo.flagExtHeader = true;
		if((tagsHeader.flags & FLAG_EXPERIMENTAL) == FLAG_EXPERIMENTAL)
			v2TagInfo.flagExperimental = true;
		if((tagsHeader.flags & FLAG_FOOTER) == FLAG_FOOTER)
			v2TagInfo.flagFooter = true;
		
		//Unsynchronization is not currently supported.
		if(v2TagInfo.size > filesize || v2TagInfo.flagUnsynchronisation)
			return;
		
		//The position to start reading from the file
		ulong frameStartPos = HEADER_BYTE_SIZE;
		
		//Skip over the extended header
		if(v2TagInfo.flagExtHeader) {
			//Seek to the position to read the extended header
			file.seekg(frameStartPos, std::ifstream::beg);
			if(file.fail()) return;
			
			//The extended header is different from ID3v2.3 and ID3v2.4.
			if(v2TagInfo.majorVer >= 4) {
				V4ExtHeader extHeader;
				
				//Verify that there's enough space
				if(frameStartPos + sizeof(V4ExtHeader) > filesize) return;
				
				//Get the extended header
				file.read(reinterpret_cast<char*>(&extHeader), sizeof(V4ExtHeader));
				
				//Increment the start position. The extended header size is synchsafe in ID3v2.4
				frameStartPos += sizeof(V4ExtHeader) + byteIntVal(extHeader.size, 4, true);
			} else {
				V3ExtHeader extHeader;
				
				//Verify that there's enough space
				if(frameStartPos + sizeof(V3ExtHeader) > filesize) return;
				
				//Get the extended header
				file.read(reinterpret_cast<char*>(&extHeader), sizeof(V3ExtHeader));
				
				//Increment the start position. The extended header size is not synchsafe in ID3v2.3
				frameStartPos += sizeof(V3ExtHeader) + byteIntVal(extHeader.size, 4, false);
			}
		}
		
		//The tag size
		v2TagInfo.totalSize = frameStartPos + v2TagInfo.size + (v2TagInfo.flagFooter ? HEADER_BYTE_SIZE : 0);
		
		//The final size check
		if(filesize < v2TagInfo.totalSize) return;
		
		//The file has correctly formatted ID3v2 tags
		tagsSet.v2 = true;
		
		//A FrameFactory to read the frames off the file
		FrameFactory factory(file, v2TagInfo.majorVer, v2TagInfo.totalSize);
		
		//Loop over the ID3 tags, and stop once all ID3 frames have been
		//reached or a frame is null. Add every frame to the frames map.
		while(frameStartPos + HEADER_BYTE_SIZE < v2TagInfo.totalSize) {
			//Create a new Frame at this position
			FramePtr frame = factory.create(frameStartPos);
			//Add the Frame to the map if it's not null
			if(!frame->null())
				frames.emplace(frame->frame(), frame);
			//If the frame content is a valid size (bigger than an ID3v2 header)
			//then continue on to the next frame. If not, then stop the loop.
			if(frame->size(true) > HEADER_BYTE_SIZE && frame->frame() != "")
				frameStartPos += frame->size(true);
			else {
				//Get the start of padding and exit the loop
				v2TagInfo.paddingStart = frameStartPos;
				break;
			}
		}
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV2(std::ifstream&): " << e.what() << std::endl;
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
		frames.emplace(FrameFactory::createPair(Frames::FRAME_TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_YEAR,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.year, 4)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_COMMENT,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.comment, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_GENRE,
		                                        v2TagInfo.majorVer,
		                                        V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::Tag&, bool): " << e.what() << std::endl;
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
		frames.emplace(FrameFactory::createPair(Frames::FRAME_TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 30)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_YEAR,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.year, 4)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_COMMENT,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.comment, 28)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_TRACK,
		                                        v2TagInfo.majorVer,
		                                        std::to_string(tags.trackNum)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_GENRE,
		                                        v2TagInfo.majorVer,
		                                        V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::P1Tag&, bool): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::ExtendedTag& tags) {
	//Save the V1 Extended tags as Frame objects.
	//Since I'm using std::unordered_map.emplace(), these will not
	//overwrite any V2 tags.
	try {
		tagsSet.v1Extended = true;
		
		frames.emplace(FrameFactory::createPair(Frames::FRAME_TITLE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.title, 60)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ARTIST,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.artist, 60)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_ALBUM,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.album, 60)));
		frames.emplace(FrameFactory::createPair(Frames::FRAME_GENRE,
		                                        v2TagInfo.majorVer,
		                                        terminatedstring(tags.genre, 30)));
		/*
		 * Placeholder comment for when I add playback speed support and
		 * support for start and end times.
		uint speed;
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
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::ExtendedTag&): " << e.what() << std::endl;
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
                          size(0),
                          totalSize(0),
                          paddingStart(0) {}
