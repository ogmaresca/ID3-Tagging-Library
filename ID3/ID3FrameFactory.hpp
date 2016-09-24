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

#ifndef ID3_FRAME_FACTORY_HPP
#define ID3_FRAME_FACTORY_HPP

#include <fstream>       //For std::ifstream
#include <string>        //For std::string
#include <unordered_map> //For std::unordered_map and std::pair
#include <memory>        //For std::shared_ptr

#include "Frames/ID3Frame.hpp"        //For the Frame class
#include "Frames/ID3PictureFrame.hpp" //For the PictureType enum
#include "ID3FrameID.hpp"             //For the FrameID class

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
	 * @see ID3.h
	 */
	typedef std::shared_ptr<Frame> FramePtr;
	
	/**
	 * @see ID3.h
	 */
	typedef std::pair<FrameID, FramePtr> FramePair;
	
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
			 * The protected constructor to create a FrameFactory.
			 * 
			 * @param file    An fstream file object.
			 * @param version The ID3 major version to use.
			 * @param tagEnd  The byte position that the ID3v2 tags end on. It is
			 *                assumed that the tag size has already been checked to
			 *                be smaller than the filesize.
			 */
			FrameFactory(std::istream&  file,
			             const ushort   version,
			             const ulong    tagEnd);
			
			/**
			 * The empty constructor.
			 * 
			 * NOTE: If creating a FrameFactory with this constructor, then the
			 *       create(ulong) method and its associated FramePair method will
			 *       always return "null" UnknownFrame objects. The FrameFactory
			 *       will also use the ID3v2 write version as its version reference.
			 */
			FrameFactory();
			
			/**
			 * Creates a Frame by reading from the given position on the file
			 * passed in the constructor.
			 * 
			 * NOTE: The passed file object must not be closed or null, and the
			 *       FrameFactory must not have been created with the empty
			 *       constructor, or a "null" UnknownFrame will be returned. The
			 *       read position must also be smaller than the ID3v2 tag size.
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
			inline FramePair createPair(const ulong readpos) const {
				FramePtr frame = create(readpos);
				return FramePair(frame->frame(), frame);
			}
			
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
			FramePtr create(const FrameID&     frameName,
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
			 * @see ID3::FrameFactory::create(FrameID&,
			 *                                std::string&,
			 *                                std::string&,
			 *                                std::string&)
			 */
			inline FramePair createPair(const FrameID& frameName,
			                            const std::string& textContent="",
			                            const std::string& description="",
			                            const std::string& language="") const {
				return FramePair(frameName, create(frameName, textContent, description, language));
			}
			
			/**
			 * Creates a text-content Frame. If the Frame ID is not a
			 * valid ID for an ID3 frame with string content, then a
			 * "null" UnknownFrame will be returned.
			 * 
			 * @param frameName    The ID3 frame ID.
			 * @param textContents The string contents.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages.
			 * @return A FramePtr containing a relevant Frame object.
			 */
			FramePtr create(const FrameID&                  frameName,
			                const std::vector<std::string>& textContents,
			                const std::string&              description="",
			                const std::string&              language="") const;
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @param frameName    The ID3 frame ID.
			 * @param textContents The string contents.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages.
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::FrameFactory::create(FrameID&,
			 *                                std::vector<std::string>&)
			 */
			inline FramePair createPair(const FrameID&                  frameName,
			                            const std::vector<std::string>& textContents,
			                            const std::string&              description,
			                            const std::string&              language) const {
				return FramePair(frameName, create(frameName, textContents, description, language));
			}
			
			/**
			 * Creates a text content or numerical Frame. If the Frame ID is not a
			 * valid ID for an ID3 frame with numerical or string content, then a
			 * "null" UnknownFrame will be returned.
			 * 
			 * @param frameName   The ID3 frame ID.
			 * @param frameValue  The numerical frame value.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages. 
			 * @return A FramePtr containing a relevant Frame object.
			 */
			FramePtr create(const FrameID&     frameName,
			                const long long    frameValue,
			                const std::string& description="",
			                const std::string& language="") const;
			
			/**
			 * Creates a relevant FramePair object.
			 * 
			 * @param frameName   The ID3 frame ID.
			 * @param frameValue  The numerical frame value.
			 * @param description The frame description. Only applies to text
			 *                    frames with descriptions.
			 * @param language    The frame language. Only applied to text frames
			 *                    with languages. 
			 * @return A FramePair, with the Frame ID in the first slot and the
			 *         FramePtr in the second slot.
			 * @see ID3::FrameFactory::create(FrameID&,
			 *                                long long&,
			 *                                std::string&,
			 *                                std::string&)
			 */
			inline FramePair createPair(const FrameID&     frameName,
			                            const long long    frameValue,
			                            const std::string& description="",
			                            const std::string& language="") const {
				return FramePair(frameName, create(frameName, frameValue, description, language));
			}
			
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
			
			/** @see ID3::Frame::Factory::createPicture(ByteArray&,
			 *                                          std::string&,
			 *                                          std::string&,
			 *                                          PictureType) */
			inline FramePair createPicturePair(const ByteArray&   pictureByteArray,
			                                   const std::string& mimeType,
			                                   const std::string& description="",
			                                   const PictureType  type=PictureType::FRONT_COVER) const {
				FramePtr frame = createPicture(pictureByteArray, mimeType, description, type);
				return FramePair(frame->frame(), frame);
			}
			
			/**
			 * Create a play count frame.
			 * 
			 * @param count The play count.
			 * @return A FramePtr with the relevant PictureFrame object.
			 */
			FramePtr createPlayCount(const unsigned long long count) const;
			
			/** @see ID3::FrameFactory::createPlayCount(unsigned long long) */
			inline FramePair createPlayCountPair(const unsigned long long count) const {
				FramePtr frame = createPlayCount(count);
				return FramePair(frame->frame(), frame);
			}
			
			/**
			 * Create a Popularimeter frame.
			 * 
			 * @param count  The play count.
			 * @param rating The 1-5 star rating.
			 * @param email  The email address.
			 * @return A FramePtr with the relevant PictureFrame object.
			 */
			FramePtr createPlayCount(const unsigned long long count,
			                         const uint8_t            rating,
			                         const std::string&       email) const;
			
			/** @see ID3::FrameFactory::createPlayCount(unsigned long long,
			 *                                          uint8_t,
			 *                                          std::string&) */
			inline FramePair createPlayCountPair(const unsigned long long count,
			                              const uint8_t            rating,
			                              const std::string&       email) const {
				FramePtr frame = createPlayCount(count, rating, email);
				return FramePair(frame->frame(), frame);
			}
		
		private:
			/**
			 * A private constructor used in static create() method calls to create
			 * frames with content given in the method's parameter.
			 * 
			 * @param The ID3v2 major version.
			 */
			explicit FrameFactory(const ushort version);
			
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
			 * A pointer to the istream object given in the protected constructor.
			 */
			std::istream* musicFile;
			
			/**
			 * The ID3v2 major version given in the constructor.
			 */
			ushort ID3Ver;
			
			/**
			 * The size of the ID3 tags in bytes, given in the public constructor.
			 */
			ulong ID3Size;
	};
}

#endif
