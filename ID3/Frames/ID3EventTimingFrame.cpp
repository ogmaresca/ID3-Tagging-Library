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

#include "ID3EventTimingFrame.hpp" //For the class definition
#include "../ID3.hpp"              //For EventTimingCode
#include "../ID3Functions.hpp"     //For intToByteArray() and byteIntVal()

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
EventTimingFrame::~EventTimingFrame() {}

///@pkg ID3EventTimingFrame.h
FrameClass EventTimingFrame::type() const noexcept { return FrameClass::CLASS_EVENT_TIMING; }

///@pkg ID3EventTimingFrame.h
bool EventTimingFrame::empty() const { return map.empty(); }

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
std::string EventTimingFrame::print() const {
	std::string out = Frame::print() + "Timestamps:     ";
	switch(timeStampFormat) {
		case TimeStampFormat::MILLISECONDS: { out += "Milliseconds"; break; }
		case TimeStampFormat::MPEG_FRAMES:  { out += "MPEG Frames"; break; }
		default:                              out += "Unknown";
	}
	for(const std::pair<uint8_t, ulong>& eventCodePair : map)
		out += "\nEvent code pair:  {Timing code " + std::to_string((ushort)eventCodePair.first) +
		       ", Value " + std::to_string(eventCodePair.second) + "}\n";
	return out + "Frame class:    EventTimingFrame\n";
}

///@pkg ID3EventTimingFrame.h
void EventTimingFrame::writeBody() {
	//Save the timestamp format
	frameContent.push_back(static_cast<uint8_t>(timeStampFormat));
	
	//Loop through the map and save each code
	for(const std::pair<uint8_t, ulong>& eventCodePair : map) {
		ByteArray valueArr = intToByteArray(eventCodePair.second, TIME_BYTE_LENGTH, false);
		
		//Add the timing code
		frameContent.push_back(eventCodePair.first);
		
		//Add the value
		frameContent.insert(frameContent.end(), valueArr.begin(), valueArr.end());
	}
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
