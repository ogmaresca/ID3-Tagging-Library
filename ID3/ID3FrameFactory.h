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

#ifndef ID3_FRAME_FACTORY_H
#define ID3_FRAME_FACTORY_H

#include <fstream>       //For std::ifstream
#include <string>        //For std::string
#include <unordered_map> //For std::unordered_map and std::pair
#include <memory>        //For std::shared_ptr

#include "ID3Frame.h"
#include "ID3PictureFrame.h"
#include "ID3FrameID.h"

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * ID3v2.3.0 standard: http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: http://id3.org/id3v2.4.0-structure
 * 
 * @see ID3.h
 */
namespace ID3 {
	/**
	 * FrameFactory is a factory class to create Frame objects.
	 * After creating a FrameFactory object call create(), or call a static
	 * create() method instead. createPair() can also be called instead as a
	 * shortcut method if you want to insert the Frame into an ID3::FrameMap.
	 * 
	 * NOTE: Whenever a call to create() has invalid parameter values, a FramePtr
	 * holding a "null" UnknownFrame will be returned. Even if the passed
	 * parameter values are valid, the returned Frame may still be "null" and/or
	 * an UnknownFrame.
	 * 
	 * NOTE: ID3 version values are not checked to ensure they fall
	 * within supported ranges. If the given version is not supported,
	 * it will be treated as ID3::MIN_SUPPORTED_VERSION if smaller or
	 * ID3::MAX_SUPPORTED_VERSION if bigger.
	 * 
	 * NOTE: Although a FrameFactory object holds an fstream file object, it will
	 * not close the file upon destruction. You must close the file manually,
	 * after you are finished with the FrameFactory object.
	 */
	class FrameFactory {
		protected:
			friend class Tag;
			
			/**
			 * The public constructor to create a FrameFactory.
			 * 
			 * @param file    An fstream file object.
			 * @param version The ID3 major version to use.
			 * @param tagEnd  The byte position that the ID3v2 tags end on. It is
			 *                assumed that the tag size has already been checked to
			 *                be smaller than the filesize.
			 */
			FrameFactory(std::ifstream& file,
			             const ushort   version,
			             const ulong    tagEnd);
			
			/**
			 * Creates a Frame by reading from the given position on the file
			 * passed in the constructor.
			 * 
			 * NOTE: The passed file object must not be closed or null, or a "null"
			 *       UnknownFrame will be returned. The read position must also be
			 *       smaller than the ID3v2 tag size.
			 * 
			 * @param readpos The position on the file to start reading from.
			 * @return A FramePtr containing a relevant Frame object.
			 */
			FramePtr create(const ulong readpos) const;
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @param readpos The position on the file to start reading from.
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::FrameFactory::create(const ulong)
			 */
			FramePair createPair(const ulong readpos) const;
			
			/**
			 * Creates a text-content Frame. If the Frame ID is not a
			 * valid ID for an ID3 frame with string content, then a
			 * "null" UnknownFrame will be returned.
			 * 
			 * @param frameName   The ID3 frame ID.
			 * @param textContent The string content.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages. 
			 * @return A FramePtr containing a relevant Frame object.
			 */
			FramePtr create(const FrameID& frameName,
			                const std::string& textContent="",
			                const std::string& description="",
			                const std::string& language="") const;
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @param frameName   The ID3 frame ID.
			 * @param textContent The string content.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages. 
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::FrameFactory::create(std::string&,
			 *                                std::string&,
			 *                                std::string&,
			 *                                std::string&)
			 */
			FramePair createPair(const FrameID& frameName,
			                     const std::string& textContent="",
			                     const std::string& description="",
			                     const std::string& language="") const;
			
			/**
			 * Create a picture Frame.
			 * 
			 * @param pictureByteArray A uint8_t vector of the picture's bytes.
			 * @param mimeType         The MIME type of the image (PNG or JPEG only).
			 * @param description      The image description (optional).
			 * @param type             The ID3v2 APIC type (optional, defaults to
			 *                         front cover).
			 * @return A FramePtr with the relevant PictureFrame object.
			 */
			FramePtr createPicture(const ByteArray&   pictureByteArray,
			                       const std::string& mimeType,
			                       const std::string& description="",
			                       const PictureType  type=PictureType::FRONT_COVER) const;
			
			/**
			 * Create a picture Frame pair.
			 * 
			 * @param pictureByteArray A uint8_t vector of the picture's bytes.
			 * @param mimeType         The MIME type of the image (PNG or JPEG only).
			 * @param description      The image description (optional).
			 * @param type             The ID3v2 APIC type (optional, defaults to
			 *                         front cover).
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::Frame::Factory::createPicture(ByteArray&,
			 *                                         std::string&,
			 *                                         std::string&,
			 *                                         PictureType)
			 */
			FramePair createPicturePair(const ByteArray&   pictureByteArray,
			                            const std::string& mimeType,
			                            const std::string& description="",
			                            const PictureType  type=PictureType::FRONT_COVER) const;
			
			/**
			 * Creates a Frame by reading from the given position on the file
			 * passed in the constructor.
			 * 
			 * NOTE: The passed file object must not be closed or null, or a "null"
			 *       UnknownFrame will be returned. The read position must also be
			 *       smaller than the ID3v2 tag size.
			 * 
			 * NOTE: The file object will not be closed during the method call.
			 * 
			 * @param file An fstream file object.
			 * @param version The ID3 major version to use.
			 * @param tagEnd  The byte position that the ID3v2 tags end on. It is
			 *                assumed that the tag size has already been checked to
			 *                be smaller than the filesize.
			 * @param readpos The position on the file to start reading from.
			 * @return A FramePtr containing a relevant Frame object.
			 */
			static FramePtr create(std::ifstream& file,
			                       const ulong    readpos,
			                       const ushort   version,
			                       const ulong    tagEnd);
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @param file An fstream file object.
			 * @param version The ID3 major version to use.
			 * @param tagEnd  The byte position that the ID3v2 tags end on. It is
			 *                assumed that the tag size has already been checked to
			 *                be smaller than the filesize.
			 * @param readpos The position on the file to start reading from.
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::FrameFactory::create(std::ifstream&, ulong, ushort, ulong).
			 */
			static FramePair createPair(std::ifstream& file,
			                            const ulong    readpos,
			                            const ushort   version,
			                            const ulong    tagEnd);
			
			/**
			 * Creates a text-content Frame. If the Frame ID is not a valid ID for
			 * an ID3 frame with string content, then a "null" UnknownFrame will be
			 * returned.
			 * 
			 * @param frameName   The ID3 frame ID.
			 * @param version     The ID3 major version to use.
			 * @param textContent The string content.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applies to text frames
			 *                    with languages. 
			 * @return A FramePtr containing a relevant Frame object.
			 */
			static FramePtr create(const FrameID&     frameName,
			                       const ushort       version,
			                       const std::string& textContent="",
			                       const std::string& description="",
			                       const std::string& language="");
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @see ID3::FrameFactory::create(std::string&,
			 *                                ushort,
			 *                                std::string&,
			 *                                std::string&)
			 * @param frameName   The ID3v2 frame ID.
			 * @param version     The ID3 major version to use.
			 * @param textContent The string content.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages. 
			 * @return A FramePair, with the Frame ID in the first slot
			 *         and the FramePtr in the second slot.
			 */
			static FramePair createPair(const FrameID&     frameName,
			                            const ushort       version,
			                            const std::string& textContent="",
			                            const std::string& description="",
			                            const std::string& language="");
			
			/**
			 * Create a picture Frame.
			 * 
			 * @param version          The ID3v2 major version.
			 * @param pictureByteArray A uint8_t vector of the picture's bytes.
			 * @param mimeType         The MIME type of the image (PNG or JPEG only).
			 * @param description      The image description (optional).
			 * @param type             The ID3v2 APIC type (optional, defaults to
			 *                         front cover).
			 * @return A FramePtr with the relevant PictureFrame object.
			 */
			static FramePtr createPicture(const ushort       version,
			                              const ByteArray&   pictureByteArray,
			                              const std::string& mimeType,
			                              const std::string& description="",
			                              const PictureType  type=PictureType::FRONT_COVER);
			
			/**
			 * Create a picture Frame pair.
			 * 
			 * @param version          The ID3v2 major version.
			 * @param pictureByteArray A uint8_t vector of the picture's bytes.
			 * @param mimeType         The MIME type of the image (PNG or JPEG only).
			 * @param description      The image description (optional).
			 * @param type             The ID3v2 APIC type (optional, defaults to
			 *                         front cover).
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::Frame::Factory::createPicture(ushort,
			 *                                         ByteArray&,
			 *                                         std::string&,
			 *                                         std::string&,
			 *                                         PictureType)
			 */
			static FramePair createPicturePair(const ushort       version,
			                                   const ByteArray&   pictureByteArray,
			                                   const std::string& mimeType,
			                                   const std::string& description="",
			                                   const PictureType  type=PictureType::FRONT_COVER);
		
		private:
			/**
			 * A private constructor used in static create() method calls to create
			 * frames with content given in the method's parameter.
			 * 
			 * @param The ID3v2 major version.
			 */
			FrameFactory(const ushort version);
			
			/**
			 * An internal method used to match each ID3 frame ID with
			 * a FrameClass enum value. If the frame ID is not recognized
			 * a FrameClass::CLASS_UNKNOWN value will be returned.
			 * 
			 * @param frameID The ID3v2 frame ID.
			 * @return The relevant FrameClass enum value.
			 */
			static FrameClass frameType(const FrameID& frameID);
			
			/**
			 * Some frames that are read with integer options specific to their
			 * frame description in the ID3v2 standard. Use this function to get
			 * the option for a specific frame.
			 * 
			 * @param frameID The ID3v2 frame ID.
			 * @return The relevant DescriptiveTextFrame option value.
			 */
			static ushort frameOptions(const FrameID& frameID);
			
			/**
			 * A pointer to the fstream object given in the public constructor.
			 */
			std::ifstream* musicFile;
			
			/**
			 * The ID3v2 major version given in the constructor.
			 */
			const ushort ID3Ver;
			
			/**
			 * The size of the ID3 tags in bytes, given in the public constructor.
			 */
			const ulong ID3Size;
	};
}

#endif
