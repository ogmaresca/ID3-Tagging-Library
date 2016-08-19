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
			 * @abstract
			 */
			virtual bool operator==(const FrameClass classID) const noexcept = 0;
			
			/**
			 * Return a FrameClass enum value that is associated with its class.
			 * 
			 * @abstract
			 */
			virtual operator FrameClass() const noexcept = 0;
			
			/**
			 * Returns true if the frame was read/created succesfully, false if not.
			 * 
			 * @see ID3::Frame::null()
			 */
			 bool operator==(bool boolean) const noexcept;
			
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
			 * Get the position of the byte at the end of the file (the byte after
			 * the last byte that makes up the frame). Will be 0 if the Frame was
			 * created by manually giving it content.
			 * 
			 * @return The last byte it read from.
			 */
			unsigned long end() const;
			
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
			 * If calling this constructor, you should check that the ByteArray is
			 * a valid size, then read() the frame bytes and set isNull to false.
			 * 
			 * NOTE: frameBytes MUST include the frame header.
			 * 
			 * NOTE: The version is not checked to see if it is a
			 *       supported ID3v2 major version.
			 * 
			 * @param frameName The frame ID string.
			 * @param version The ID3v2 major version.
			 * @param frameBytes The content of the frame in bytes.
			 * @param end The byte position in the music file where the frame ends.
			 */
			Frame(const std::string& frameName,
			      const unsigned short version,
			      ByteArray& frameBytes,
			      const unsigned long end);
			
			/**
			 * An empty constructor to initialize variables. Creating a Frame with
			 * this constructor will result in a "null" Frame object.
			 */
			Frame() noexcept;
			
			/**
			 * Read and process the bytes of an ID3v2 frame.
			 * The frame header must be included in the ByteArray.
			 * The ID3v2 major version used will be the value saved
			 * in ID3::Frame::ID3ver.
			 * This method is to be implemented in child classes.
			 * 
			 * @param frameBytes The bytes of the frame.
			 * @abstract
			 */
			virtual void read(ByteArray& frameBytes) = 0;
			
			/**
			 * This variable records if the Frame is null.
			 * 
			 * @see ID3::Frame::null()
			 */
			bool isNull;
			
			/**
			 * The ending byte position that the frame occupies on the file, + 1.
			 * 
			 * @see ID3::Frame::end()
			 */
			unsigned long endPosition;
			
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
			 * Checks if the FrameClass value is FrameClass::CLASS_UNKNOWN.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_UNKNOWN.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept;
			
			/**
			 * Check if the Frame's content is empty. An UnknownFrame is empty if
			 * the size of the frame in bytes is <= HEADER_BYTE_SIZE.
			 * 
			 * @return true if the content is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
			/**
			 * The write() method for UnknownFrame simply returns
			 * a copy of frameContent.
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
			             ByteArray& frameBytes,
			             const unsigned long end);
			
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
			 * The read() method for UnknownFrame just saves the given
			 * ByteArray to the frameContents variable.
			 * 
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes);
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
			 * Checks if the FrameClass value is FrameClass::CLASS_TEXT.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_TEXT.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept;
			
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
			                                    ByteArray bytes,
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
			          ByteArray& frameBytes,
			          const unsigned long end);
			
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
			 * @param frameBytes The byte vector to read.
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes);
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
			 * Checks if the FrameClass value is FrameClass::CLASS_NUMERICAL.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_NUMERICAL.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept;
			
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
			 * Set the numerical content. Call write() to finalize changes.
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
			                   ByteArray& frameBytes,
			                   const unsigned long end);
			
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
			 * @param frameBytes The byte vector to read.
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes);
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
			 * Checks if the FrameClass value is FrameClass::CLASS_DESCRIPTIVE.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_DESCRIPTIVE.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept;
			
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
			                     const unsigned long end,
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
			 * @param frameBytes The byte vector to read.
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes);
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
			 * Checks if the FrameClass value is FrameClass::CLASS_URL.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::CLASS_URL.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept;
			
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
			             ByteArray& frameBytes,
			             const unsigned long end);
			
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
			 * @param frameBytes The byte vector to read.
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes);
	};
}

#endif
