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

#ifndef ID3_PICTURE_FRAME_H
#define ID3_PICTURE_FRAME_H

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
	 * An enum of the different picture types defined in the ID3v2 Attached
	 * Picture frame.
	 */
	enum class PictureType : uint8_t {
		OTHER              = 0,
		FILE_ICON          = 1,
		OTHER_FILE_ICON    = 2,
		FRONT_COVER        = 3,
		BACK_COVER         = 4,
		LEAFLET_PAGE       = 5,
		MEDIA              = 6,
		LEAD_ARTIST        = 7,
		LEAD_PERFORMER     = 7,
		SOLOIST            = 7,
		ARTIST             = 8,
		PERFORMER          = 8,
		CONDUCTOR          = 9,
		BAND               = 10,
		ORCHESTRA          = 10,
		COMPOSER           = 11,
		LYRICIST           = 12,
		TEXT_WRITER        = 12,
		RECORDING_LOCATION = 13,
		DURING_RECORDING   = 14,
		DURING_PERFORMANCE = 15,
		MOVIE_CAPTURE      = 16,
		BRIGHT_FISH        = 17,
		ILLUSTRATION       = 18,
		BAND_LOGOTYPE      = 19,
		ARTIST_LOGOTYPE    = 19,
		PUBLISHER_LOGOTYPE = 20,
		STUDIO_LOGOTYPE    = 20
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////// P I C T U R E F R A M E //////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * PictureFrame is a child class of Frame. It is to be used for frames
	 * with picture content (the APIC frame).
	 * 
	 * @see ID3::Frame
	 */
	class PictureFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a PictureFrame with the same frame ID,
			 * "null" status, and if the MIME type and picture data match
			 * (when neither of the Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_PICTURE.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. A PictureFrame is empty if
			 * the picture data is empty.
			 * 
			 * @return true if the picture is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * Get the MIME type.
			 * 
			 * @return The MIME type.
			 */
			std::string mimeType() const;
			
			/**
			 * Get the picture type.
			 * 
			 * @return The PictureType enum value.
			 */
			PictureType pictureType() const;
			
			/**
			 * Get the picture type. Call write() to finalize changes.
			 * 
			 * @return newType The new PictureType enum value.
			 */
			void pictureType(PictureType newType);
			
			/**
			 * Get the description.
			 * 
			 * @return The description.
			 */
			std::string description() const;
			
			/**
			 * Set the description. Call write() to finalize changes.
			 * 
			 * @param newDescription The new description.
			 */
			void description(const std::string& newDescription);
			
			/**
			 * Get the picture data.
			 * 
			 * @return The picture data as a uint8_t vector.
			 */
			ByteArray picture() const;
			
			/**
			 * Update the picture. Call write() to finalize changes.
			 * 
			 * NOTE: If the MIME type is not valid as defined in
			 * ID3::TextFrame::allowedMIMEType(std::string&), then this Frame will
			 * become "null".
			 * 
			 * @param newPictureData The new PNG or JPG picture, as a uint8_t vector.
			 * @param newMIMEType The new MIME type.
			 */
			void picture(const ByteArray& newPictureData,
			             const std::string& newMIMEType);
			
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
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write(ushort)
			 */
			virtual ByteArray write(ushort version=0,
			                        bool minimize=false);
			
			/**
			 * Check if a given MIME type is allowed for ID3v2 pictures.
			 * The only allowed MIME types are "png" or "jpeg" with "image/"
			 * optionally prepended.
			 * 
			 * @param mimeType The MIME type to check.
			 * @return If the MIME type is allowed.
			 */
			static bool allowedMIMEType(const std::string& mimeType);
		
		protected:
			/**
			 * This constructor calls the similar constructor in the Frame class
			 * with APIC for frameName. If the given ByteArray is long enough to be
			 * valid it then calls ID3::PictureFrame::read() to process the ByteArray.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * NOTE: The picture is not checked to ensure that the enclosed picture
			 *       is a valid PNG or JPG image, or that there is even a picture.
			 *       This frame will only be null if the frame bytes are improperly
			 *       formatted, due to a missing MIME type null-termination,
			 *       picture type, or description null-termination.
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			PictureFrame(const ushort version,
			             const ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a picture frame. A Frame created
			 * from this constructor will return false when calling createdFromFile().
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * NOTE: The picture data is not checked to ensure that the it is a
			 *       valid PNG or JPG image.
			 * 
			 * @param version The ID3v2 major version.
			 * @param pictureBytes A ByteArray of the PNG or JPG image.
			 * @param mimeType The MIME type of the picture. If the MIMe type is
			 *                 not valid for ID3v2 pictures, then a "null" Frame
			 *                 object will be created instead.
			 * @param description The picture description (optional).
			 * @param type The picture type (optional). Defaults to the front cover.
			 * 
			 */
			PictureFrame(const ushort version,
			             const ByteArray pictureBytes,
			             const std::string& mimeType,
			             const std::string& description="",
			             const PictureType type=PictureType::FRONT_COVER);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" PictureFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			PictureFrame() noexcept;
			
			/**
			 * The read() method for PictureFrame reads the MIME type, picture
			 * type, description, and picture data from the stored frame bytes.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The image MIME type.
			 * 
			 * @see ID3::PictureFrame::mimeType()
			 */
			std::string textMIME;
			
			/**
			 * The picture type defined in the APIC frame standard.
			 * 
			 * @see ID3::PictureFrame::pictureType()
			 */
			PictureType APICType;
			
			/**
			 * The image description.
			 * 
			 * @see ID3::PictureFrame::description()
			 */
			std::string textDescription;
			
			/**
			 * The PNG or JPG image, saved as a uint8_t vector.
			 * 
			 * @see ID3::PictureFrame::picture()
			 */
			ByteArray pictureData;
	};
}

#endif
