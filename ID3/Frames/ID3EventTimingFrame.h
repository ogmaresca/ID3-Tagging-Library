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

#ifndef ID3_EVENT_TIMING_FRAME_H
#define ID3_EVENT_TIMING_FRAME_H

#include <unordered_map> //For std::unordered_map

#include "ID3Frame.h"

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * ID3v2.3.0 standard: http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: http://id3.org/id3v2.4.0-structure
 * 
 * @see ID3.h
 * @see ID3Frame.h
 */
namespace ID3 {
	/**
	 * An enum of the different event timing codes.
	 */
	enum class TimingCodes : uint8_t {
		PADDING                  = 0x0, //(has no meaning)
		INITIAL_SILENCE_END      = 0x1,
		INTRO_START              = 0x2,
		MAIN_PART_START          = 0x3,
		OUTRO_START              = 0x4,
		OUTRO_END                = 0x5,
		VERSE_START              = 0x6,
		REFRAIN_START            = 0x7,
		INTERLUDE_START          = 0x8,
		THEME_START              = 0x9,
		VARIATION_START          = 0xA,
		KEY_CHANGE               = 0xB,
		TIME_CHANGE              = 0xC,
		MOMENTARY_UNWANTED_NOISE = 0xD, //(Snap, Crackle, & Pop)
		SUSTAINED_NOISE          = 0xE,
		SUSTAINED_NOISE_END      = 0xF,
		INTRO_END                = 0x10,
		MAIN_PART_END            = 0x11,
		VERSE_END                = 0x12,
		REFRAIN_END              = 0x13,
		THEME_END                = 0x14,
		PROFANITY                = 0x15,
		PROFANITY_END            = 0x16,
		RESERVED_BLOCK_1_START   = 0x17,
		RESERVED_BLOCK_1_END     = 0xDF,
		NOT_PREDEFINED_SYNCH_0   = 0xE0,
		NOT_PREDEFINED_SYNCH_1   = 0xE1,
		NOT_PREDEFINED_SYNCH_2   = 0xE2,
		NOT_PREDEFINED_SYNCH_3   = 0xE3,
		NOT_PREDEFINED_SYNCH_4   = 0xE4,
		NOT_PREDEFINED_SYNCH_5   = 0xE5,
		NOT_PREDEFINED_SYNCH_6   = 0xE6,
		NOT_PREDEFINED_SYNCH_7   = 0xE7,
		NOT_PREDEFINED_SYNCH_8   = 0xE8,
		NOT_PREDEFINED_SYNCH_9   = 0xE9,
		NOT_PREDEFINED_SYNCH_A   = 0xEA,
		NOT_PREDEFINED_SYNCH_B   = 0xEB,
		NOT_PREDEFINED_SYNCH_C   = 0xEC,
		NOT_PREDEFINED_SYNCH_D   = 0xED,
		NOT_PREDEFINED_SYNCH_E   = 0xEE,
		NOT_PREDEFINED_SYNCH_F   = 0xEF,
		RESERVED_BLOCK_2_START   = 0xF0,
		RESERVED_BLOCK_2_END     = 0xFC,
		AUDIO_END                = 0xFD, //(start of silence)
		AUDIO_FILE_END           = 0xFE,
		RESERVED_BLOCK_3         = 0xFF
	};
	
	/**
	 * An enum of possible time stamp formats. NULL_BLOCK_1 and NULL_BLOCK_2_START
	 * are only used internally,
	 */
	enum class TimeStampFormat : uint8_t {
		NULL_BLOCK_1       = 0,
		MPEG_FRAMES        = 1,
		MILLISECONDS       = 2,
		NULL_BLOCK_2_START = 3,
		NULL_FORMAT        = 0xFF
	};
	
	/*
	 *struct TimingCode {
	 *		bool usingMilliseconds;
	 *		TimingCodes timingCode;
	 * 	ulong value;
	 *}; 
	 */
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////// E V E N T T I M I N G F R A M E //////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * EventTimingFrame is a child class of Frame. It is to be used for frames
	 * with event timing codes (the ETCO frame).
	 * 
	 * @see ID3::Frame
	 */
	class EventTimingFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a EventTimingFrame with the same frame
			 * ID, "null" status, and if the timestamp format and event codes match
			 * (when neither of the Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_EVENT_TIMING.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. A EventTimingFrame is empty
			 * if its event timing map is empty.
			 * 
			 * @return true if the Frame is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * Get the time stamp format.
			 * 
			 * @return TimeStampFormat::MPEG_FRAMES, TimeStampFormat::MILLISECONDS,
			 *         or TimeStampFormat::NULL_FORMAT.
			 */
			TimeStampFormat format() const;
			
			/**
			 * Get the value of a given event timing code.
			 * 
			 * @param timingCode The relevant TimingCodes enum value that
			 *                   represents the timing code you wish to retrieve.
			 * @return The event timing code, or 0 if it doesn't exist.
			 */
			ulong value(const TimingCodes timingCode) const;
			
			/**
			 * Set the value of a given event timing code.
			 * 
			 * @param timingCode The relevant TimingCodes enum value that
			 *                   represents the timing code you wish to set.
			 * @param time       The new timing code value.
			 * @return The event timing code, or 0 if it doesn't exist.
			 */
			void value(const TimingCodes timingCode, const ulong time);
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for PictureFrame writes a ByteArray with the
			 * currently stored content.
			 * 
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
			
		protected:
			/**
			 * This constructor calls the similar constructor in the Frame class
			 * with ETCO for frameName. If the given ByteArray is long enough to be
			 * valid it then calls ID3::EventTimingFrame::read() to process the ByteArray.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * NOTE: If the TimeStampFormat is not MILLISECONDS or MPEG_FRAMES,
			 *       then the Frame will be "null".
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			EventTimingFrame(const ushort version,
			                 const ByteArray& frameBytes);
			
			/**
			 * An empty constructor to create a new ETCO frame.
			 * 
			 * NOTE: An EventTimingFrame created with this constructor will have
			 *       the timestamp format be TimeStampFormat::MILLISECONDS.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			EventTimingFrame() noexcept;
			
			/**
			 * The read() method for PictureFrame reads the MIME type, picture
			 * type, description, and picture data from the stored frame bytes.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The time stamp format. The only formats that should be saved are
			 * TimeStampFormat::MPEG_FRAMES, TimeStampFormat::MILLISECONDS, and
			 * TimeStampFormat::NULL_FORMAT.
			 * 
			 * @see ID3::PictureFrame::format()
			 */
			TimeStampFormat timeStampFormat;
			
			/**
			 * The event timing codes map. The key is the a value in the
			 * TimingCodes enum, and the value is the time of the event.
			 */
			std::unordered_map<uint8_t, ulong> map;
	};
}

#endif
