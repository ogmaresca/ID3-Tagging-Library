/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 **********************************************************************/

#ifndef ID3_FRAME_H
#define ID3_FRAME_H

#include <fstream>
#include <string>

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
	 * An ID3::Frame stores information from an ID3v2 frame.
	 * If given an fstream object, it will read the tag from a given position.
	 * The file will not be closed.
	 */
	class Frame {
		public:
			/**
			 * This constructor reads an ID3v2 tag from a given location
			 * on a file.
			 * 
			 * @param file A file object. It will not be closed after
			 *             the constructor has finished.
			 * @param readpos The position on the file to start reading from.
			 * @param version The ID3v2 major version.
			 * @param filesize The size of the file. If the tag is
			 *                 supposedly longer than the filesize, the
			 *                 frame object will be "null" instead.
			 */
			Frame(std::ifstream& file,
			      const unsigned long readpos,
			      const unsigned int version,
			      const unsigned long filesize);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text.
			 * If the frame is not a text frame ID (doesn't begin with a
			 * 'T'), then the text content will not be set and this will
			 * be a "null" frame object.
			 * A Frame created from this constructor will return false
			 * when calling createdFromFile().
			 * 
			 * @param frameName The frame ID.
			 * @param textContent The text of the frame.
			 */
			Frame(const std::string& frameName,
			      const std::string& textContent);
			      
			/**
			 * An empty constructor to initialize variables.
			 * Creating a Frame with this constructor will result in a
			 * "null" Frame object.
			 */
			Frame();
			
			/**
			 * Use this function to determine if the Frame was
			 * succesfully created or not.
			 * 
			 * @return true if the frame was read/created succesfully,
			 *         false if not.
			 */
			bool null() const;
			
			/**
			 * Get the starting byte that the Frame object read from the
			 * file, including the frame header.
			 * Will be 0 if the Frame was created by manually giving it
			 * content.
			 * 
			 * @return The first byte it read from.
			 */
			unsigned long start() const;
			
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
			 * Get the size of the frame, minus the frame header.
			 * The frame header size is stored at ID3::HEADER_BYTE_SIZE.
			 * 
			 * @return The size of the frame.
			 */
			unsigned long size() const;
			
			/**
			 * Get the ID of the frame (ex: TIT2 for titles).
			 * 
			 * @return The frame ID.
			 */
			std::string frame() const;
			
			/**
			 * Check if the Frame is a text frame.
			 * This is done by checking if the first letter of the ID is
			 * 'T'.
			 * 
			 * @return true if the Frame is a text frame, false if not.
			 */
			bool isTextFrame() const;
			
			/**
			 * Returns the text content of the frame.
			 * 
			 * @return The text content, or "" if the Frame is null or
			 *         not a text frame.
			 */
			std::string text() const;
			
			/**
			 * Set the text content of the frame. If not a text frame,
			 * this does nothing.
			 * 
			 * @param newValue The new text content.
			 */
			void text(std::string newValue);
			
			/**
			 * Revert any changes made to the frame since it was last
			 * read, created, or written.
			 */
			void revert();
			
			/**
			 * Finalize any changes after a write.
			 */
			void save();
			
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
		
		protected:
			/**
			 * A helper constructor method to read the contents of a
			 * text frame from file.
			 * 
			 * @param file The file object. It will not be closed.
			 */
			void getTextFrame(std::ifstream& file);
			
			/**
			 * A variable that records if the Frame is null.
			 * 
			 * @see ID3::Frame::null()
			 */
			bool isNull;
			
			/**
			 * The starting byte position that it read from the file.
			 * 
			 * @see ID3::Frame::start()
			 */
			unsigned long startpos;
			
			/**
			 * The ending byte position that it read from the file.
			 * 
			 * @see ID3::Frame::end()
			 */
			unsigned long endpos;
			
			/**
			 * The size of the frame, not including the header.
			 * 
			 * @see ID3::Frame::size()
			 */
			unsigned long frameSize;
			
			/**
			 * The frame ID.
			 * 
			 * @see ID3::Frame::frame()
			 */
			std::string id;
			
			/**
			 * The content of the frame, if it's a text frame.
			 * 
			 * @see ID3::Frame::text()
			 */
			std::string textContent;
			
			/**
			 * The content of the frame on file, if it's a text frame.
			 * This will differ from ID3::Frame::textContent upon calls
			 * to ID3::Frame::text(std::string), and will only be the
			 * same again after a call to ID3::Frame::revert() or
			 * ID3::Frame::save().
			 * 
			 * NOTE: This is currently a placeholder. This will be changed
			 * once I add write support.
			 */
			std::string textContentOnFile;
	};
}

#endif
