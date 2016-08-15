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
	 * An ID3::Frame stores information from an ID3v2 frame.
	 * It is an abstract class, use one of its children defined below
	 * to create a Frame.
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
			 * Use this method to determine if the Frame was
			 * succesfully created or not.
			 * 
			 * @return true if the frame was read/created succesfully,
			 *         false if not.
			 */
			bool null() const;
			
			/**
			 * Get the position of the byte at the end of the file
			 * (the byte after the last byte that makes up the frame).
			 * Will be 0 if the Frame was created by manually giving it
			 * content.
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
			ByteArray bytes() const;
			
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
			 * @return true if the constructor with an file object was
			 *         called, false if not.
			 */
			bool createdFromFile() const;
			
			/**
			 * Save any changes made to the frame, and get the updated
			 * content of the frame in bytes.
			 * This method is to be implemented in child classes.
			 * 
			 * @param version The ID3v2 major version to save the frame
			 *                contents as. If a value is not supplied,
			 *                or it is not supported, it will default
			 *                to using the ID3v2 major version supplied
			 *                during the object's creation.
			 * @return The new content of the frame, in bytes.
			 * @abstract
			 */
			virtual ByteArray write(unsigned short version=0) = 0;
		
		protected:
			/**
			 * This constructor initializes the relevant variables with
			 * the passed-in variables. Calling this constructor will
			 * set ID3::Frame::isFromFile to true.
			 * If calling this constructor, you should check that the
			 * frame is a valid size, then read() the frame bytes and
			 * set isNull to false.
			 * 
			 * NOTE: frameBytes MUST include the frame header.
			 * 
			 * NOTE: The version is not checked to see if it is a
			 *       supported ID3v2 major version.
			 * 
			 * @param frameName The frame ID string.
			 * @param version The ID3v2 major version.
			 * @param frameBytes The content of the frame in bytes.
			 * @param end The byte position in the music file where the
			 *            frame end.
			 */
			Frame(const std::string& frameName,
			      const unsigned short version,
			      ByteArray& frameBytes,
			      const unsigned long end);
			
			/**
			 * An empty constructor to initialize variables.
			 * Creating a Frame with this constructor will result in a
			 * "null" Frame object.
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
			 * The ending byte position that the frame occupies on the
			 * file, + 1.
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
			 * The write() method for UnknownFrame simply returns
			 * a copy of frameContent.
			 * 
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0) override;
		
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
			virtual void read(ByteArray& frameBytes) override;
	};
	
	/**
	 * TextFrame is a child class of Frame. It is to be used for frames
	 * with text content.
	 * 
	 * @see ID3::Frame
	 */
	class TextFrame : virtual public Frame {
		friend class FrameFactory;
		
		public:
			/**
			 * The write() method for TextFrame re-creates the frame
			 * header with UTF-8 encoding and its stored text content.
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write(unsigned short)
			 */
			virtual ByteArray write(unsigned short version=0);
			
			/**
			 * Get the text content.
			 * 
			 * @returns The text content of the frame in UTF-8 encoding.
			 */
			virtual std::string content() const;
			
			/**
			 * Set the text content. Call write() to finalize changes.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void content(std::string newContent);
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long
			 * enough to be valid it then sets isNull to false and calls
			 * ID3::TextFrame::read(ByteArray&) to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame
			 * ID is a valid text frame ID.
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
			 * @param textContent The text of the frame (optional).
			 * @param description The frame description (optional).
			 */
			TextFrame(const std::string& frameName,
			          const unsigned short version,
			          const std::string& textContent="",
			          const std::string& description="");
			
			/**
			 * An empty constructor to initialize variables.
			 * Creating a Frame with this constructor will result in a
			 * "null" TextFrame object.
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
			 * The read() method for TextFrame first gets the text
			 * encoding of the frame at the 11th byte, and saves every
			 * following byte in the ByteArray as its content string.
			 * If the text is not encoded in UTF-8, it is converted
			 * to UTF-8 before saving it.
			 * 
			 * @see ID3::Frame::read(ByteArray&)
			 */
			virtual void read(ByteArray& frameBytes) override;
	};
}

#endif
