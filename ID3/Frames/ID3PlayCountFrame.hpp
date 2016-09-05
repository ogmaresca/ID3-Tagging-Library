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

#ifndef ID3_PLAY_COUNT_FRAME_HPP
#define ID3_PLAY_COUNT_FRAME_HPP

#include "ID3Frame.hpp" //For the Frame base class definition

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
			 * The destructor.
			 */
			virtual ~PlayCountFrame();
			
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
			
		protected:
			/**
			 * This constructor calls the similar constructor in the Frame class
			 * with PCNT for frameName. If the given ByteArray is long enough to be
			 * valid it then calls ID3::PlayCountFrame::read() to process the ByteArray.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it is a
			 *       supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(FrameID&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			PlayCountFrame(const ushort     version,
			               const ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a play count frame. A Frame created
			 * from this constructor will return false when calling createdFromFile().
			 * 
			 * @param playCount The play count (defaults to 0).
			 * @see ID3::Frame::Frame()
			 */
			PlayCountFrame(const unsigned long long playCount=0ULL) noexcept;
			
			/**
			 * The read() method for PlayCountFrame reads the play count from the
			 * stored frame bytes.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The writeBody() method for PlayCountFrame appends to the ByteArray
			 * the play count.
			 * 
			 * @see ID3::Frame::write()
			 */
			virtual void writeBody();
			
			/**
			 * The play count.
			 * 
			 * @see ID3::PlayCountFrame::playCount()
			 */
			unsigned long long count;
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//////////////////// P O P U L A R I M E T E R F R A M E ////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * PopularimeterFrame is a child class of PlayCountFrame. It is to be used
	 * for frames with an email address, rating, and play count (the POPM frame).
	 * 
	 * @see ID3::Frame
	 */
	class PopularimeterFrame : virtual public PlayCountFrame {
		friend class FrameFactory;
		
		public:
			/**
			 * The destructor.
			 */
			virtual ~PopularimeterFrame();
			
			/**
			 * Checks if the given Frame is a PopularimeterFrame with the same
			 * frame frame ID, "null" status, and if the email address, ratings,
			 * and play counts match (when neither of the Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_POPULARIMETER.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Get the email address.
			 * 
			 * @return The email address.
			 */
			std::string email() const;
			
			/**
			 * Set the email address. Call write() to finalize changes.
			 * 
			 * @param newEmail The new email address.
			 */
			void email(const std::string& newEmail);
			
			/**
			 * Get the rating. Its value is between 0 and 5, where 0 is undefined
			 * and 1-5 are star ratings.
			 * 
			 * @return The rating.
			 */
			ushort rating() const;
			
			/**
			 * Set the rating. If the value is less than or equal to 5, then the
			 * rating will be set to what is given. If not, then the following
			 * ranges map to each rating:
			 * 
			 * 6-31:    1
			 * 32-95:   2
			 * 96-159:  3
			 * 169-223: 4
			 * 224-255: 5
			 * 
			 * @param newRating The new rating.
			 */
			void rating(uint8_t newRating);
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
		protected:
			/**
			 * This constructor calls the similar constructor in the Frame class
			 * with POPM for frameName. If the given ByteArray is long enough to be
			 * valid it then calls ID3::PopularimeterFrame::read() to process the ByteArray.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it is a
			 *       supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(FrameID&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			PopularimeterFrame(const ushort     version,
			                   const ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a Popularimeter frame. A Frame
			 * created from this constructor will return false when calling createdFromFile().
			 * 
			 * NOTE: If the rating is less than or equal to 5, then it will be set
			 * to what is given. If not, then the following ranges map to each rating:
			 *       6-31:    1
			 *       32-95:   2
			 *       96-159:  3
			 *       169-223: 4
			 *       224-255: 5
			 * 
			 * @param playCount The play count (defaults to 0).
			 * @param rating    The rating (defaults to 0).
			 * @param email     The email address (defaults to "")
			 * @see ID3::Frame::Frame()
			 */
			PopularimeterFrame(const unsigned long long playCount=0ULL,
			                   uint8_t                  rating=0,
			                   const std::string&       email="") noexcept;
			
			/**
			 * The read() method for PopularimeterFrame reads the play count from
			 * the stored frame bytes.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The writeBody() method for PopularimeterFrame appends the email,
			 * rating, and play count to the ByteArray.
			 * 
			 * @see ID3::Frame::writeBody()
			 */
			virtual void writeBody();
			
			/**
			 * The email address.
			 * 
			 * @see ID3::PopularimeterFrame::email()
			 */
			std::string emailAddress;
			
			/**
			 * The rating.
			 * 
			 * @see ID3::PopularimeterFrame::rating()
			 */
			ushort fiveStarRating;
	};
}

#endif
