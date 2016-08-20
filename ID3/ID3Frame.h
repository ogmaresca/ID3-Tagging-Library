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

#ifndef ID3_FRAME_H
#define ID3_FRAME_H

#include <vector> //For std::vector
#include <string> //For std::string

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
	typedef std::vector<char> ByteArray;
	
	/**
	 * An enum that  represents a Frame class.
	 * 
	 * Values and their respective class:
	 *     TEXT:    TextFrame
	 *     UNKNOWN: UnknownFrame
	 */
	enum FrameClass {
		CLASS_DESCRIPTIVE = 3, //DescriptiveTextFrame
		CLASS_NUMERICAL   = 2, //NumericalTextFrame
		CLASS_TEXT        = 1, //TextFrame
		CLASS_UNKNOWN     = 0, //UnknownFrame
		CLASS_URL         = 4  //URLTextFrame
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
	 * Implemented in ID3Frame.cpp
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
			static const uint8_t FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN    = 0b10000000;
			static const uint8_t FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4 = 0b01000000;
			
			/**
			 * This flag tells the software what to do with this frame if it is
			 * unknown and the file, excluding the tag, is altered. This does not
			 * apply when the audio is completely replaced with other audio data.
			 * This flag is found on the first frame flag byte.
			 */
			static const uint8_t FLAG1_DISCARD_UPON_AUDIO_ALTER    = 0b01000000;
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
			static const uint8_t FLAG1_READ_ONLY    = 0b00100000;
			static const uint8_t FLAG1_READ_ONLY_V4 = 0b00010000;
			
			/**
			 * This flag indicates whether or not the frame is compressed using zlib.
			 * If true, 4 bytes are added to the frame header.
			 * The 4 bytes are included in the frame's size in the header.
			 * This flag is found on the second frame flag byte.
			 */
			static const uint8_t FLAG2_COMPRESSED    = 0b10000000;
			static const uint8_t FLAG2_COMPRESSED_V4 = 0b00001000;
			
			/**
			 * This flag indicates wether or not the frame is enrypted. If set one
			 * byte indicating with which method it was encrypted will be appended
			 * to the frame header. The byte follows the compressed flag bytes,
			 * if the compressed flag is set.
			 * The encryption byte is included in the frame's size in the header.
			 * This flag is found on the second frame flag byte.
			 */
			static const uint8_t FLAG2_ENCRYPTED    = 0b01000000;
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
			static const uint8_t FLAG2_GROUPING_IDENTITY    = 0b00100000;
			static const uint8_t FLAG2_GROUPING_IDENTITY_V4 = 0b01000000;
			
			/**
			 * This flag indicates whether or not unsynchronisation was applied to
			 * this frame. If this flag is set all data from the end of this header
			 * to the end of this frame has been unsynchronised. Although
			 * desirable, the presence of a 'Data Length Indicator' is not made
			 * mandatory by unsynchronisation.
			 * ID3v2.4 only.
			 */
			static const uint8_t FLAG2_UNSYNCHRONIZED_V4 = 0b00000010;
			
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
			 * The frame header size is stored at ID3::HEADER_BYTE_SIZE.
			 * 
			 * @return The size of the frame.
			 */
			unsigned long size() const;
			
			/**
			 * Get the string ID of the frame (ex: "TIT2" for titles).
			 * 
			 * @return The frame ID.
			 */
			std::string frame() const;
			
			/**
			 * Get the content of the frame as bytes.
			 * 
			 * @return The frame content, including the header.
			 */
			ByteArray bytes() const noexcept;
			
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
			 * @return If the "Tag alter preservation" flag is set.
			 * @see ID3::Frame::FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN
			 */
			bool discardUponTagAlterIfUnknown() const;
			
			/**
			 * @return If the "File alter preservation" flag is set.
			 * @see ID3::Frame::FLAG1_DISCARD_UPON_AUDIO_ALTER
			 */
			bool discardUponAudioAlter() const;
			
			/**
			 * @return If the "File alter preservation" flag is set.
			 * @see ID3::Frame::FLAG1_READ_ONLY
			 */
			bool readOnly() const;
			
			/**
			 * @return If the Read-only flag is set.
			 * @see ID3::Frame::FLAG2_COMPRESSED
			 */
			bool compressed() const;
			
			/**
			 * @return If the Encryption flag is set.
			 * @see ID3::Frame::FLAG2_ENCRYPTED
			 */
			bool encrypted() const;
			
			/**
			 * @return If the Grouping Identity flag is set.
			 * @see ID3::Frame::FLAG2_GROUPING_IDENTIY
			 */
			bool groupingIdentity() const;
			
			/**
			 * @return If the Unsynchronization flag is set.
			 * @see ID3::Frame::FLAG2_UNSYNCHRONIZED_V4
			 */
			bool unsynchronized() const;
			
			/**
			 * @return If the Data Length Indicator flag is set.
			 * @see ID3::Frame::FLAG2_DATA_LENGTH_INDICATOR_V4
			 */
			bool dataLengthIndicator() const;
			
			/**
			 * Get the grouping identity. If the grouping identity flag is not set,
			 * this will always return 0.
			 * 
			 * @return The group identity.
			 */
			uint8_t groupIdentity() const;
			
			/**
			 * Get the size of the frame header. This will be HEADER_BYTE_SIZE if
			 * the grouping identity flag is not set, and HEADER_BYTE_SIZE+1
			 * if it is.
			 * 
			 * @return The size of the frame header.
			 */
			short headerSize() const;
			
			/**
			 * Save any changes made to the frame, and get the updated content of
			 * the frame in bytes.
			 * This method is to be implemented in child classes.
			 * 
			 * @param version The ID3v2 major version to save the frame
			 *                contents as. If a value is not supplied,
			 *                or it is not supported, it will default
			 *                to using the ID3v2 major version supplied
			 *                during the object's creation.
			 * @param minimize By default, the write() method should not shrink the
			 *                 size of the frame if the modified content is
			 *                 smaller than the original content. If this is set to
			 *                 true, no padding will ever be added to the frame.
			 * @return The new content of the frame, in bytes.
			 * @abstract
			 */
			virtual ByteArray write(unsigned short version=0,
			                        bool minimize=false) = 0;
		
		protected:
			/**
			 * This constructor initializes the relevant variables with
			 * the passed-in variables. Calling this constructor will
			 * set ID3::Frame::isFromFile to true.
			 * isNull will be set to true if the number of bytes in frameBytes is
			 * fewer than or equal to the amount of bytes as HEADER_BYTE_SIZE.
			 * It will also be set true if the frame is compressed, encrypted, or.
			 * unsynchronized. Call read() in children after calling this
			 * constructor to get the frame contents.
			 * 
			 * NOTE: frameBytes MUST include the frame header.
			 * 
			 * NOTE: The version is not checked to see if it is a
			 *       supported ID3v2 major version.
			 * 
			 * @param frameName The frame ID string.
			 * @param version The ID3v2 major version.
			 * @param frameBytes The content of the frame in bytes.
			 */
			Frame(const std::string& frameName,
			      const unsigned short version,
			      ByteArray& frameBytes);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" Frame object.
			 */
			Frame() noexcept;
			
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
			 * This variable records if the Frame is null.
			 * 
			 * @see ID3::Frame::null()
			 */
			bool isNull;
			
			/**
			 * The ID3v2 frame ID.
			 * 
			 * @see ID3::Frame::frame()
			 */
			std::string id;
			
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
			
			/**
			 * This variable records the ID3 version used for reading
			 * and writing. It can be updated through ID3::Frame::write().
			 */
			unsigned short ID3Ver;
			
			/**
			 * This ByteArray records the bytes of the frame on file,
			 * including the frame header. This value will be updated
			 * after calling ID3::Frame::write().
			 */
			ByteArray frameContent;
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
	 * Implemented in ID3Frame.cpp
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
			 * The write() method for UnknownFrame will only do two things to the
			 * frame. If the flag to discard unknown frames when the tag is altered
			 * is set, then the Frame contents will be cleared. Additionally, if
			 * the version is a supported version and different from the version
			 * given in the constructor, the Frame's version will be updated to the
			 * given version and the frame size in the Frame's bytes will be
			 * modified to support the changes between ID3v2 that were made about
			 * whether the frame size is a synchsafe value or nnot.
			 * 
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0,
			                        bool minimize=false);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class, and sets isNull to false if the byte array
			 * is bigger than HEADER_BYTE_SIZE.
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        unsigned short,
			 *                        ByteArray&,
			 *                        unsigned long)
			 */
			UnknownFrame(const std::string& frameName,
			             const unsigned short version,
			             ByteArray& frameBytes);
			
			/**
			 * This constructor creates calls ID3::Frame::Frame() and
			 * creates a "null" frame, but also saves the frame ID.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			UnknownFrame(const std::string& frameName);
			
			/**
			 * @see ID3::Frame::Frame()
			 */
			UnknownFrame() noexcept;
			
			/**
			 * The read() method for UnknownFrame is an empty method.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////// T E X T F R A M E /////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * TextFrame is a child class of Frame. It is to be used for frames
	 * with text content.
	 * 
	 * Implemented in ID3TextFrame.cpp
	 * 
	 * @see ID3::Frame
	 */
	class TextFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a TextFrame with the same frame ID,
			 * "null" status, and if text content match (when neither of the Frames
			 * are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Checks if the given string is the same as the text content.
			 * 
			 * @param str The string to check.
			 * @return true if the strings are the same, false otherwise.
			 */
			virtual bool operator==(const std::string& str) const noexcept;
			
			/**
			 * Get the text content of the frame.
			 */
			virtual operator std::string() const noexcept;
			
			/**
			 * Append a string to the frame content.
			 * 
			 * @param str The string to append.
			 * @return The text frame.
			 */
			virtual TextFrame& operator+=(const std::string& str) noexcept;
			
			/**
			 * Returns FrameClass::CLASS_TEXT.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. An UnknownFrame is empty if
			 * the frame content is an empty string.
			 * 
			 * @return true if the content is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * The write() method for TextFrame re-creates the frame
			 * header with UTF-8 encoding and its stored text content.
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0,
			                        bool minimize=false);
			
			/**
			 * Get the text content.
			 * 
			 * @returns The text content of the frame in UTF-8 encoding.
			 */
			std::string content() const;
			
			/**
			 * Set the text content. Call write() to finalize changes.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void content(const std::string& newContent);
			
			/**
			 * Get a ByteArray encoded in either LATIN-1, UTF-8, or UTF-16, and
			 * return the string as a UTF-8 encoded string.
			 * This function uses utf16toutf8() to encode UTF-16 strings to UTF-8,
			 * and ID3::latin1toutf8() to encode LATIN-1 strings to UTF-8.
			 * 
			 * @param encoding A char whose int values are represented by the
			 *                 enum ID3::FrameEncoding. If the encoding is unknown
			 *                 it will default to LATIN-1.
			 * @param bytes The char vector that contains the string you wish to
			 *              encode.
			 * @param start The byte position in the ByteArray to start reading
			 *              (optional). If not given a value or given a negative
			 *              value, it will default to 0.
			 * @param end The byte position + 1 in the ByteArray to end reading
			 *            (optional). If not given or given a negative value, it
			 *            will default to the end of the ByteArray. If it is longer
			 *            than the length of the ByteArray, the function will stop
			 *            at the end of the ByteArray.
			 * @return The UTF-8 encoded string.
			 */
			static std::string readStringAsUTF8(char encoding,
			                                    const ByteArray& bytes,
			                                    long start=-1,
			                                    long end=-1);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long
			 * enough to be valid it then sets isNull to false and calls
			 * ID3::TextFrame::read(ByteArray&) to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid text frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        unsigned short,
			 *                        ByteArray&,
			 *                        unsigned long)
			 */
			TextFrame(const std::string& frameName,
			          const unsigned short version,
			          ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 */
			TextFrame(const std::string& frameName,
			          const unsigned short version,
			          const std::string& value="");
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" TextFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			TextFrame() noexcept;
			
			/**
			 * The content of the frame.
			 * 
			 * @see ID3::TextFrame::content()
			 */
			std::string textContent;
			
			/**
			 * The read() method for TextFrame first gets the text encoding of the
			 * frame at the 11th byte, and saves every following byte in the
			 * ByteArray as its content string. If the text is not encoded in
			 * UTF-8, it is converted to UTF-8 before saving it.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	//////////////////// N U M E R I C A L T E X T F R A M E ////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * NumericalTextFrame is a child class of TextFrame. It is to be used for
	 * frames with numerical content, but the numbers are represented as a
	 * text encoding instead of its bit values. A NumericalTextFrame will always
	 * check its content when set to see if it's a valid numerical value, and if
	 * it isn't then it will set the empty string instead.
	 * 
	 * Implemented in ID3TextFrame.cpp
	 * 
	 * @see ID3::Frame
	 * @see ID3::TextFrame
	 */
	class NumericalTextFrame : virtual public TextFrame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a NumericalTextFrame with the same,
			 * frame ID, "null" status, and if text content match (when neither of
			 * the Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Checks if the given integer is the same value as the text content.
			 * 
			 * @param val The long int to check.
			 * @return true if the values are the same, false otherwise.
			 */
			virtual bool operator==(long val) const noexcept;
			
			/**
			 * Get the text content of the frame casted to a long int.
			 */
			virtual operator long() const noexcept;
			
			/**
			 * Adds an int value to the frame content.
			 * 
			 * @param val The value to add.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator+=(long val) noexcept;
			
			/**
			 * Subtracts an int value from the frame content.
			 * 
			 * @param val The value to subtract.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator-=(long val) noexcept;
			
			/**
			 * Multiplies an the frame content by an int value.
			 * 
			 * @param val The value to multiply the frame content by.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator*=(long val) noexcept;
			
			/**
			 * Divides the frame content by an int value.
			 * 
			 * @param val The value to divide the frame content by.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator/=(long val) noexcept;
			
			/**
			 * Mods the frame content by an int value.
			 * 
			 * @param val The value to mod the frame content by.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator%=(long val) noexcept;
			
			/**
			 * Append a string to the frame content.
			 * 
			 * NOTE: If the string is not an int value, nothing will be added and,
			 * if the NumericalTextFrame object hasn't been edited before, edited()
			 * will continue to return false.
			 * 
			 * NOTE: This method concatenates the frame content with the given
			 * string. If you wish to perform a mathematical addition to the frame
			 * content, use ID3::NumericalTextFrame::operator+=(long) instead.
			 * 
			 * @param str The value to append.
			 * @return The text frame.
			 */
			virtual NumericalTextFrame& operator+=(const std::string& str) noexcept;
			
			/**
			 * Returns FrameClass::CLASS_NUMERICAL.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * Set the numerical content. Call write() to finalize changes.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * @param newContent The new numerical content.
			 */
			virtual void content(long newContent);
			
			/**
			 * Set the text content. Call write() to finalize changes.
			 * 
			 * NOTE: If the string is not an int value, then the NumericalTextFrame
			 *       object will store an empty string instead.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void content(const std::string& newContent);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * TextFrame class. If the given ByteArray is long
			 * enough to be valid it then sets isNull to false and calls
			 * ID3::NumericalTextFrame::read(ByteArray&) to process the ByteArray.
			 * 
			 * NOTE: If the content string in the ByteArray is not an int value,
			 *       then the NumericalTextFrame object will store an empty string
			 *       instead.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid numerical text frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        unsigned short,
			 *                        ByteArray&,
			 *                        unsigned long)
			 */
			NumericalTextFrame(const std::string& frameName,
			                   const unsigned short version,
			                   ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: If the string is not an int value, then the NumericalTextFrame
			 *       object will store an empty string instead.
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 * @see ID3::TextFrame::TextFrame(std::string&,
			 *                                unsigned short,
			 *                                std::string&)
			 */
			NumericalTextFrame(const std::string& frameName,
			                   const unsigned short version,
			                   const std::string& value="");
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param textContent The numerical text content of the frame.
			 * @param description The frame description (optional).
			 */
			NumericalTextFrame(const std::string& frameName,
			                   const unsigned short version,
			                   const long intContent);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" NumericalTextFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			NumericalTextFrame() noexcept;
			
			/**
			 * The read() method for NumericalTextFrame first gets the text
			 * encoding of the frame at the 11th byte, and saves every following
			 * byte in the ByteArray as its content string. If the text is not
			 * encoded in UTF-8, it is converted to UTF-8 before saving it. If the
			 * text content is not an integer value, then the text content will be
			 * set to an empty string.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////// D E S C R I P T I V E T E X T F R A M E //////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * DescriptiveTextFrame is a child class of TextFrame. It is to be used for
	 * frames that have a set description. However, descriptions are optional in
	 * DescriptiveTextFrames. Therefore, the empty() method is not overriden as
	 * it doesn't matter if the description is empty or not.
	 * 
	 * Implemented in ID3TextFrame.cpp
	 * 
	 * @see ID3::Frame
	 * @see ID3::TextFrame
	 */
	class DescriptiveTextFrame : virtual public TextFrame {
		friend class FrameFactory;
		
		public:
			/**
			 * An option value used when reading USLT and COMM frames, as they have
			 * a 3-byte language string after the encoding byte.
			 */
			static const short OPTION_LANGUAGE = 0b00000001;
			
			/**
			 * An option value used when reading the WXXX frame, as the URL string
			 * is always encoded in LATIN-1.
			 */
			static const short OPTION_LATIN1_TEXT = 0b00000010;
			
			/**
			 * Checks if the given Frame is a DescriptiveTextFrame with the same
			 * frame ID, "null" status, and if text content, description, and
			 * language match (when neither of the Frames are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_DESCRIPTIVE.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * The write() method for DescriptiveTextFrame re-creates the frame
			 * header with UTF-8 encoding and its stored text content, description,
			 * and language (if it has one). If the Frame has language enabled,
			 * and the Frame's language is empty upon the call to write(), the
			 * language will default to "xxx".
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0,
			                        bool minimize=false);
			
			/**
			 * Set the text content and optionally description.
			 * Call write() to finalize changes.
			 * 
			 * NOTE: The content and description will not be modified if the Frame 
			 *       is read-only.
			 * 
			 * @param newContent The new text content.
			 * @param newDescription The new description.
			 */
			virtual void content(const std::string& newContent,
			                     const std::string& newDescription="");
			
			/**
			 * Get the description.
			 * 
			 * @return The description of the frame in UTF-8 encoding.
			 */
			std::string description() const;
			
			/**
			 * Set the description. Call write() to finalize changes.
			 * 
			 * NOTE: The description will not be modified if the Frame is read-only.
			 * 
			 * @param newDescription The new description.
			 */
			virtual void description(const std::string& newDescription);
			
			/**
			 * Get the language. If the language option was not passed in the
			 * to the constructor, this will always return "".
			 * 
			 * @returns The language of the frame in UTF-8 encoding.
			 */
			std::string language() const;
			
			/**
			 * Set the language. Call write() to finalize changes.
			 * 
			 * NOTE: The language will only be set if the language option was
			 *       passed to the constructor, and the new language string is
			 *       either an empty string or 3 characters long.
			 * 
			 * NOTE: The language is a ISO 639-2 code.
			 * 
			 * NOTE: The language will not be modified if the Frame is read-only.
			 * 
			 * @param newLanguage The new language.
			 */
			virtual void language(const std::string& newLanguage);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long
			 * enough to be valid it then sets isNull to false and calls
			 * ID3::TextFrame::read(ByteArray&) to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid text frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.			 
			 * 
			 * @param options Optional options for decoding the ByteArray, where
			 *                the option values checked for are
			 *                ID3::DescriptiveTextFrame::OPTION_LANGUAGE and
			 *                ID3::DescriptiveTextFrame::OPTION_LATIN1_TEXT (optional).
			 * @see ID3::Frame::Frame(std::string&,
			 *                        unsigned short,
			 *                        ByteArray&,
			 *                        unsigned long)
			 */
			DescriptiveTextFrame(const std::string& frameName,
			                     const unsigned short version,
			                     ByteArray& frameBytes,
			                     const short options=0);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * NOTE: The language will only be set if the language option is
			 *       passed in the parameter values, and the new language string is
			 *       either an empty string or 3 characters long.
			 * 
			 * NOTE: The language is a ISO 639-2 code.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 * @param description The frame description (optional).
			 * @param language The frame language (optional).
			 * @param options Optional options for decoding the ByteArray, where
			 *                the option values checked for are
			 *                ID3::DescriptiveTextFrame::OPTION_LANGUAGE and
			 *                ID3::DescriptiveTextFrame::OPTION_LATIN1_TEXT (optional).
			 *                For multiple values, OR (option | option) them together.
			 */
			DescriptiveTextFrame(const std::string& frameName,
			                     const unsigned short version,
			                     const std::string& value="",
			                     const std::string& description="",
			                     const std::string& language="",
			                     const short options=0);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" TextFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			DescriptiveTextFrame() noexcept;
			
			/**
			 * The description of the frame.
			 * 
			 * @see ID3::DescriptiveTextFrame::description()
			 */
			std::string textDescription;
			
			/**
			 * The language of the frame, if applicable.
			 * It should be formatted as a 3-character long ISO 639-2 code.
			 * 
			 * @see ID3::DescriptiveTextFrame::language()
			 */
			std::string textLanguage;
			
			/**
			 * The Frame options. It is set in the constructor and it is constant.
			 * The possible options are ID3::DescriptiveTextFrame::OPTION_LANGUAGE
			 * and ID3::DescriptiveTextFrame::OPTION_LATIN1_TEXT.
			 */
			const short frameOptions;
			
			/**
			 * The read() method for TextFrame first gets the text encoding of the
			 * frame at the 11th byte, and then reads the description and text, as
			 * well as the language if the option is set. If text is not encoded in
			 * UTF-8, it is converted to UTF-8 before saving it. If there is no
			 * null characters after the encoding byte and (optional) language
			 * bytes, then it will be assumed that the frame doesn't contain a
			 * description.
			 * 
			 * NOTE: This will use the options that were given in the constructor.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////// U R L T E X T F R A M E //////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * URLTextFrame is a child class of TextFrame. It is to be used for frames
	 * frames with URL content, aside from the WXXX frame. The URLTextFrame text
	 * content is assumed to be encoded in LATIN-1 as per the ID3v2 spec.
	 * 
	 * Implemented in ID3TextFrame.cpp
	 * 
	 * @see ID3::Frame
	 * @see ID3::TextFrame
	 */
	class URLTextFrame : virtual public TextFrame {
		friend class FrameFactory;
		
		public:
			/**
			 * Checks if the given Frame is a URLTextFrame with the same frame ID,
			 * "null" status, and if text content match (when neither of the Frames
			 * are "null").
			 * 
			 * @see ID3::Frame::operator==(Frame*)
			 */
			virtual bool operator==(const Frame* const frame) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_URL.
			 * 
			 * @return The FrameClass that is associated with the Frame class.
			 * @see ID3::Frame::type()
			 */
			virtual FrameClass type() const noexcept;
			
			/**
			 * The write() method for URLTextFrame re-creates the frame
			 * header with LATIN-1 encoding and its stored text content.
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0,
			                        bool minimize=false);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long
			 * enough to be valid it then sets isNull to false and calls
			 * ID3::TextFrame::read(ByteArray&) to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid URL frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(std::string&,
			 *                        unsigned short,
			 *                        ByteArray&,
			 *                        unsigned long)
			 */
			URLTextFrame(const std::string& frameName,
			             const unsigned short version,
			             ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a text frame with custom text.
			 * A Frame created from this constructor will return false
			 * when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid URL frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 */
			URLTextFrame(const std::string& frameName,
			             const unsigned short version,
			             const std::string& value="");
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" URLTextFrame object.
			 * 
			 * @see ID3::Frame::Frame()
			 */
			URLTextFrame() noexcept;
			
			/**
			 * The read() method for URLTextFrame saves every byte following the
			 * frame header as its text content. The text is first converted from
			 * LATIN-1 to UTF-8 before storing the string.
			 * 
			 * @see ID3::Frame::read()
			 */
			virtual void read();
	};
}

#endif
