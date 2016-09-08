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
#include <cstring>   //For memcmp()
#include <regex>     //For regular expressions

#include "ID3.hpp"                      //For the Tag class definition
#include "ID3Functions.hpp"             //For assorted functions
#include "ID3FrameFactory.hpp"          //For FrameFactory
#include "Frames/ID3TextFrame.hpp"      //For TextFrame
#include "Frames/ID3PictureFrame.hpp"   //For PictureFrame
#include "Frames/ID3PlayCountFrame.hpp" //For PlayCountFrame
#include "ID3Constants.hpp"             //For constants such as HEADER_BYTE_SIZE
#include "ID3Exception.hpp"             //For exceptions

using namespace ID3;

//Private namespace
namespace {
	/**
	 * Process a genre tag string.
	 * 
	 * @see ID3::Tag::genre(bool)
	 */
	static std::string processGenre(const std::string& genre) {
		if(genre.empty()) return "";
		std::string genreString;
		
		if(numericalString(genre)) {
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
				if(genreString.empty()) genreString = V1::getGenreString(genreInt);
			} else { genreString = genre; }
		}
		
		return genreString;
	}
	
	/**
	 * Check if a file is a valid MP3 or MP4 file.
	 * 
	 * @param fileLoc The file location.
	 * @throws NotMP3FileException if the file location is not valid.
	 */
	static void validateFileLocation(const std::string& fileLoc) {
		//Check if the file is an MP3 file
		if(!std::regex_search(fileLoc, std::regex("\\.(?:mp3|tag|mp4)$", std::regex::icase|std::regex::ECMAScript)))
			throw NotMP3FileException("File \"" + fileLoc + "\" is not an MP3 or MP4 file!\n");
	}
}

///@pkg ID3.h
Tag::Tag(const std::string& fileLoc) : Tag(fileLoc, true) {}

///@pkg ID3.h
Tag::Tag(const std::string& fileLoc, const bool readFrames) : filename(fileLoc), filesize(0) {
	validateFileLocation(fileLoc); //Throws NotMP3FileException
	
	std::ifstream file(fileLoc, std::ios::in | std::ios::binary | std::ios::ate);
	
	if(file.is_open()) {
		readFile(file, readFrames);
		file.close();
	} else {
		throw FileNotFoundException("File \"" + filename + "\" cannot be opened!\n");
	}
}

///@pkg ID3.h
Tag::Tag() noexcept : filesize(0) {}

///@pkg ID3.h
Tag::operator bool() const noexcept { return !frames.empty(); }

///@pkg ID3.h
bool Tag::operator!() const noexcept { return frames.empty(); }

///@pkg ID3.h
void Tag::write(const std::string& fileLoc,
                const float        paddingFactor,
			       const bool         setFileNameUponSuccess,
			       const bool         discardNonCoverPictures,
			       const bool         discardUnknown) {
	if(!setFileNameUponSuccess) filename = fileLoc;
	validateFileLocation(fileLoc); //Throws NotMP3FileException
	
	//A newly-constructed Tag of the file, to get the most up-to-date file information
	const Tag fileInfo(fileLoc, false);
	
	std::fstream file(fileLoc, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if(!file.is_open())
		throw FileNotFoundException("File \"" + fileLoc + "\" cannot be opened!\n");
	
	//The ID3v2 tag data to write to file
	ByteArray binaryTagData(10, '\0');
	//Make the tags at least one KiB long
	binaryTagData.reserve(fileInfo.v2TagInfo.totalSize > 1024 ? fileInfo.v2TagInfo.totalSize : 1024);
	
	//Add "ID3"
	binaryTagData[0] = 'I';
	binaryTagData[1] = 'D';
	binaryTagData[2] = '3';
	
	//Add the version
	binaryTagData[3] = WRITE_VERSION;
	binaryTagData[4] = SUPPORTED_MINOR_VERSION;
	
	//Byte 5 is the flag, which is already initialized to 0. No flags are being set.
	//Bytes 6-9 are the size, and have already been intialized to 0.
	
	//Loop through every Frame and write it
	bool foundCoverPicture = false;
	for(const FramePair& framePair : frames) {
		//Ignore null and empty Frames
		if(framePair.second.get() == nullptr || framePair.second->null() || framePair.second->empty()) continue;
		//Delete non-conforming pictures if discardNonCoverPictures is true
		if(discardNonCoverPictures && dynamic_cast<PictureFrame*>(framePair.second.get()) != nullptr) {
			if(!foundCoverPicture && dynamic_cast<PictureFrame*>(framePair.second.get())->pictureType() == PictureType::FRONT_COVER)
				foundCoverPicture = true;
			else
				continue;
		}
		//Delete unknown frames if discardUnknown is true
		if(discardUnknown && dynamic_cast<UnknownFrame*>(framePair.second.get()) != nullptr) continue;
		
		ByteArray frameBytes = framePair.second->write();
		//If the Frame data is valid add the it to the tag data
		if(frameBytes.size() > HEADER_BYTE_SIZE)
			binaryTagData.insert(binaryTagData.end(), frameBytes.begin(), frameBytes.end());
	}
	
	//Whether the file needs to be completely rewritten
	bool needToRewriteFile = fileInfo.tagsSet.v1 || fileInfo.tagsSet.v1_1 || !fileInfo.tagsSet.v2 ||
	                         binaryTagData.size() > fileInfo.v2TagInfo.totalSize;
	
	//Reset the v2 tag info
	v2TagInfo = TagInfo();
	v2TagInfo.majorVer = WRITE_VERSION;
	v2TagInfo.majorVer = SUPPORTED_MINOR_VERSION;
	v2TagInfo.paddingStart = binaryTagData.size();
	
	//If the data is smaller than the file's tag size, then extend it with padding
	if(!needToRewriteFile) {
		//Ignore case where tag data size == file tag size
		if(binaryTagData.size() < fileInfo.v2TagInfo.totalSize) {		
			ByteArray padding(fileInfo.v2TagInfo.totalSize - binaryTagData.size(), '\0');
			
			//This check if just being overly cautious, probably not necessary
			if(binaryTagData.size() + padding.size() < MAX_TAG_SIZE)
				binaryTagData.insert(binaryTagData.end(), padding.begin(), padding.end());
			else
				needToRewriteFile = true;
		}
	} else if(paddingFactor > 0.0) { //Append padding
		//Get the padding size, then round it up to the next highest multiple of 4096.
		const ulong factorMult  = binaryTagData.size() + (binaryTagData.size() * paddingFactor),
		            paddingSize = (factorMult + (4096 - (factorMult % 4096))) - binaryTagData.size();
		
		ByteArray padding(paddingSize, '\0');
		if(binaryTagData.size() + padding.size() < MAX_TAG_SIZE)
				binaryTagData.insert(binaryTagData.end(), padding.begin(), padding.end());
	}
	
	//Validate the size by throwing a TagSizeException if it's too big
	if(binaryTagData.size() - HEADER_BYTE_SIZE > MAX_TAG_SIZE)
		throw TagSizeException("Cannot write tags to file \""+fileLoc+"\", as it exceeds the maximum size of "+std::to_string(MAX_TAG_SIZE)+"!\n");
	
	//Save the frame size
	ByteArray sizeBytes = intToByteArray(binaryTagData.size() - HEADER_BYTE_SIZE, 4, true);
	for(ushort i = 0; i < 4; i++) binaryTagData[i+6] = sizeBytes[i];
	v2TagInfo.size = binaryTagData.size() - HEADER_BYTE_SIZE;
	v2TagInfo.totalSize = binaryTagData.size();
	
	if(needToRewriteFile) {
		//Rewrite the file to accomodate the bigger tags/removed ID3v1 tags.
		            //The start of the audio data in the file
		const ulong AUDIO_START = fileInfo.tagsSet.v2 ? fileInfo.v2TagInfo.totalSize : 0,
		            //The end of the audio data in the file
		            AUDIO_END = fileInfo.filesize -
		                        (fileInfo.tagsSet.v1 || fileInfo.tagsSet.v1_1 ? V1::BYTE_SIZE : 0) -
		                        (fileInfo.tagsSet.v1Extended ? V1::EXTENDED_BYTE_SIZE : 0);
		
		//This will probably never be true, but you can never be too careful
		if(AUDIO_END < AUDIO_START)
			throw FileFormatException("Cannot write tags to file \""+fileLoc+"\", ID3v1 and ID3v2 tags overlap on file.");
		
		//Create a ByteArray of the music file
		ByteArray binaryAudioData(AUDIO_END - AUDIO_START, '\0');
		
		//Seek to the audio start and read the audio
		file.seekg(AUDIO_START, std::ios_base::beg);
		if(!file) throw WriteException("Cannot write tags to file \""+fileLoc+"\", error seeking on file.");
		file.read(reinterpret_cast<char*>(&binaryAudioData.front()), binaryAudioData.size());
		
		//Close the file, and re-open it truncated
		file.close();
		file.open(fileLoc, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		if(!file.is_open()) throw WriteException("Cannot write tags to file \""+fileLoc+"\", unable to open file in write mode.");
		
		//Write the tags
		file.seekp(0, std::ios_base::beg);
		file.write(reinterpret_cast<char*>(&binaryTagData.front()), binaryTagData.size());
		
		//Write the audio
		file.seekp(0, std::ios_base::end);
		file.write(reinterpret_cast<char*>(&binaryAudioData.front()), binaryAudioData.size());
	} else {
		//Overwrite the existing ID3v2 tags
		//Seek to the beginning
		file.seekp(0, std::ios_base::beg);
		
		//Write the tags
		file.write(reinterpret_cast<char*>(&binaryTagData.front()), binaryTagData.size());
	}
	
	//Now that the write has been successful, remove any null/empty frames
	foundCoverPicture = false;
	auto itr = frames.begin();
	while(itr != frames.end()) {
		//Delete null and empty Frames
		if(itr->second.get() == nullptr || itr->second->null() || itr->second->empty()) {
			itr = frames.erase(itr);
		} else if(discardNonCoverPictures && dynamic_cast<PictureFrame*>(itr->second.get()) != nullptr) {
			//Delete non-conforming pictures if discardNonCoverPictures is true
			if(!foundCoverPicture && dynamic_cast<PictureFrame*>(itr->second.get())->pictureType() == PictureType::FRONT_COVER) {
				foundCoverPicture = true;
				itr++;
			} else {
				itr = frames.erase(itr);
			}
		} else if(discardUnknown && dynamic_cast<UnknownFrame*>(itr->second.get()) != nullptr) {
			itr = frames.erase(itr); //Delete unknown frames if discardUnknown is true
		}else {
			itr++;
		}
	}
	
	//Close the file
	file.close();
	if(setFileNameUponSuccess) filename = fileLoc;
	tagsSet.v1 = false, tagsSet.v1_1 = false, tagsSet.v1Extended = false;
}

///@pkg ID3.h
void Tag::revert() {
	//Loop through every Frame and revert it
	auto itr = frames.begin();
	while(itr != frames.end()) {
		if(itr->second.get() == nullptr) { itr = frames.erase(itr); continue; }
		itr->second->revert();
		//If the Frame is null or empty then remove it
		if(itr->second->null() || itr->second->empty()) itr = frames.erase(itr);
		else                                            itr++;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////  S T A R T   F R A M E   G E T T E R S ////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3.h
bool Tag::exists(const FrameID& frameName) const { return frames.count(frameName) > 0; }

///@pkg ID3.h
std::string Tag::textString(const FrameID& frameName) const {
	TextFrame* textFrameObj = getFrame<TextFrame>(frameName); //Get the frame
	
	//If the Frame object isn't valid and thus a null pointer, then return an
	//empty string. Else, return the text content.
	return textFrameObj == nullptr ? "" : textFrameObj->content();
}

///@pkg ID3.h
std::vector<std::string> Tag::textStrings(const FrameID& frameName) const {
	if(frameName.allowsMultiple()) {
		//Get the text frames
		std::vector<TextFrame*> textFrames = getFrames<TextFrame>(frameName);
		
		std::vector<std::string> textStrings; //The vector to return
		
		if(textFrames.size() == 0) {
			textStrings.push_back(""); //If no frames were returned
		} else {
			textStrings.reserve(textFrames.size()); //Reserve slots for each TextFrame
			for(TextFrame* currentFrame : textFrames) //Add the frame contents
				textStrings.push_back(currentFrame->content());
		}
		return textStrings;
	} else {
		TextFrame* textFrame = getFrame<TextFrame>(frameName);
		
		return textFrame == nullptr ?
		       //If no TextFrame exists with that frame ID
		       std::vector<std::string>(1, "") :
		       //If there is a TextFrame, then return its contents
		       textFrame->contents();
	}
}

///@pkg ID3.h
Text Tag::text(const FrameID& frameName) const { return getTextStruct(getFrame<TextFrame>(frameName)); }

///@pkg ID3.h
Text Tag::text(const FrameID& frameName,
               const std::function<bool (const std::string&, const std::string&)>& filterFunc) const {
	//If multiple instances of the frame are not allowed, then ignore the filter function
	if(!frameName.allowsMultiple()) return text(frameName);
	
	//Get the Frame vector
	const std::vector<DescriptiveTextFrame*> descTextFrames = getFrames<DescriptiveTextFrame>(frameName);
	
	//If it allows multiple instances of the frame, but there are no DescriptiveTextFrame's,
	//then try ignoring the filter function
	if(descTextFrames.size() == 0) return text(frameName);
	
	for(const DescriptiveTextFrame* const currentFrame : descTextFrames)
		//Test the frame's description and language with the given filter function
		if(filterFunc(currentFrame->description(), currentFrame->language()))
			return getTextStruct(currentFrame);
	
	return Text(); //No matching Frame found
}

///@pkg ID3.h
std::vector<Text> Tag::texts(const FrameID& frameName) const {
	//Get the Frame vector
	std::vector<TextFrame*> textFrames = getFrames<TextFrame>(frameName);
	
	//If no relevant text frames were found, return one with an empty Text
	if(textFrames.size() == 0) return std::vector<Text>(1, Text());
	
	std::vector<Text> toReturn;          //The vector to return
	toReturn.reserve(textFrames.size()); //Reserve slots for each TextFrame
	
	//Loop through every text frame and add a Text struct for each Frame
	for(TextFrame* currentFrame : textFrames)
		toReturn.push_back(getTextStruct(currentFrame));
	return toReturn; //Return the Text vector
}

///@pkg ID3.h
ByteArray Tag::binaryData(const FrameID& frameName) const {
	Frame* frame = getFrame<Frame>(frameName);
	return frame == nullptr ? ByteArray() : frame->bytes();
}

///@pkg ID3.h
std::vector<ByteArray> Tag::binaryDatas(const FrameID& frameName) const {
	std::vector<Frame*> frames = getFrames<Frame>(frameName);
	std::vector<ByteArray> toReturn;
	if(frames.size() > 0) {
		toReturn.reserve(frames.size());
		for(const Frame* const frame : frames) toReturn.push_back(frame->bytes());
	}
	return toReturn;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////  E N D   F R A M E   G E T T E R S //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////  S T A R T   F R A M E   S E T T E R S ////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3.h
void Tag::text(const FrameID& frameID, const Text& text) {
	//Get the text frame. If a UnknownFrame exists at the position, delete it.
	TextFrame* textFrameObj = getFrame<TextFrame>(frameID, true);
	
	if(textFrameObj == nullptr) {
		//If the Frame doesn't exist, create it
		//If the FrameID shouldn't be a TextFrame, then this will be ignored.
		addFrame(frameID, factory.create(frameID, text.text, text.description, text.language));
	} else {		
		//See if it's a DescriptiveTextFrame
		DescriptiveTextFrame* descFrameObj = getFrame<DescriptiveTextFrame>(frameID);
		//Set the content
		if(descFrameObj == nullptr) textFrameObj->content(text.text);
		else                        descFrameObj->content(text.text, text.description, text.language);
	}
}

///@pkg ID3.h
void Tag::text(const FrameID& frameID, const std::string& text) {
	//Get the text frame. If a UnknownFrame exists at the position, delete it.
	TextFrame* textFrameObj = getFrame<TextFrame>(frameID, true);
	
	//If the Frame doesn't exist, create it
	//If the FrameID shouldn't be a TextFrame, then this will be ignored.
	if(textFrameObj == nullptr) addFrame(frameID, factory.create(frameID, text));
	else                        textFrameObj->content(text); //Set the content
}

///@pkg ID3.h
void Tag::text(const FrameID& frameID, const std::vector<std::string>& text) {
	//Get the text frame. If a UnknownFrame exists at the position, delete it.
	TextFrame* textFrameObj = getFrame<TextFrame>(frameID, true);
	
	//If the Frame doesn't exist, create it
	//If the FrameID shouldn't be a TextFrame, then this will be ignored.
	if(textFrameObj == nullptr) addFrame(frameID, factory.create(frameID, text));
	else                        textFrameObj->contents(text); //Set the content
}

///@pkg ID3.h
void Tag::text(const FrameID&                           frameID,
			      const Text&                              text,
			      const std::function<bool (const Text&)>& filterFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	ulong hits = 0;
	for(TextFrame* currentFrame : frameVector) {
		if(filterFunc(getTextStruct(currentFrame))) {
			hits++;
			DescriptiveTextFrame* descFrameObj = dynamic_cast<DescriptiveTextFrame*>(currentFrame);
			if(descFrameObj == nullptr) currentFrame->content(text.text);
			else                        descFrameObj->content(text.text, text.description, text.language);
		}
	}
	//If no frame matched, try to create a new frame
	if(!hits) addFrame(frameID, factory.create(frameID, text.text, text.description, text.language));
}

///@pkg ID3.h
void Tag::text(const FrameID& frameID,
			      const Text&    text,
			      const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	ulong hits = 0;
	for(TextFrame* currentFrame : frameVector) {
		Text frameText = getTextStruct(currentFrame);
		if(filterFunc(frameText.text, frameText.description, frameText.language)) {
			hits++;
			DescriptiveTextFrame* descFrameObj = dynamic_cast<DescriptiveTextFrame*>(currentFrame);
			if(descFrameObj == nullptr) currentFrame->content(text.text);
			else                        descFrameObj->content(text.text, text.description, text.language);
		}
	}
	//If no frame matched, try to create a new frame
	if(!hits) addFrame(frameID, factory.create(frameID, text.text, text.description, text.language));
}

///@pkg ID3.h
void Tag::text(const FrameID&                           frameID,
			      const std::function<Text (const Text&)>& transformFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	for(TextFrame* currentFrame : frameVector) {
		Text text = transformFunc(getTextStruct(currentFrame));
		DescriptiveTextFrame* descFrameObj = dynamic_cast<DescriptiveTextFrame*>(currentFrame);
		if(descFrameObj == nullptr) currentFrame->content(text.text);
		else                        descFrameObj->content(text.text, text.description, text.language);
	}
}

///@pkg ID3.h
void Tag::text(const FrameID&                           frameID,
			      const std::string&                       text,
			      const std::function<bool (const Text&)>& filterFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	ulong hits = 0;
	for(TextFrame* currentFrame : frameVector) {
		if(filterFunc(getTextStruct(currentFrame))) {
			hits++;
			currentFrame->content(text);
		}
	}
	//If no frame matched, try to create a new frame
	if(!hits) addFrame(frameID, factory.create(frameID, text));
}

///@pkg ID3.h
void Tag::text(const FrameID&     frameID,
			      const std::string& text,
			      const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	ulong hits = 0;
	for(TextFrame* currentFrame : frameVector) {
		Text frameText = getTextStruct(currentFrame);
		if(filterFunc(frameText.text, frameText.description, frameText.language)) {
			hits++;
			currentFrame->content(text);
		}
	}
	//If no frame matched, try to create a new frame
	if(!hits) addFrame(frameID, factory.create(frameID, text));
}

///@pkg ID3.h
void Tag::text(const FrameID&  frameID,
			      const std::function<std::string (const std::string&, const std::string&, const std::string&)>& transformFunc) {
	std::vector<TextFrame*> frameVector = getFrames<TextFrame>(frameID);
	for(TextFrame* currentFrame : frameVector) {
		Text frameText = getTextStruct(currentFrame);
		std::string text = transformFunc(frameText.text, frameText.description, frameText.language);
		currentFrame->content(text);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////  E N D   F R A M E   S E T T E R S //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////  S T A R T   S P E C I F I C   F R A M E ///////////////////
//////////////////////  G E T T E R S   &   S E T T E R S //////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3.h
std::string Tag::genre(bool process) const {
	std::string genreString = textString(Frames::FRAME_GENRE);
	return process ? processGenre(genreString) : genreString;
}
///@pkg ID3.h
std::vector<std::string> Tag::genres(bool process) const {
	std::vector<std::string> genreStrings = textStrings(Frames::FRAME_GENRE);
	if(process) for(std::string& genre : genreStrings) genre = processGenre(genre);
	return genreStrings;
}
///@pkg ID3.h
void Tag::genre(const ushort newGenre) { text(FRAME_GENRE, V1::getGenreString(newGenre)); }

///@pkg ID3.h
void Tag::year(const std::string& newYear) {
	static const ushort YEAR_LENGTH = 4;
	//First chop off any additional characters
	std::string yearString = newYear.substr(0, YEAR_LENGTH);
	if(!yearString.empty() && numericalString(yearString)) {
		//Prepend zeros to make it four characters long
		while(yearString.size() < YEAR_LENGTH) yearString = '0' + yearString;
		const std::string tdrc = textString(FRAME_RECORDING_TIME);
		//If there is more to the TDRC than the year, preserve it
		if(tdrc.size() > YEAR_LENGTH) text(FRAME_RECORDING_TIME, yearString+tdrc.substr(YEAR_LENGTH+1));
		else                          text(FRAME_RECORDING_TIME, yearString);
	} else {
		yearString = "";
		text(FRAME_RECORDING_TIME, yearString);
	}
	text(FRAME_YEAR, yearString);
}

///@pkg ID3.h
std::string Tag::track() const {
	std::string trackString = textString(Frames::FRAME_TRACK);
	return trackString.substr(0, trackString.find_first_of('/'));
}
///@pkg ID3.h
std::string Tag::trackTotal() const {
	std::string trackString = textString(Frames::FRAME_TRACK);
	size_t slashPos = trackString.find_first_of('/');
	return slashPos == std::string::npos ? "" : trackString.substr(slashPos + 1);
}
///@pkg ID3.h
void Tag::track(const std::string& newTrack) {
	std::string	trackString = numericalString(newTrack) ? newTrack : "",
	            total       = trackTotal();
	text(FRAME_TRACK, total.empty() ? trackString : trackString + '/' + total);
}
///@pkg ID3.h
void Tag::trackTotal(const std::string& newTrackTotal) {
	std::string	trackString = numericalString(newTrackTotal) ? newTrackTotal : "";
	if(trackString.empty()) text(FRAME_TRACK, track());
	else                    text(FRAME_TRACK, track() + '/' + trackString);
}

///@pkg ID3.h
std::string Tag::disc() const {
	std::string discString = textString(Frames::FRAME_DISC);
	return discString.substr(0, discString.find_first_of('/'));
}
///@pkg ID3.h
std::string Tag::discTotal() const {
	std::string discString = textString(Frames::FRAME_DISC);
	size_t slashPos = discString.find_first_of('/');
	return slashPos == std::string::npos ? "" : discString.substr(slashPos + 1);
}
///@pkg ID3.h
void Tag::disc(const std::string& newDisc) {
	std::string	discString = numericalString(newDisc) ? newDisc : "",
	            total      = discTotal();
	text(FRAME_DISC, total.empty() ? discString : discString + '/' + total);
}
///@pkg ID3.h
void Tag::discTotal(const std::string& newDiscTotal) {
	std::string	discString = numericalString(newDiscTotal) ? newDiscTotal : "";
	if(discString.empty()) text(FRAME_DISC, disc());
	else                   text(FRAME_DISC, disc() + '/' + discString);
}

///@pkg ID3.h
Picture Tag::picture() const {
	//Get the picture Frame, or a nullptr if there isn't a picture
	PictureFrame* picture = getFrame<PictureFrame>(Frames::FRAME_PICTURE);
	
	//Return a Picture struct
	return picture == nullptr ? Picture() : Picture(picture->picture(),
	                                                picture->mimeType(),
	                                                picture->description(),
	                                                picture->pictureType());
}
///@pkg ID3.h
std::vector<Picture> Tag::pictures() const {
	//Get the vector of PictureFrames in the Frame map.
	std::vector<PictureFrame*> frames = getFrames<PictureFrame>(Frames::FRAME_PICTURE);
	
	std::vector<Picture> toReturn;   //The Picture vector to return
	toReturn.reserve(frames.size()); //Reserve space for each picture Frame
	
	//Loop through the Frame vector and add a Picture struct for each Frame
	for(PictureFrame* currentFrame : frames)
		toReturn.push_back(Picture(currentFrame->picture(),
	                              currentFrame->mimeType(),
	                              currentFrame->description(),
	                              currentFrame->pictureType()));
	
	return toReturn; //Return the Picture vector
}
///@pkg ID3.h
Picture Tag::picture(const std::function<bool (const std::string&, const PictureType)>& filterFunc) const {
	//Get the vector of PictureFrames in the Frame map.
	std::vector<PictureFrame*> frames = getFrames<PictureFrame>(Frames::FRAME_PICTURE);
	
	//Look for a PictureFrame that matches the filter function
	for(PictureFrame* currentFrame : frames)
		if(filterFunc(currentFrame->description(), currentFrame->pictureType()))
			return Picture(currentFrame->picture(),
	                     currentFrame->mimeType(),
	                     currentFrame->description(),
	                     currentFrame->pictureType());
	
	return Picture(); //Return an empty picture
}
///@pkg ID3.h
void Tag::picture(const Picture& newPicture) {
	//Validate the picture size
	if(newPicture.size() + HEADER_BYTE_SIZE > MAX_TAG_SIZE) {
		FrameID picID = FRAME_PICTURE;
		throw FrameSizeException(picID, picID.description());
	}
	
	if(exists(FRAME_PICTURE)) {
		//Get the range of pictures. Each iterator has a "second" variable which
		//stores the FramePtr object.
		std::pair<FrameMap::iterator, FrameMap::iterator> range = frames.equal_range(FRAME_PICTURE);
		//The frame object to use
		PictureFrame* frame = nullptr;
		//Whether only a single picture of that type can exist in the tag
		bool singleType = newPicture.type == PictureType::FILE_ICON ||
		                  newPicture.type == PictureType::OTHER_FILE_ICON;
		
		//Loop through the range
		for(auto start = range.first; start != range.second; start++) {
			//Get the Frame object from the FramePtr, and cast it to PictureFrame
			PictureFrame* derivedFrame = dynamic_cast<PictureFrame*>(start->second.get());
			//If the description doesn't match, or is singleType and the types don't match
			if(derivedFrame == nullptr ||
			   !(derivedFrame->description() == newPicture.description ||
			     (singleType && derivedFrame->pictureType() == newPicture.type))) continue;
			//Get the object
			if(frame == nullptr) frame = derivedFrame;
			//Keep the Frame object in case revert() gets called, but make it
			//null to prevent it from being written to file
			else derivedFrame->picture(ByteArray(), "", "", PictureType::NULL_PICTURE);
		}
		//If a frame was found, update it
		if(frame != nullptr) {
			frame->picture(newPicture.data, newPicture.MIME, newPicture.description, newPicture.type);
			return;
		}
	}
	//If no picture with the same description exists
	addFrame(factory.createPicturePair(newPicture.data, newPicture.MIME, newPicture.description, newPicture.type));
}

///@pkg ID3.h
unsigned long long Tag::playCount() const {
	//Get the play count Frame, or a nullptr if it's not on file
	PlayCountFrame* pcnt = getFrame<PlayCountFrame>(Frames::FRAME_PLAY_COUNT);
	if(pcnt != nullptr) return pcnt->playCount();
	
	//If no FRAME_PLAY_COUNT frame, then try the Popularimeter
	pcnt = getFrame<PlayCountFrame>(Frames::FRAME_POPULARIMETER);
	
	return pcnt == nullptr ? 0ULL : pcnt->playCount(); //Return the play count
}

///@pkg ID3.h
unsigned long long Tag::playCount(const std::function<bool (const std::string&)>& filterFunc) const {
	//Look through the Popularimeters with the filter function
	std::vector<PopularimeterFrame*> popularimeters = getFrames<PopularimeterFrame>(Frames::FRAME_POPULARIMETER);
	for(const PopularimeterFrame* const popm : popularimeters)
		if(filterFunc(popm->email())) return popm->playCount();
	
	if(!popularimeters.size()) {
		//No popularimeters on file, so get the play count Frame, or a nullptr if
		//it's not on file
		PlayCountFrame* pcnt = getFrame<PlayCountFrame>(Frames::FRAME_PLAY_COUNT);
		if(pcnt != nullptr) return pcnt->playCount();
	}
	return 0ULL; //No matching Frame found, so return 0
}
///@pkg ID3.h
void Tag::playCount(const unsigned long long count) {
	PlayCountFrame* pcnt = getFrame<PlayCountFrame>(FRAME_PLAY_COUNT);
	if(pcnt == nullptr) addFrame(factory.createPlayCountPair(count));
	else                pcnt->playCount(count);
}
///@pkg ID3.h
void Tag::playCount(const unsigned long long count, const std::string& email) {
	std::vector<PopularimeterFrame*> popularimeters = getFrames<PopularimeterFrame>(Frames::FRAME_POPULARIMETER);
	for(PopularimeterFrame* const popm : popularimeters) {
		if(email == popm->email()) {
			popm->playCount(count); //Update the playcount
			return;
		}
	}
	//No Popularimeter with the same play count found, create a new one
	addFrame(factory.createPlayCountPair(count, 0, email));
}

///@pkg ID3.h
ushort Tag::rating() const {
	//Look for the first Popularimeter
	PopularimeterFrame* popm = getFrame<PopularimeterFrame>(Frames::FRAME_POPULARIMETER);
	return popm == nullptr ? 0 : popm->rating(); //Return the rating
}
///@pkg ID3.h
ushort Tag::rating(const std::function<bool (const std::string&)>& filterFunc) const {
	//Look through the Popularimeters with the filter function
	std::vector<PopularimeterFrame*> popularimeters = getFrames<PopularimeterFrame>(Frames::FRAME_POPULARIMETER);
	for(const PopularimeterFrame* const popm : popularimeters)
		if(filterFunc(popm->email())) return popm->rating();
	return 0; //No matching PopularimeterFrame found, so return 0
}
///@pkg ID3.h
void Tag::rating(const uint8_t rating, const std::string& email) {
	std::vector<PopularimeterFrame*> popularimeters = getFrames<PopularimeterFrame>(Frames::FRAME_POPULARIMETER);
	for(PopularimeterFrame* const popm : popularimeters) {
		if(email == popm->email()) {
			popm->rating(rating); //Update the playcount
			return;
		}
	}
	//No Popularimeter with the same rating found, create a new one
	addFrame(factory.createPlayCountPair(0, rating, email));
}

///@pkg ID3.h
EventTimingCode Tag::timingCode(const TimingCodes code) const {
	EventTimingFrame* frame = getFrame<EventTimingFrame>(Frames::FRAME_EVENT_TIMING_CODES);	
	return frame == nullptr || frame->null() ?
	       //No valid event timing code frame found, return a struct with the default value
	       EventTimingCode(code) :
	       //Return a struct with all the values set
	       EventTimingCode(code, frame->value(code), frame->format() == TimeStampFormat::MILLISECONDS);
}

///@pkg ID3.h
void Tag::timingCode(const TimingCodes code, const ulong value, const bool forceMilliseconds) {
	if(exists(FRAME_EVENT_TIMING_CODES)) {
		EventTimingFrame* frame = getFrame<EventTimingFrame>(FRAME_EVENT_TIMING_CODES);
		if(frame != nullptr) {
			//Force the use of milliseconds
			if(forceMilliseconds && frame->format() == TimeStampFormat::MPEG_FRAMES)
				frame->clear();
			frame->value(code, value); //Set the value
			return;
		}
	}
	//If the frame does not exist
	FramePtr framePtr = factory.create(FRAME_EVENT_TIMING_CODES);
	EventTimingFrame* frame = dynamic_cast<EventTimingFrame*>(framePtr.get());
	if(frame != nullptr) frame->value(code, value);
	addFrame(FRAME_EVENT_TIMING_CODES, framePtr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////  E N D   S P E C I F I C   F R A M E /////////////////////
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
size_t Tag::size() const { return frames.size(); }

///@pkg ID3.h
std::string Tag::fileName() const { return filename; }

///@pkg ID3.h
ulong Tag::fileSize() const { return filesize; }

///@pkg ID3.h
void Tag::print() const {
	std::cout << "\n......................\n";
	if(filename.empty()) std::cout << "Printing ID3 tag information:\n";
	else                 std::cout << "Printing ID3 tag information about file " << filename << ":\n";
	std::cout << "Tag size:                 " << v2TagInfo.size << '\n';
	std::cout << "Padding size:             " << (v2TagInfo.totalSize - v2TagInfo.paddingStart) << '\n';
	
	std::cout << "ID3 version(s) and flags: " << getVersionString(true) << '\n';
	std::cout << "Number of frames:         " << frames.size() << '\n';
	
	for(const FramePair& currentFramePair : frames) {
		std::cout << "--------------------------\n";
		currentFramePair.second->print();
	}
	
	std::cout << "..........................\n" << std::noboolalpha;
}

///@pkg ID3.h
bool Tag::addFrame(const FrameID& frameName, FramePtr frame) {
	//Check if the Frame is valid
	if((exists(frameName) && !frameName.allowsMultiple()) ||
	   frame.get() == nullptr || frame->null() || frame->empty())
		return false;
	frames.emplace(frameName, frame);
	return true;
}

///@pkg ID3.h
bool Tag::addFrame(const FramePair& frameMapPair) {
	//Check if the Frame is valid
	if((exists(frameMapPair.first) && !FrameID(frameMapPair.first).allowsMultiple()) ||
	   frameMapPair.second.get() == nullptr || frameMapPair.second->null() || frameMapPair.second->empty())
		return false;
	frames.emplace(frameMapPair);
	return true;
}

///@pkg ID3.h
template<typename DerivedFrame>
DerivedFrame* Tag::getFrame(const FrameID& frameName) const {
	//Check if the Frame is in the map
	const FrameMap::const_iterator result = frames.find(frameName);
	if(result == frames.end()) return nullptr;
	
	//If the frame is in the map, get it
	FramePtr frameObj = result->second;
	
	//If the frame is "null" then return nullptr
	if(frameObj->null()) return nullptr;
	
	//Get the requested frame class
	DerivedFrame* derivedFrameObj = dynamic_cast<DerivedFrame*>(frameObj.get());
	
	//If the Frame is not a DerivedFrame then nullptr will be returned
	return derivedFrameObj;
}

///@pkg ID3.h
template<typename DerivedFrame>
DerivedFrame* Tag::getFrame(const FrameID& frameName, const bool mismatchDelete) {
	//Check if the Frame is in the map
	const FrameMap::const_iterator result = frames.find(frameName);
	if(result == frames.end()) return nullptr;
	
	//If the frame is in the map, get it
	FramePtr frameObj = result->second;
	
	//If the frame is "null" then return nullptr
	if(result->second->null()) {
		if(mismatchDelete) frames.erase(result);
		return nullptr;
	}
	
	//Get the requested frame class
	DerivedFrame* derivedFrameObj = dynamic_cast<DerivedFrame*>(result->second.get());
	
	//If mismatchDelete, then check if it's an UnknownFrame, and if so erase it
	if(mismatchDelete && derivedFrameObj == nullptr) {
		UnknownFrame* unknownFrameObj = dynamic_cast<UnknownFrame*>(result->second.get());
		if(unknownFrameObj != nullptr) frames.erase(result);
	}
	
	//If the Frame is not a DerivedFrame then nullptr will be returned
	return derivedFrameObj;
}

///@pkg ID3.h
template<typename DerivedFrame>
std::vector<DerivedFrame*> Tag::getFrames(const FrameID& frameName) const {
	//Get the range.
	//Each const_iterator has a "first" variable, which stores the FrameID value,
	//and a "second" variable, which stores the FramePtr object.
	std::pair<FrameMap::const_iterator, FrameMap::const_iterator> range = frames.equal_range(frameName);
	
	//The vector to return
	std::vector<DerivedFrame*> derivedFrameVector;
	
	//Loop through the range
	for(auto start = range.first; start != range.second; start++) {
		//Get the Frame object from the FramePtr, and cast it to DerivedFrame
		DerivedFrame* derivedFrameObj = dynamic_cast<DerivedFrame*>(start->second.get());
		
		//Only append the Frame if it casted correctly and it's not null
		if(derivedFrameObj != nullptr && !derivedFrameObj->null())
			derivedFrameVector.push_back(derivedFrameObj);
	}
	
	//Return the vector
	return derivedFrameVector;
}

///@pkg ID3.h
Text Tag::getTextStruct(const Frame* const frame) const {
	//If a nullptr, return a default Text struct
	if(frame == nullptr) return Text();
	
	//Cast the frame to a TextFrame
	const TextFrame* const textFrameObj = dynamic_cast<const TextFrame* const>(frame);
	
	//If it's not a TextFrame, return a default Text struct
	if(textFrameObj == nullptr) return Text();
	
	//Try casting the Frame to a DescriptiveTextFrame
	const DescriptiveTextFrame* const descFrameObj = dynamic_cast<const DescriptiveTextFrame* const>(frame);
	
	//If the Frame is a DescriptiveTextFrame, then return a Text struct with its
	//description and language. If it's a different TextFrame class, then let
	//those fields default to empty strings.
	return descFrameObj == nullptr ?
	       Text(textFrameObj->content()) :
	       Text(descFrameObj->TextFrame::content(), descFrameObj->description(), descFrameObj->language());
}

///@pkg ID3.h
void Tag::readFile(std::istream& file, const bool readFrames) {
	if(file) {
		file.seekg(0, std::ifstream::end);
		filesize = file.tellg(); //Get the filesize
		readFileV2(file, readFrames);
		readFileV1(file, readFrames);
	}
}

///@pkg ID3.h
void Tag::readFileV1(std::istream& file, const bool readFrames) {
	V1::Tag tags;
	V1::ExtendedTag extTags;
	bool extTagsSet = false;
	
	if(filesize < V1::BYTE_SIZE) return;
	
	try {
		file.seekg(-V1::BYTE_SIZE, std::ifstream::end);
		if(file.fail()) return;
		
		file.read(reinterpret_cast<char*>(&tags), V1::BYTE_SIZE);
		
		if(memcmp(tags.header, "TAG", 3) != 0) return;
		
		//Get the bytes for the extended tags
		if(filesize > V1::BYTE_SIZE + V1::EXTENDED_BYTE_SIZE) {
			file.seekg(-V1::BYTE_SIZE-V1::EXTENDED_BYTE_SIZE, std::ifstream::end);
			extTagsSet = static_cast<bool>(file); //extTagsSet will be true only if the file is valid
			if(extTagsSet) {
				file.read(reinterpret_cast<char*>(&extTags), V1::EXTENDED_BYTE_SIZE);
				extTagsSet = memcmp(extTags.header, "TAG+", 4) == 0;
			}
		}
		
		if(!readFrames) return;
		if(extTagsSet) setTags(extTags);
		setTags(tags);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV1(std::ifstream&): " << e.what() << '\n';
	}
}

///@pkg ID3.h
void Tag::readFileV2(std::istream& file, const bool readFrames) {
	Header tagsHeader;
	
	if(filesize < HEADER_BYTE_SIZE) return;
	
	file.seekg(0, std::ifstream::beg);
	if(!file) return;
	
	file.read(reinterpret_cast<char*>(&tagsHeader), HEADER_BYTE_SIZE);
	if(memcmp(tagsHeader.header, "ID3", 3) != 0) return;
	
	//Get the tag flags
	if((tagsHeader.flags & FLAG_UNSYNCHRONISATION) == FLAG_UNSYNCHRONISATION)
		v2TagInfo.flagUnsynchronisation = true;
	if((tagsHeader.flags & FLAG_EXT_HEADER) == FLAG_EXT_HEADER)
		v2TagInfo.flagExtHeader = true;
	if((tagsHeader.flags & FLAG_EXPERIMENTAL) == FLAG_EXPERIMENTAL)
		v2TagInfo.flagExperimental = true;
	if((tagsHeader.flags & FLAG_FOOTER) == FLAG_FOOTER)
		v2TagInfo.flagFooter = true;
	
	//Get the major version and size
	v2TagInfo.majorVer = tagsHeader.majorVer;
	v2TagInfo.minorVer = tagsHeader.minorVer;
	v2TagInfo.size = byteIntVal(tagsHeader.size, 4, true);
	v2TagInfo.totalSize = HEADER_BYTE_SIZE + v2TagInfo.size + (v2TagInfo.flagFooter ? HEADER_BYTE_SIZE : 0);
	
	//The position to start reading from the file
	ulong frameStartPos = HEADER_BYTE_SIZE;
	
	//Make sure the ID3v2 version is supported and that unsynchronisation
	//isn't set on ID3v2.3 and below.
	//In ID3v2.4, it is handled on a per-frame basis.
	if(v2TagInfo.majorVer < MIN_SUPPORTED_VERSION ||
		v2TagInfo.majorVer > MAX_SUPPORTED_VERSION ||
		v2TagInfo.minorVer != SUPPORTED_MINOR_VERSION ||
		(v2TagInfo.flagUnsynchronisation && v2TagInfo.majorVer <= 3))
		return;
	
	//Make sure that the size is valid, or throw a FormatExcetion
	if(v2TagInfo.totalSize > filesize)
		throw FileFormatException("Tag size format error on file \"" + filename + "\" when reading tags: tags are bigger than the file size!");
	
	//Skip over the extended header
	if(v2TagInfo.flagExtHeader) {
		//Seek to the position to read the extended header
		file.seekg(frameStartPos, std::ifstream::beg);
		if(!file) return;
		
		//The extended header is different from ID3v2.4, and ID3v2.3, and ID3v2.2.
		if(v2TagInfo.majorVer >= 4) {
			V4ExtHeader extHeader;
			
			//Verify that there's enough space
			if(frameStartPos + sizeof(V4ExtHeader) > filesize) return;
			
			//Get the extended header
			file.read(reinterpret_cast<char*>(&extHeader), sizeof(V4ExtHeader));
			
			//Increment the start position. The extended header size is synchsafe in ID3v2.4
			ulong extHeaderSize = byteIntVal(extHeader.size, 4, true);
			frameStartPos += sizeof(V4ExtHeader) + extHeaderSize;
		} else if(v2TagInfo.majorVer == 3) {
			V3ExtHeader extHeader;
			
			//Verify that there's enough space
			if(frameStartPos + sizeof(V3ExtHeader) > filesize) return;
			
			//Get the extended header
			file.read(reinterpret_cast<char*>(&extHeader), sizeof(V3ExtHeader));
			
			//Increment the start position. The extended header size is not synchsafe in ID3v2.3
			ulong extHeaderSize = byteIntVal(extHeader.size, 4, false);
			frameStartPos += sizeof(V3ExtHeader) + extHeaderSize;
		} else {
			//In ID3v2.2, the extended header flag bit is used for a compression flag
			//instead. Since there is no standard compression format used in ID3v2.2,
			//it is not supported.
			return;
		}
	}
	
	//The file has correctly formatted ID3v2 tags
	tagsSet.v2 = true;
	
	//Initialize the Tag's FrameFactory properly
	factory = FrameFactory(file, v2TagInfo.majorVer, v2TagInfo.totalSize);
	
	if(!readFrames) return; //If readFrames is false, stop now
	
	//Loop over the ID3 tags, and stop once all ID3 frames have been
	//reached or a frame is null. Add every frame to the frames map.
	while(frameStartPos + HEADER_BYTE_SIZE < v2TagInfo.totalSize) {
		//Create a new Frame at this position
		FramePtr frame = factory.create(frameStartPos);
		//Add the Frame to the map if it's not null
		if(!frame->null()) addFrame(frame->frame(), frame);
		//If the frame content is a valid size (bigger than an ID3v2 header)
		//then continue on to the next frame. If not, then stop the loop.
		if(frame->size(true) > HEADER_BYTE_SIZE && !frame->frame().unknown()) {
			frameStartPos += frame->size(true);
			
			//Account for 4 bytes added when reading ID3v2.2 frames from the
			//ID3::FrameFactory class
			if(v2TagInfo.majorVer <= 2) frameStartPos -= 4;
		}
		else {
			//Get the start of padding and exit the loop
			v2TagInfo.paddingStart = frameStartPos;
			break;
		}
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::Tag& tags, bool zeroCheck) {
	//Check if this isn't actually a ID3v1.1 tag
	if(zeroCheck && tags.comment[28] == '\0' && tags.comment[29] != '\0') {
		V1::P1Tag correctID3VerTags;
		std::memcpy(&correctID3VerTags, &tags, sizeof correctID3VerTags);
		setTags(correctID3VerTags, false);
		return;
	}
	
	tagsSet.v1 = true;
	
	//Save the V1 tags as Frame objects.
	//If ID3v2 frame equivalents were previously read, then addFrame() will not
	//add the ID3v1 tags. The only exception is the comment. Since ID3v1 comments
	//are not really equivalent to ID3v2 comments, don't add an ID3v1 comment if
	//there's already a read comment frame.
	try {
		addFrame(factory.createPair(Frames::FRAME_TITLE,  terminatedstring(tags.title, 30)));
		addFrame(factory.createPair(Frames::FRAME_ARTIST, terminatedstring(tags.artist, 30)));
		addFrame(factory.createPair(Frames::FRAME_ALBUM,  terminatedstring(tags.album, 30)));
		addFrame(factory.createPair(Frames::FRAME_YEAR,   terminatedstring(tags.year, 4)));
		if(!exists(Frames::FRAME_COMMENT))
			addFrame(factory.createPair(Frames::FRAME_COMMENT, terminatedstring(tags.comment, 30)));
		addFrame(factory.createPair(Frames::FRAME_GENRE,  V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::Tag&, bool): " << e.what() << '\n';
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
	
	//Save the V1 tags as Frame objects.
	//If ID3v2 frame equivalents were previously read, then addFrame() will not
	//add the ID3v1 tags. The only exception is the comment. Since ID3v1 comments
	//are not really equivalent to ID3v2 comments, don't add an ID3v1 comment if
	//there's already a read comment frame.
	try {
		addFrame(factory.createPair(Frames::FRAME_TITLE,  terminatedstring(tags.title, 30)));
		addFrame(factory.createPair(Frames::FRAME_ARTIST, terminatedstring(tags.artist, 30)));
		addFrame(factory.createPair(Frames::FRAME_ALBUM,  terminatedstring(tags.album, 30)));
		addFrame(factory.createPair(Frames::FRAME_YEAR,   terminatedstring(tags.year, 4)));
		if(!exists(Frames::FRAME_COMMENT))
			addFrame(factory.createPair(Frames::FRAME_COMMENT, terminatedstring(tags.comment, 28)));
		addFrame(factory.createPair(Frames::FRAME_TRACK,  std::to_string(tags.trackNum)));
		addFrame(factory.createPair(Frames::FRAME_GENRE,  V1::getGenreString(tags.genre)));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::P1Tag&, bool): " << e.what() << '\n';
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::ExtendedTag& tags) {
	//Save the V1 Extended tags as Frame objects.
	//Since I'm using ID3::Tag::addFrame(), these will not overwrite any V2 tags.
	try {
		tagsSet.v1Extended = true;
		
		addFrame(factory.createPair(Frames::FRAME_TITLE,  terminatedstring(tags.title, 60)));
		addFrame(factory.createPair(Frames::FRAME_ARTIST, terminatedstring(tags.artist, 60)));
		addFrame(factory.createPair(Frames::FRAME_ALBUM,  terminatedstring(tags.album, 60)));
		addFrame(factory.createPair(Frames::FRAME_GENRE,  terminatedstring(tags.genre, 30)));
		
		//Set the start and end times
		uint8_t startTime = atoi(tags.startTime), endTime = atoi(tags.endTime);
		if(timingCode(TimingCodes::AUDIO_START).value != 0)
			timingCode(TimingCodes::AUDIO_START, startTime, true);
		if(timingCode(TimingCodes::AUDIO_END).value != 0)
			timingCode(TimingCodes::AUDIO_END, endTime, true);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(ID3::V1::ExtendedTag&): " << e.what() << '\n';
	}
}

///@pkg ID3.h
Tag::TagsOnFile::TagsOnFile() : v1(false), v1_1(false), v1Extended(false), v2(false) {}

///@pkg ID3.h
Tag::TagInfo::TagInfo() : majorVer(WRITE_VERSION),
                          minorVer(SUPPORTED_MINOR_VERSION),
                          flagUnsynchronisation(false),
                          flagExtHeader(false),
                          flagExperimental(false),
                          flagFooter(false),
                          size(0),
                          totalSize(0),
                          paddingStart(0) {}
