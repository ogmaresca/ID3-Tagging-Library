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

#ifndef ID3_TEXT_FRAME_H
#define ID3_TEXT_FRAME_H

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
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////// T E X T F R A M E /////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
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
			 * Append a string to the frame content. This string will be appended
			 * to the text content with a separating character, and it will appear
			 * in its own string in the vector when calling contents().
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
			 * Check if the Frame's content is empty. A TextFrame is empty if
			 * the frame content is an empty string.
			 * 
			 * @return true if the content is empty, false otherwise.
			 * @see ID3::Frame::empty()
			 */
			virtual bool empty() const;
			
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
			 * Get the text content, split by the separating character.
			 * 
			 * @return The text content of the frame in UTF-8 encoding split into
			 *         a string vector.
			 */
			std::vector<std::string> contents() const;
			
			/**
			 * Set the text content with a string vector. The vector will be
			 * contatenated by the frame's separating character.
			 * Call write() to finalize changes.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void contents(const std::vector<std::string>& newContent);
			
			/**
			 * Get the character that is used to separate multiple string values
			 * in the frame.
			 * 
			 * @return '/' for certain ID3v2.2 and ID3v2.3 frames, '\0' for the rest.
			 */
			char stringSeparator() const;
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for TextFrame re-creates the frame header and the
			 * frame text, with LATIN-1 as the encoding if the text is in ASCII and
			 * UTF-8 if characters fall outside of the ASCII range.
			 * 
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long enough to be valid it
			 * then calls ID3::TextFrame::read() to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid text frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(FrameID&, ushort, ByteArray&)
			 */
			TextFrame(const FrameID&     frameName,
			          const ushort       version,
			          const ByteArray&   frameBytes);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * @param frameName The frame ID.
			 * @param value The text of the frame (optional).
			 */
			TextFrame(const FrameID&     frameName=Frames::FRAME_UNKNOWN_FRAME,
			          const std::string& value="") noexcept;
			
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
			
			/**
			 * When write()-ing the Frame, before updating the interal ID3v2
			 * version call this function to convert any separating character to a
			 * null character, if necessary.
			 */
			void convertSeparator();
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
			virtual bool operator==(long long val) const noexcept;
			
			/**
			 * Get the text content of the frame casted to a long int.
			 */
			virtual operator long long() const noexcept;
			
			/**
			 * Append an int to the frame content. This int will be appended as a
			 * string to the text content with a separating character, and it will
			 * appear in its own string in the vector when calling contents().
			 * 
			 * @param val The value to add.
			 * @return The numerical text frame.
			 */
			virtual NumericalTextFrame& operator+=(long long val) noexcept;
			
			/**
			 * Append a string to the frame content. This string will be appended
			 * to the text content with a separating character, and it will appear
			 * in its own string in the vector when calling contents().
			 * 
			 * NOTE: If the string is not an int value, nothing will be added and,
			 * if the NumericalTextFrame object hasn't been edited before, edited()
			 * will continue to return false.
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
			virtual void content(long long newContent);
			
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
			
			/**
			 * Set the text content with a string vector. The vector will be
			 * contatenated by the frame's separating character.
			 * Call write() to finalize changes.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * NOTE: Any strings stored in the vector that are not an integer value
			 *       will not be set.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void contents(const std::vector<std::string>& newContent);
			
			/**
			 * Set the text content with a integer vector. The vector will be
			 * contatenated by the frame's separating character.
			 * Call write() to finalize changes.
			 * 
			 * NOTE: The content will not be modified if the Frame is read-only.
			 * 
			 * @param newContent The new text content.
			 */
			virtual void contents(const std::vector<long long>& newContent);
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * TextFrame class. If the given ByteArray is long enough to be valid it
			 * then calls ID3::TextFrame::read() to process the ByteArray.
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
			 * @see ID3::Frame::Frame(FrameID&, ushort, ByteArray&)
			 */
			NumericalTextFrame(const FrameID&   frameName,
			                   const ushort     version,
			                   const ByteArray& frameBytes);
			
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
			 * @param value The text of the frame (optional).
			 * @see ID3::Frame::Frame(FrameID&, ushort, ByteArray&)
			 */
			NumericalTextFrame(const FrameID&     frameName=Frames::FRAME_UNKNOWN_FRAME,
			                   const std::string& value="");
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid text frame.
			 * 
			 * @param frameName The frame ID.
			 * @param textContent The numerical text content of the frame.
			 * @param description The frame description (optional).
			 */
			NumericalTextFrame(const FrameID& frameName,
			                   const long     intContent);
			
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
	 * @see ID3::Frame
	 * @see ID3::TextFrame
	 */
	class DescriptiveTextFrame : virtual public TextFrame {
		friend class FrameFactory;
		
		public:
			/**
			 * An option value used when reading USLT, USER, and COMM frames, as
			 * they have a 3-byte language string after the encoding byte.
			 */
			static const ushort OPTION_LANGUAGE = 0b00000001;
			
			/**
			 * An option value used when reading the WXXX frame, as the URL string
			 * is always encoded in LATIN-1.
			 */
			static const ushort OPTION_LATIN1_TEXT = 0b00000010;
			
			/**
			 * An option value used when reading the USER frame, as it doesn't have
			 * a description.
			 */
			static const ushort OPTION_NO_DESCRIPTION = 0b00000100;
			
			/**
			 * How long, in bytes, a valid ISO 639-2 code language is.
			 */
			static const ushort LANGUAGE_SIZE = 3;
			
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
			
			/**
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for DescriptiveTextFrame re-creates the frame
			 * header with UTF-8 encoding and its stored text content, description,
			 * and language (if it has one). If the Frame has language enabled,
			 * and the Frame's language is empty upon the call to write(), the
			 * language will default to "xxx".
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long enough to be valid it
			 * then calls ID3::TextFrame::read() to process the ByteArray.
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
			 * @see ID3::Frame::Frame(FrameID&, ushort, ByteArray&)
			 */
			DescriptiveTextFrame(const FrameID&   frameName,
			                     const ushort     version,
			                     const ByteArray& frameBytes,
			                     const ushort     options=0);
			
			/**
			 * This constructor manually creates a text frame with
			 * custom text. A Frame created from this constructor will
			 * return false when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid descriptive text frame.
			 * 
			 * NOTE: The language will only be set if the language option is
			 *       passed in the parameter values, and the new language string is
			 *       either an empty string or 3 characters long.
			 * 
			 * NOTE: The language is a ISO 639-2 code.
			 * 
			 * @param frameName   The frame ID.
			 * @param value       The text of the frame (optional).
			 * @param description The frame description (optional).
			 * @param language    The frame language (optional).
			 * @param options Optional options for decoding the ByteArray, where
			 *                the option values checked for are
			 *                ID3::DescriptiveTextFrame::OPTION_LANGUAGE and
			 *                ID3::DescriptiveTextFrame::OPTION_LATIN1_TEXT (optional).
			 *                For multiple values, OR (option | option) them together.
			 */
			DescriptiveTextFrame(const FrameID&    frameName=Frames::FRAME_UNKNOWN_FRAME,
			                     const std::string& value="",
			                     const std::string& description="",
			                     const std::string& language="",
			                     const ushort       options=0);
			
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
			 * This value saves whether the language option was passed to the
			 * constructor or not.
			 * 
			 * @see ID3::DescriptiveTextFrame::OPTION_LANGUAGE
			 */
			const bool optionLanguage;
			
			/**
			 * This value saves whether the LATIN-1 text option was passed to the
			 * constructor or not.
			 * 
			 * @see ID3::DescriptiveTextFrame::OPTION_LATIN1_TEXT
			 */
			const bool optionLatin1;
			
			/**
			 * This value saves whether the "no description" option was passed to
			 * the constructor or not. This is only checked in the write() method,
			 * the read() method will try to account for frames that shouldn't be
			 * missing a description that do miss a description.
			 * 
			 * @see ID3::DescriptiveTextFrame::OPTION_NO_DESCRIPTION
			 */
			const bool optionNoDescription;
			
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
			 * Print information about the frame.
			 * 
			 * @see ID3::Frame::print()
			 */
			virtual void print() const;
			
			/**
			 * The write() method for URLTextFrame re-creates the frame
			 * header with LATIN-1 encoding and its stored text content.
			 * 
			 * @todo Actually implement the function.
			 * @see ID3::Frame::write()
			 */
			virtual ByteArray write();
		
		protected:
			/**
			 * This constructor calls the identical constructor in the
			 * Frame class. If the given ByteArray is long enough to be valid it
			 * then calls ID3::TextFrame::read() to process the ByteArray.
			 * 
			 * NOTE: frameName is not checked to verify that the frame ID is a
			 *       valid URL frame ID.
			 * 
			 * NOTE: The ID3v2 version is not checked to verify that it
			 *       is a supported ID3v2 version.
			 * 
			 * @see ID3::Frame::Frame(FrameID&, ushort, ByteArray&)
			 */
			URLTextFrame(const FrameID&   frameName,
			             const ushort     version,
			             const ByteArray& frameBytes);
			
			/**
			 * This constructor manually creates a text frame with custom text.
			 * A Frame created from this constructor will return false
			 * when calling createdFromFile().
			 * 
			 * NOTE: The frame ID is not checked to verify that the
			 *       frame ID is a valid URL text frame ID.
			 * 
			 * @param frameName The frame ID.
			 * @param value The text of the frame (optional).
			 */
			URLTextFrame(const FrameID&     frameName=Frames::FRAME_UNKNOWN_FRAME,
			             const std::string& value="");
			
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
