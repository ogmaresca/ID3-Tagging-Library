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

#include "ID3EventTimingFrame.h"
#include "../ID3.h"
#include "../ID3Functions.h"
#include "../ID3Constants.h"

using namespace ID3;

///@pkg ID3.h
EventTimingCode::EventTimingCode(TimingCodes code,
                                 ulong val,
                                 bool milliseconds) : usingMilliseconds(milliseconds),
                                                      timingCode(code),
                                                      value(val) {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////  E V E N T T I M I N G F R A M E ///////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3EventTimingFrame.h
EventTimingFrame::EventTimingFrame(const ushort version,
                                   const ByteArray& frameBytes) : Frame::Frame(FRAME_EVENT_TIMING_CODES,
                                                                               version,
                                                                               frameBytes) {
	//If the frame content isn't null, then get the text content
	if(!isNull)
		read();
}

///@pkg ID3EventTimingFrame.h
EventTimingFrame::EventTimingFrame() noexcept : Frame::Frame(FRAME_EVENT_TIMING_CODES),
                                                timeStampFormat(TimeStampFormat::MILLISECONDS) {}


///@pkg ID3EventTimingFrame.h
FrameClass EventTimingFrame::type() const noexcept { return FrameClass::CLASS_EVENT_TIMING; }

///@pkg ID3EventTimingFrame.h
bool EventTimingFrame::empty() const { return map.size() == 0; }

///@pkg ID3EventTimingFrame.h
TimeStampFormat EventTimingFrame::format() const { return timeStampFormat; }

///@pkg ID3EventTimingFrame.h
ulong EventTimingFrame::value(const TimingCodes timingCode) const {
	const auto ITER = map.find(static_cast<uint8_t>(timingCode));
	
	//If the timing code isn't stored in the map, then return 0
	//Else, return the value stored in the map
	return ITER == map.end() ? 0 : ITER->second;
}

///@pkg ID3EventTimingFrame.h
void EventTimingFrame::value(const TimingCodes timingCode, const ulong time) {
	map[static_cast<uint8_t>(timingCode)] = time;
	isEdited = true;
}

///@pkg ID3EventTimingFrame.h
void EventTimingFrame::print() const {
	Frame::print();
	std::cout << "Timestamps:     ";
	switch(timeStampFormat) {
		case TimeStampFormat::MILLISECONDS: { std::cout << "Milliseconds"; break; }
		case TimeStampFormat::MPEG_FRAMES:  { std::cout << "MPEG Frames"; break; }
		default:                              std::cout << "Unknown";
	}
	for(const std::pair<uint8_t, ulong>& eventCodePair : map)
		std::cout << std::endl << "Event code pair:  (Timing code " <<
		          eventCodePair.first << ", Value " << eventCodePair.second <<
		          ")" << std::endl;
	std::cout << "Frame class:    EventTimingFrame" << std::endl;
}

///@pkg ID3EventTimingFrame.h
ByteArray EventTimingFrame::write() {
	//Set the ID3 version to ID3::WRITE_VERSION
	ID3Ver = WRITE_VERSION;
	
	const ushort TIME_BYTE_LENGTH = 4;
	
	//If the Frame is empty or null, then don't write anything to file
	if(empty() || isNull) {
		frameContent = ByteArray();
	} else {
		//Create a ByteArray that fits the header, timestamp format, and all the
		//timing codes
		const ulong NEW_FRAME_SIZE = HEADER_BYTE_SIZE + 1 + (map.size() * (1 + TIME_BYTE_LENGTH));
		
		//Reset the frame ByteArray. This automatically clears any flags.
		frameContent = ByteArray(HEADER_BYTE_SIZE + 1, '\0');
		//And reserve space for all the timing codes
		frameContent.reserve(NEW_FRAME_SIZE);
		
		//Save the frame name
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i] = id[i];
		
		//Save the frame size
		ByteArray size = intToByteArray(NEW_FRAME_SIZE - HEADER_BYTE_SIZE, 4, true);
		for(ushort i = 0; i < 4 && i < id.size(); i++)
			frameContent[i+4] = size[i];
		
		//Save the timestamp format
		frameContent[HEADER_BYTE_SIZE] = static_cast<uint8_t>(timeStampFormat);
		
		//Loop through the map and save each code
		for(const std::pair<uint8_t, ulong>& eventCodePair : map) {
			ByteArray valueArr = intToByteArray(eventCodePair.second, TIME_BYTE_LENGTH, false);
			
			//Add the timing code
			frameContent.push_back(eventCodePair.first);
			
			//Add the value
			frameContent.insert(frameContent.end(), valueArr.begin(), valueArr.end());
		}
	}
	
	isEdited = false;
	
	return frameContent;
}

///@pkg ID3EventTimingFrame.h
void EventTimingFrame::read() {
	const ushort HEADER_SIZE = headerSize();
	
	const ulong FRAME_SIZE = frameContent.size();
	
	const ushort TIME_BYTE_LENGTH = 4;
	
	//Reset the map
	map.clear();
	
	//Reserve space for the timing codes
	map.reserve((FRAME_SIZE - HEADER_SIZE) / (TIME_BYTE_LENGTH + 1));
	
	//Make sure that there is enough room for text before reading the frame bytes
	if(FRAME_SIZE > HEADER_SIZE) {
		const uint8_t TIMESTAMP = frameContent[HEADER_SIZE];
		
		//Set the timestamp. If the timestamp format is unknown, then the frame is "null"
		switch(TIMESTAMP) {
			  case static_cast<uint8_t>(TimeStampFormat::MPEG_FRAMES): {
				timeStampFormat = TimeStampFormat::MPEG_FRAMES; break;
			} case static_cast<uint8_t>(TimeStampFormat::MILLISECONDS): {
				timeStampFormat = TimeStampFormat::MILLISECONDS; break;
			} default: {
				timeStampFormat = TimeStampFormat::NULL_FORMAT; isNull = true;
			}
		}
		
		for(ulong i = HEADER_SIZE + 1; i + TIME_BYTE_LENGTH < FRAME_SIZE; i += 1 + TIME_BYTE_LENGTH) {
			uint8_t timingCode = frameContent[i];
			
			//Check that timing code is valid
			if(!((timingCode >= static_cast<uint8_t>(TimingCodes::RESERVED_BLOCK_1_START) &&
			      timingCode <= static_cast<uint8_t>(TimingCodes::RESERVED_BLOCK_1_END)) ||
			     (timingCode >= static_cast<uint8_t>(TimingCodes::RESERVED_BLOCK_2_START) &&
			      timingCode <= static_cast<uint8_t>(TimingCodes::RESERVED_BLOCK_2_END)) ||
			     timingCode == static_cast<uint8_t>(TimingCodes::RESERVED_BLOCK_3))) {
				ulong timeValue = byteIntVal(frameContent.data()+i+1, TIME_BYTE_LENGTH, false);
				
				//Add the timing code and its value to the map
				map.emplace(timingCode, timeValue);
			}
		}
	} else {
		isNull = true;
		timeStampFormat = TimeStampFormat::MILLISECONDS;
	}
}

///@pkg ID3EventTimingFrame.h
bool EventTimingFrame::operator==(const Frame* const frame) const noexcept {
	//Check if the frame IDs or "null" statuses match
	if(frame == nullptr || frame->frame() != id || isNull != frame->null())
		return false;
	//Check if it's a EventTimingFrame, and if it is compare the content
	const EventTimingFrame* const castFrame = dynamic_cast<const EventTimingFrame* const>(frame);
	//If it's not a EventTimingFrame return false
	if(castFrame == nullptr) return false;
	return isNull ? true : (timeStampFormat == castFrame->timeStampFormat &&
	                        map             == castFrame->map);
}
