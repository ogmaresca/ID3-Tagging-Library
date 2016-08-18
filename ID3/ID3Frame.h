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
		NUMERICAL = 2, //NumericalTextFrame
		TEXT      = 1, //TextFrame
		UNKNOWN   = 0  //UnknownFrame
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
	///////////////////////// M U L T I P L E F R A M E /////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * MultipleFrame is an interface/abstract class that will eventually
	 * be used to add support for reading and writing multiple frames
	 * with the same ID.
	 * 
	 * @todo Implement the class
	 * @see ID3::Frame
	 */
	class MultipleFrame : virtual public Frame {
		public:
			/**
			 * The destructor.
			 */
			virtual ~MultipleFrame() = 0;
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////// M U L T I P L E F R A M E /////////////////////////
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
			 * Checks if the FrameClass value is FrameClass::UNKNOWN.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::UNKNOWN.
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
			 * Checks if the FrameClass value is FrameClass::TEXT.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept;
			
			/**
			 * Returns FrameClass::TEXT.
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
			virtual std::string content() const final;
			
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
			 * NOTE: If the given frame does not support a description,
			 *       the description will not be set.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 * @param description The frame description (optional).
			 */
			TextFrame(const std::string& frameName,
			          const unsigned short version,
			          const std::string& value="",
			          const std::string& description="");
			
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
			 * @see ID3::Frame::text()
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
			virtual bool operator==(const Frame* const frame) const noexcept override;
			
			/**
			 * Checks if the FrameClass value is FrameClass::NUMERICAL.
			 * 
			 * @see ID3::Frame::operator==(FrameClass)
			 */
			virtual bool operator==(const FrameClass classID) const noexcept override;
			
			/**
			 * Returns FrameClass::NUMERICAL.
			 * 
			 * @see ID3::Frame::operator FrameClass()
			 */
			virtual operator FrameClass() const noexcept override;
			
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
			virtual NumericalTextFrame& operator+=(const std::string& str) noexcept override;
			
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
			virtual void content(const std::string& newContent) override;
		
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
			 * NOTE: If the given frame does not support a description,
			 *       the description will not be set.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param value The text of the frame (optional).
			 * @param description The frame description (optional).
			 * @see ID3::TextFrame::TextFrame(std::string&,
			 *                                unsigned short,
			 *                                std::string&,
			 *                                std::string&)
			 */
			NumericalTextFrame(const std::string& frameName,
			                   const unsigned short version,
			                   const std::string& value="",
			                   const std::string& description="");
			
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
			 * NOTE: If the given frame does not support a description,
			 *       the description will not be set.
			 * 
			 * @param frameName The frame ID.
			 * @param version The ID3v2 major version.
			 * @param textContent The numerical text content of the frame.
			 * @param description The frame description (optional).
			 */
			NumericalTextFrame(const std::string& frameName,
			                   const unsigned short version,
			                   const long intContent,
			                   const std::string& description="");
			
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
}

#endif
