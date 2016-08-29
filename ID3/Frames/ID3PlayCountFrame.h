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

#ifndef ID3_PLAY_COUNT_FRAME_H
#define ID3_PLAY_COUNT_FRAME_H

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
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//////////////////////// P L A Y C O U N T F R A M E ////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * PlayCountFrame is a child class of Frame. It is to be used for frames
	 * with a play count (the PCNT frame).
	 * 
	 * @see ID3::Frame
	 */
	class PlayCountFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a PlayCounterFrame with the same frame
			 * ID, "null" status, and if the play counts match (when neither of the
			 * Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_PLAY_COUNT.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. A PlayCountFrame is never
			 * empty, as it could store a play count of 0.
			 * 
			 * @return false
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * Get the play count.
			 * 
			 * @return The description.
			 */
			unsigned long long playCount() const;
			
			/**
			 * Set the play count. Call write() to finalize changes.
			 * 
			 * @param newPlayCount The new play count.
			 */
			void playCount(const unsigned long long newPlayCount);
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for PlayCountFrame writes a ByteArray with the
			 * currently stored content.
			 * 
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor calls the similar constructor in the Frame class
			 * with PCNT for frameName. If the given ByteArray is long enough to be
			 * valid it then calls ID3::PictureFrame::read() to process the ByteArray.
			 * 
			 * @see ID3::Frame::Frame(FrameID&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			PlayCountFrame(const ushort version,
			               const ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a play count frame. A Frame created
			 * from this constructor will return false when calling createdFromFile().
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @param version   The ID3v2 major version.
			 * @param playCount The play count (defaults to 0).
			 * 
			 */
			PlayCountFrame(const ushort version,
			               const unsigned long long playCount=0ULL);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" PlayCountFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			PlayCountFrame() noexcept;
			
			/**
			 * The read() method for PlayCountFrame reads the play count from the
			 * stored frame bytes.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The play count.
			 * 
			 * @see ID3::PlayCountFrame::playCount()
			 */
			unsigned long long count;
	};
}

#endif
