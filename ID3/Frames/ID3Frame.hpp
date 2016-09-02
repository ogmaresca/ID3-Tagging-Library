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

#ifndef ID3_FRAME_HPP
#define ID3_FRAME_HPP

#include <vector> //For std::vector
#include <string> //For std::string

#include "../ID3FrameID.hpp" //For the FrameID class

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
	typedef std::vector<uint8_t> ByteArray;
	
	/**
	 * An enum that  represents a Frame class.
	 * 
	 * Values and their respective class:
	 *     CLASS_DESCRIPTIVE:   DescriptiveTextFrame
	 *     CLASS_NUMERICAL:     NumericalTextFrame
	 *     CLASS_PICTURE:       PictureFrame
	 *     CLASS_PLAY_COUNT:    PlayCountFrame
	 *     CLASS_POPULARIMETER: PopularimeterFrame
	 *     CLASS_TEXT:          TextFrame
	 *     CLASS_UNKNOWN:       UnknownFrame
	 *     CLASS_URL:           URLTextFrame
	 */
	enum FrameClass : short {
		CLASS_DESCRIPTIVE   = 3, //DescriptiveTextFrame
		CLASS_NUMERICAL     = 2, //NumericalTextFrame
		CLASS_TEXT          = 1, //TextFrame
		CLASS_UNKNOWN       = 0, //UnknownFrame
		CLASS_URL           = 4, //URLTextFrame
		CLASS_PICTURE       = 5, //PictureTextFrame
		CLASS_PLAY_COUNT    = 6, //PlayCountFrame
		CLASS_POPULARIMETER = 7, //PopularimeterFrame
		CLASS_EVENT_TIMING  = 8  //EventTimingFrame
	};
	
	/**
	 * An enum of text encodings used in ID3v2 frames.
	 */
	enum FrameEncoding : uint8_t {
		ENCODING_LATIN1   = 0, //AKA ISO-8859-1
		ENCODING_UTF16BOM = 1, //AKA UCS-2
		ENCODING_UTF16    = 2, //ID3v2.4+ only, ID3-Tagging-Library will read in ID3v2.3
		ENCODING_UTF8     = 3  //ID3v2.4+ only, ID3-Tagging-Library will read in ID3v2.3
	};
	
	/**
	 * A 10-bit struct that captures the structure of the ID3v2 frame header.
	 */
	struct FrameHeader {
		char id[4];
		uint8_t size[4]; //A synchsafe integer in ID3v2.4+
		uint8_t flags1;
		uint8_t flags2;
	};
	
	/**
	 * A 6-bit struct that captures the structure of the ID3v2.2 frame header.
	 */
	struct V2FrameHeader {
		char id[3];
		uint8_t size[3];
	};
	
	/**
	 * An enum of different frame flags that can be set on a frame.
	 * 
	 * Used in ID3::Frame::flag(FrameFlag).
	 */
	enum class FrameFlag : uint8_t {
		DISCARD_UPON_TAG_ALTER_IF_UNKNOWN,
		DISCARD_UPON_AUDIO_ALTER,
		READ_ONLY,
		COMPRESSED,
		ENCRYPTED,
		GROUPING_IDENTITY,
		UNSYNCHRONISED,
		DATA_LENGTH_INDICATOR
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// F R A M E /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * An ID3::Frame stores information from an ID3v2 frame.
	 * It is an abstract class, use one of its children defined below
	 * to create a Frame.
	 * 
	 * Frame flag descriptions taken from http://id3.org/id3v2.4.0-structure.
	 * 
	 * @abstract
	 */
	class Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * This flag tells the software what to do with this frame if it is
			 * unknown and the tag is altered in any way. This applies to all kinds
			 * of alterations, including adding more padding and reordering the frames.
			 * This flag is found on the first frame flag byte.
			 */
			static const uint8_t FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V3 = 0b10000000;
			static const uint8_t FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4 = 0b01000000;
			
			/**
			 * This flag tells the software what to do with this frame if it is
			 * unknown and the file, excluding the tag, is altered. This does not
			 * apply when the audio is completely replaced with other audio data.
			 * This flag is found on the first frame flag byte.
			 */
			static const uint8_t FLAG1_DISCARD_UPON_AUDIO_ALTER_V3 = 0b01000000;
			static const uint8_t FLAG1_DISCARD_UPON_AUDIO_ALTER_V4 = 0b00100000;
			
			/**
			 * This flag, if set, tells the software that the contents of this
			 * frame is intended to be read only. Changing the contents might break
			 * something, e.g. a signature. If the contents are changed, without
			 * knowledge in why the frame was flagged read only and without taking
			 * the proper means to compensate, e.g. recalculating the signature,
			 * the bit should be cleared.
			 * This flag is found on the first frame flag byte.
			 */
			static const uint8_t FLAG1_READ_ONLY_V3 = 0b00100000;
			static const uint8_t FLAG1_READ_ONLY_V4 = 0b00010000;
			
			/**
			 * This flag indicates whether or not the frame is compressed using zlib.
			 * If true, 4 bytes are added to the frame header.
			 * The 4 bytes are included in the frame's size in the header.
			 * This flag is found on the second frame flag byte.
			 */
			static const uint8_t FLAG2_COMPRESSED_V3 = 0b10000000;
			static const uint8_t FLAG2_COMPRESSED_V4 = 0b00001000;
			
			/**
			 * This flag indicates wether or not the frame is enrypted. If set one
			 * byte indicating with which method it was encrypted will be appended
			 * to the frame header. The byte follows the compressed flag bytes,
			 * if the compressed flag is set.
			 * The encryption byte is included in the frame's size in the header.
			 * This flag is found on the second frame flag byte.
			 */
			static const uint8_t FLAG2_ENCRYPTED_V3 = 0b01000000;
			static const uint8_t FLAG2_ENCRYPTED_V4 = 0b00000100;
			
			/**
			 * This flag indicates whether or not this frame belongs in a group
			 * with other frames. If set a group identifier byte is added to the
			 * frame header. Every frame with the same group identifier belongs to
			 * the same group. If the compressed and/or encrypted flags are set,
			 * the group identifier byte follows the compressed and encrypted bytes.
			 * The grouping identity byte is included in the frame's size in the header.
			 * This flag is found on the second frame flag byte.
			 */
			static const uint8_t FLAG2_GROUPING_IDENTITY_V3 = 0b00100000;
			static const uint8_t FLAG2_GROUPING_IDENTITY_V4 = 0b01000000;
			
			/**
			 * This flag indicates whether or not unsynchronisation was applied to
			 * this frame. If this flag is set all data from the end of this header
			 * to the end of this frame has been unsynchronised. Although
			 * desirable, the presence of a 'Data Length Indicator' is not made
			 * mandatory by unsynchronisation.
			 * ID3v2.4 only.
			 */
			static const uint8_t FLAG2_UNSYNCHRONISED_V4 = 0b00000010;
			
			/**
			 * This flag indicates that a data length indicator has been added to
			 * the frame. The data length indicator is the value one would write as
			 * the 'Frame length' if all of the frame format flags were zeroed,
			 * represented as a 32 bit synchsafe integer.
			 * ID3v2.4 only.
			 */
			static const uint8_t FLAG2_DATA_LENGTH_INDICATOR_V4 = 0b00000001;
			
			/**
			 * The destructor.
			 */
			virtual ~Frame();
			
			/**
			 * Check if two Frames are equal.
			 * 
			 * This method is to be implemented in child classes.
			 * This method should only return true if the frame ID, class, "null"
			 * status, and if frame content match (when neither of the Frames
			 * are "null").
			 * 
			 * @param frame The Frame to compare against.
			 * @returns If the Frame is equal to this object.
			 * @abstract
			 */
			virtual bool operator==(const Frame* const frame) const noexcept = 0;
			
			/**
			 * Check if the Frame is the same type as the given FrameClass.
			 * 
			 * @param classID A FrameClass enum value.
			 * @return true if it's a matching FrameClass, false otherwise.
			 * @see ID3::Frame::type()
			 */
			bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Return a FrameClass enum value that is associated with its class.
			 * 
			 * @see ID3::Frame::type()
			 */
			operator FrameClass() const noexcept;
			
			/**
			 * Returns true if the frame was read/created succesfully, false if not.
			 * 
			 * @see ID3::Frame::null()
			 */
			bool operator==(bool boolean) const noexcept;
			 
			/**
			 * Get the FrameClass enum value that is associated with its class.
			 * This method is to be implemented in child classes.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @abstract
			 */
			virtual FrameClass type() const noexcept = 0;
			
			/**
			 * Get the content of the frame as bytes.
			 * 
			 * @see ID3::Frame::bytes()
			 */
			operator ByteArray() const noexcept;
			
			/**
			 * Use this method to determine if the Frame was succesfully created
			 * or not.
			 * 
			 * @return true if the frame was read/created succesfully, false if not.
			 */
			bool null() const;
			
			/**
			 * Get the size of the frame.
			 * The frame header size can be retrieved at ID3::Frame::headerSize().
			 * 
			 * NOTE: If the Frame was created by reading from a file, then this
			 *       will always return 0 until write() is called.
			 * 
			 * @param header If true, the header size will be included in the size.
			 * @return The size of the frame.
			 */
			ulong size(bool header=false) const;
			
			/**
			 * Get the ID of the frame (ex: "TIT2"/Frames::FRAME_TITLE for titles).
			 * 
			 * @return The frame ID.
			 */
			FrameID frame() const;
			
			/**
			 * Get the content of the frame as bytes.
			 * 
			 * @param header If true, the frame header will be included in the
			 *               returned ByteArray as well.
			 * @return The frame content.
			 */
			ByteArray bytes(bool header=false) const noexcept;
			
			/**
			 * Revert any changes made to the frame since it was last
			 * read, created, or written.
			 */
			void revert();
			
			/**
			 * Check if the Frame has been edited. Call revert() to undo
			 * any changes or save() to save them.
			 * 
			 * @return true if the Frame has been edited, false if not.
			 */
			bool edited() const;
			
			/**
			 * Check how the Frame was created.
			 * 
			 * @return true if the constructor with an file object was called,
			 *         false if not.
			 */
			bool createdFromFile() const;
			
			/**
			 * Check if the Frame's content is empty.
			 * This method is to be implemented in child classes.
			 * 
			 * @return true if the content is empty, false otherwise.
			 * @abstract
			 */
			virtual bool empty() const = 0;
			
			/**
			 * Check if a flag is set.
			 * 
			 * @param flag The frame flag to check.
			 * @return If the flag is set.
			 * @see ID3::FrameFlag
			 */
			bool flag(const FrameFlag flag) const;
			
			/**
			 * Get the grouping identity. If the grouping identity flag is not set,
			 * this will always return 0.
			 * 
			 * @return The group identity.
			 * @see ID3::Frame::groupingIdentity()
			 */
			uint8_t groupIdentity() const;
			
			/**
			 * Get the size of the frame header. This will be HEADER_BYTE_SIZE if
			 * the grouping identity flag is not set, and HEADER_BYTE_SIZE+1
			 * if it is.
			 * 
			 * @return The size of the frame header.
			 */
			ushort headerSize() const;
			
			/**
			 * Print information about the Frame.
			 */
			virtual void print() const;
			
			/**
			 * Save any changes made to the frame, and get the updated content of
			 * the frame in bytes.
			 * 
			 * Upon calling this method, the internal ID3v2 major verision gets
			 * changed to ID3::WRITE_VERSION (ID3v2.4.0).
			 * 
			 * The only flags that are preserved by this method is the grouping
			 * identity.
			 * 
			 * @return The new content of the frame, in bytes.
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor initializes the relevant variables with
			 * the passed-in variables. Calling this constructor will
			 * set ID3::Frame::isFromFile to true.
			 * isNull will be set to true if the number of bytes in frameBytes is
			 * fewer than or equal to the amount of bytes as HEADER_BYTE_SIZE.
			 * It will also be set true if the frame is compressed, or encrypted.
			 * If the frame is synchronised, then it will be unsynchronised.
			 * Call read() in children after calling this constructor to get the
			 * frame contents.
			 * 
			 * NOTE: frameBytes MUST include the frame header.
			 * 
			 * NOTE: The version is not checked to see if it is a
			 *       supported ID3v2 major version.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param frameBytes The content of the frame in bytes.
			 */
			Frame(const FrameID& frameName,
			      const ushort version,
			      const ByteArray& frameBytes);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" Frame object if the frame
			 * ID is the unknown frame.
			 * 
			 * @param frameName The frame ID (defaults to Unknown Frame)
			 */
			Frame(const FrameID& frameName=Frames::FRAME_UNKNOWN_FRAME) noexcept;
			
			/**
			 * Read and process the bytes of an ID3v2 frame. This method should
			 * read from the frameContents variable. The frame header must be
			 * included in frameContents. The ID3v2 major version used will be the
			 * value saved in the ID3Ver variable.
			 * This method is to be implemented in child classes.
			 * 
			 * @abstract
			 */
			virtual void read() = 0;
			
			/**
			 * The writeBody() method is called after building the frame header
			 * in frameContents. The frame data should be appended to frameContents
			 * in this method. The frame size will be set in write() after calling
			 * writeBody().
			 * 
			 * This method will not be called if the Frame is empty or null.
			 * 
			 * This method is to be implemented in child classes.
			 * 
			 * @abstract
			 */
			virtual void writeBody() = 0;
			
			/**
			 * Unsynchronise frame byte contents. This checks for the
			 * unsynchronisation frame flag to be set first, so it only supports
			 * ID3v2.4+ frames.
			 * This method is automatically called from Frame(std::string&, ushort,
			 * ByteArray&), and shouldn't be called elsewhere.
			 */
			void unsynchronise();
			
			/**
			 * The ID3v2 frame ID.
			 * 
			 * @see ID3::Frame::frame()
			 */
			FrameID id;
			
			/**
			 * This variable records the ID3 version used for reading
			 * and writing. It can be updated through ID3::Frame::write().
			 */
			ushort ID3Ver;
			
			/**
			 * This ByteArray records the bytes of the frame on file,
			 * including the frame header. This value will be updated
			 * after calling ID3::Frame::write().
			 */
			ByteArray frameContent;
			
			/**
			 * This variable records if the Frame is null.
			 * 
			 * @see ID3::Frame::null()
			 */
			bool isNull;
			
			/**
			 * A variable that records whether the Frame object has been
			 * modified or not.
			 * 
			 * @see ID3::Frame::edited()
			 */
			bool isEdited;
			
			/**
			 * This variable records if the Frame has been created from
			 * a file or not.
			 * 
			 * @see ID3::Frame::createdFromFile()
			 */
			bool isFromFile;
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////// U N K N O W N F R A M E //////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * UnknownFrame is a child class of Frame. It is to be used when
	 * a given frame ID is unknown.
	 * 
	 * @see ID3::Frame
	 */
	class UnknownFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is an UnknownFrame with the same frame ID,
			 * "null" status, and if frame content match (when neither of the
			 * Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_UNKNOWN.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. An UnknownFrame is empty if
			 * the size of the frame in bytes is <= HEADER_BYTE_SIZE.
			 * 
			 * @return true if the content is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for UnknownFrame overrides the write() method in
			 * Frame. The only changes that will be made is empting the frame if
			 * its null or if the discard unknown frames flag is set, and changing
			 * the frame size to be synchsafe is the ID3 major version is <= 3.
			 * 
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class, and sets isNull to false if the byte array
			 * is bigger than HEADER_BYTE_SIZE.
			 * 
			 * @see ID3::Frame::Frame(FrameID&,
			 *                        ushort,
			 *                        ByteArray&)
			 */
			UnknownFrame(const FrameID& frameName,
			             const ushort version,
			             const ByteArray& frameBytes);
			
			/**
			 * This constructor creates calls ID3::Frame::Frame() and creates a
			 * "null" frame, but also saves the frame ID.
			 * 
			 * @see ID3::Frame::Frame(FrameID&)
			 */
			UnknownFrame(const FrameID& frameName=Frames::FRAME_UNKNOWN_FRAME) noexcept;
			
			/**
			 * The read() method for UnknownFrame is an empty method.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
			
			/**
			 * The writeBody() method for UnknownFrame is an empty method.
			 * 
			 * @see ID3::Frame::writeBody()
			 */
			virtual void writeBody();
	};
}

#endif
