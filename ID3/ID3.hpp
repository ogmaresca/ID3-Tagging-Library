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

#ifndef ID3_HPP
#define ID3_HPP

#include <fstream>       //For std::ifstream and std::fstream
#include <vector>        //For std::vector
#include <unordered_map> //For std::unordered_map and std::pair
#include <memory>        //For std::shared_ptr
#include <functional>    //For std::function

#include "Frames/ID3Frame.hpp"            //For supporting Frames
#include "Frames/ID3PictureFrame.hpp"     //For PictureType
#include "Frames/ID3EventTimingFrame.hpp" //For TimingCodes
#include "ID3FrameID.hpp"                 //For frame IDs
#include "ID3FrameFactory.hpp"            //For FrameFactory

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * Items relating to ID3v1 are kept in the namespace ID3::V1. V2 related
 * items are kept within the ID3 namespace.
 * 
 * All strings are stored in UTF-8 text encoding.
 * 
 * ID3 class knowledge:
 *     The ID3::Tag class knows the C++ type equivalent of what each frame
 *     stores, and any special formatting the frame has, as well as the ID3
 *     header, footer, and ID3v1.
 *     The ID3::FrameFactory class knows the frame header and how to create a
 *     Frame object.
 *     The ID3::Frame class' children know how to parse and store the bytes of
 *     their frame(s), but not if the data is invalid or not.
 * 
 * ID3-Tagging-Library places a focus on compatibility when reading. As long as
 * there aren't any fundamental differences between ID3v2 versions (such as
 * whether the frame size is synchsafe or not), it will read the information
 * regardless of version. For example, ID3v2.4 tags will be read on ID3v2.3
 * files and vice versa, and ID3v2.4 text encodings are supported in ID3v2.3 frames.
 * 
 * ID3v2.3.0 standard: @link http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: @link http://id3.org/id3v2.4.0-structure
 */
namespace ID3 {
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////// T Y P E D E F S //////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	typedef std::vector<uint8_t> ByteArray;
	typedef std::shared_ptr<Frame> FramePtr;
	typedef std::unordered_multimap<FrameID, FramePtr> FrameMap;
	typedef std::pair<FrameID, FramePtr> FramePair;
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// S T R U C T S ///////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 structs.
	 */
	namespace V1 {
		/**
		 * A 128-bit struct that captures the structure of an ID3v1 tag.
		 */
		struct Tag {
			char header[3];
			char title[30];
			char artist[30];
			char album[30];
			char year[4];
			char comment[30];
			uint8_t genre;
		};
		
		/**
		 * A 128-bit struct that captures the structure of an ID3v1.1 tag.
		 */
		struct P1Tag {
			char header[3];
			char title[30];
			char artist[30];
			char album[30];
			char year[4];
			char comment[28];
			char zero;
			uint8_t trackNum;
			uint8_t genre;
		};
		
		/**
		 * A 227-bit struct that captures the structure of an extended ID3v1 tag.
		 * ID3v1 Extended tags are placed before the standard ID3v1 tag.
		 */
		struct ExtendedTag {
			char header[4];
			char title[60];
			char artist[60];
			char album[60];
			uint8_t speed;
			char genre[30];
			char startTime[6];
			char endTime[6];
		};
	}
	
	/**
	 * A 10-bit struct that captures the structure of the ID3v2 header.
	 */
	struct Header {
		char header[3];
		uint8_t majorVer;
		uint8_t minorVer;
		uint8_t flags;
		uint8_t size[4]; //A synchsafe integer in ID3v2.4+
	};
	
	/**
	 * A struct that contains information about a picture embedded in ID3v2 tags.
	 * The picture data is not checked to verify that it is a valid image.
	 */
	struct Picture {
		/**
		 * Create a Picture struct. Every parameter is optional, but unless given
		 * a valid MIME type the struct will be null.
		 * 
		 * Defined in ID3PictureFrame.cpp.
		 * 
		 * @param pictureByteArray The uint8_t vector of the PNG or JPG image.
		 * @param mimeType The MIME type.
		 * @param pictureDescription The description.
		 * @param pictureType The picture type defined in the ID3v2 specification
		 *                    for the APIC field. Defaults to FRONT_COVER.
		 */
		Picture(const ByteArray&   pictureByteArray=ByteArray(),
			     const std::string& mimeType="",
			     const std::string& pictureDescription="",
			     const PictureType  pictureType=PictureType::FRONT_COVER);
		/** @return Whether the MIME type is valid or not. */
		inline bool null() const { return !PictureFrame::allowedMIMEType(MIME); }
		/**
		 * @return The size that this struct will take when written to an Attached
		 *         Picture frame, excluding the header.
		 */
		inline ulong size() const { return MIME.size() + 3 + description.size() + data.size(); }
		std::string MIME;
		PictureType type;
		std::string description;
		ByteArray   data;
	};
	
	/**
	 * A struct that contains information about an event timing code.
	 * If the value of the timing code is not set in the tags, the value should
	 * be 0.
	 * If usingMilliseconds is false, then the value refers to the MPEG frames of
	 * the file, instead of milliseconds.
	 */
	struct EventTimingCode {
		/**
		 * Create a EventTimingCode struct.
		 * 
		 * Defined in ID3EventTimingFrame.cpp.
		 * 
		 * @param code The event timing code.
		 * @param val  The event timing code value (optional, defaults to 0).
		 * @param milliseconds Whether the value refers to milliseconds or MPEG
		 *                     frames (optional, defaults to true).
		 */
		EventTimingCode(TimingCodes code, ulong val=0UL, bool milliseconds=true);
		bool usingMilliseconds;
		TimingCodes timingCode;
		ulong value;
	};
	
	/**
	 * A struct that holds the text content of a frame in a tag, and the
	 * description and language field as well if the frame supports it.
	 */
	struct Text {
		/**
		 * Create a Text struct. Every paramter is optional, and the respective
		 * field will default to an empty string.
		 * 
		 * Defined in ID3TextFrame.cpp.
		 * 
		 * @param textContent The content string of the frame.
		 * @param descText    The description of the frame.
		 * @param langText    The language of the frame. It should be a ISO 639-2
		 *                    language code.
		 */
		Text(const std::string& textContent="",
		     const std::string& descText="",
		     const std::string& langText="");
		std::string text;
		std::string description;
		std::string language;
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// C L A S S E S ///////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * A class that, given a file or filename, will read its ID3 tags.
	 * Call Tag::null() after instantiation to check if the file was
	 * properly read. Files must be .mp3 files.
	 * 
	 * Defined in ID3Tag.cpp.
	 */	
	class Tag {
		public:
			/**
			 * Constructor that takes a filename and opens the file.
			 * 
			 * NOTE: If the file does not exist a ID3::ID3FileNotFoundException
			 *       will be thrown.
			 * NOTE: If the given file contains ID3v2 tags whose size is stated to
			 *       be larger than the file itself, an ID3::FormatException will
			 *       be thrown.
			 * NOTE: If the file is not an MP3 or MP4 file an
			 *       ID3::NotMP3FileException will be thrown.
			 * 
			 * @param fileLoc The file path.
			 * @throws ID3::ID3FileNotFoundException if the file does not exist.
			 * @throws ID3::FileFormatException if the ID3v2 tags on file are
			 *         supposedly bigger than the file itself.
			 * @throws ID3::NotMP3FileException if the file is not an MP3 or MP4 file.
			 */
			Tag(const std::string& fileLoc);
			
			/**
			 * A constructor that creates a blank Tag object without a file.
			 */
			Tag() noexcept;
			
			/**
			 * Returns true if the Frame map is not empty, false otherwise.
			 */
			operator bool() const noexcept;
			
			/**
			 * Returns true if the Frame map empty, false otherwise.
			 */
			bool operator!() const noexcept;
			
			/**
			 * Write the tags to the file location given.
			 * 
			 * NOTE: If a file does not exist at the location a
			 *       ID3::FileNotFoundException will be thrown.
			 * NOTE: If the given file contains ID3v2 tags whose size is stated to
			 *       be larger than the file itself, an ID3::FormatException will
			 *       be thrown.
			 * NOTE: If the file is not an MP3 or MP4 file a
			 *       ID3::NotMP3FileException will be thrown.
			 * NOTE: If another Tag object or another program has edited the ID3
			 *       tags on the file between the creation of this object and the
			 *       call to this method, the new tags will be overriden with the
			 *       tags in this object.
			 * NOTE: Any ID3v1, ID3v1.1, and ID3v1 Extended tags will be removed.
			 * NOTE: Any ID3v1, v1.1, and v1 Extended tags will be removed, the
			 *       ID3v2 tag will be written to ID3v2.4.0, and it will not include
			 *       unsynchronisation, encryption, compression, extended header,
			 *       or footer. If the new tag size is smaller than the old tag
			 *       size, then padding will be added to make it fit. If it is
			 *       bigger, or a v1 tag is on file, then the entire file will be
			 *       rewritten to contain the tags.
			 * 
			 * @param fileLoc        The file to write to.
			 * @param paddingFactor  The padding to add to the tag, if the file
			 *                       needs to be rewritten. Value is between 0.0
			 *                       and 1.0. The amount of padding will be (tag
			 *                       size) * paddingFactor, rounded up to the next
			 *                       highest multiple of 4096 unless it is 0. This
			 *                       can be safely set to 0, but if even one byte
			 *                       of data will need to be written to the file in
			 *                       the future, then the file will need to be
			 *                       rewritten again, which is a costly operation.
			 *                       Defaults to 10% (0.1).
			 * @param setFileNameUponSuccess  If true, the internal filename will not
			 *                                be set to fileLoc until the method has
			 *                                succesfully completed.
			 * @param discardNonCoverPictures An option to reduce tag size.
			 *                                If true, all but the first front cover
			 *                                pictures in the tag will not be saved.
			 * @param discardUnknown An option to reduce tag size. If true, all
			 *                       unknown frames will be discarded.
			 * @throws ID3::FileNotFoundException if the file location saved in
			 *         this object does not exist.
			 * @throws ID3::NotMP3FileException if the file is not an MP3 file.
			 * @throws ID3::FileFormatException if the existing ID3 tags on file
			 *         are supposedly bigger than the file itself, or the ID3v1 and
			 *         ID3v2 tags overlap on file.
			 * @throws ID3::FrameSizeException if a frame in the tag is bigger than
			 *         the maximum frame size (28 bits, 256 MiB).
			 * @throws ID3::TagSizeException if the tag to write is bigger than the
			 *         maximum tag size (28 bits, 256 MiB).
			 */
			void write(const std::string& fileLoc,
			           const float        paddingFactor=0.1,
			           const bool         setFileNameUponSuccess=true,
			           const bool         discardNonCoverPictures=false,
			           const bool         discardUnknown=false);
			
			/**
			 * Write the tags to the file. This method will write to the last valid
			 * file location given in the write method, or if was never called the
			 * location that was that was given in the constructor
			 * ID3::Tag::Tag(std::string&). If that constructor was not called when
			 * creating this object, or the file at that location has been renamed,
			 * an ID3::FileNotFoundException will be thrown.
			 * 
			 * @see ID3::Tag::write(std::string&)
			 */
			inline void write() { write(fileName()); }
			
			/**
			 * Revert any changes made to the tags since the last call to a
			 * write() method, or since the creation of the Tag object if a write()
			 * method has never been called.
			 */
			void revert();
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			//////////////// S T A R T   F R A M E   G E T T E R S ////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Check if a frame exists.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return If the Frame exists or not.
			 */
			bool exists(const FrameID& frameName) const;
			
			/**
			 * Get the text content of a frame.
			 * 
			 * NOTE: Not all frames support text content. If the given frame name
			 *       does not, or the request frame does not exist in the tag, an
			 *       empty string will be returned. See ID3::Text::exists(Frames)
			 *       to check if the frame actually exists.
			 * 
			 * NOTE: No formatting will be done, so if the frame has special
			 *       formatting then you should call the relevant method instead.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return The text content, or "" if the frame is not found, "null",
			 *         or not a text frame.
			 */
			std::string textString(const FrameID& frameName) const;
			
			/**
			 * Get the text content of a frame, split up into a vector for each
			 * individual string in the frame.
			 * 
			 * If the frame ID supports multiple instances of the frame, the vector
			 * will instead hold a string of each text value of every instance of
			 * each frame unsplit.
			 * 
			 * NOTE: Not all frames support text content. If the given frame name
			 *       does not, or the music file does not contain an ID3v2 frame of
			 *       that type, the vector will contain just an empty string. Also,
			 *       multiple values do not make sense for all text frames. For
			 *       those frames, use ID3::Tag::textString(Frames) instead.
			 * 
			 * NOTE: No formatting will be done, so if the frame has special
			 *       formatting then you should call the relevant method instead.
			 * 
			 * NOTE: If the file uses ID3v2.3, then the multiple value separator
			 *       is a slash instead of a null character, and is only supported
			 *       in the Composer, Lyricist, Original Lyricist, Original Artist,
			 *       and Artist frames. If requesting another frame from an ID3v2.3
			 *       file, then the vector will not separate the text content by
			 *       the slash separator.
			 * 
			 * NOTE: The returned vector will always hold at least one string that
			 *       is either an empty string or the frame text content. See
			 *       ID3::Text::exists(Frames) to check if the requested frame
			 *       content actually exists in the tag.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return A string vector of the text content.
			 */
			std::vector<std::string> textStrings(const FrameID& frameName) const;
			
			/**
			 * Return a Text struct with a frame's text content, description, and
			 * language.
			 * 
			 * NOTE: Not all frames support text content. If the given frame name
			 *       does not, or the music file does not contain an ID3v2 frame of
			 *       that type, the returned struct will contain just empty strings.
			 *       Likewise, if the frame is a text frame but does not support
			 *       descriptions and/or languages, those fields will be empty
			 *       strings. See ID3::Text::exists(Frames) to check if the frame
			 *       actually exists in the tag.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return A Text struct of the frame content.
			 */
			Text text(const FrameID& frameName) const;
			
			/**
			 * Return a Text struct that matches the given function.
			 * 
			 * NOTE: If multiple instances of the given frame ID are not allowed on
			 *       the file, then this function call will be identical to
			 *       ID3::Tag::text(FrameID&). Additionally, if the frame does
			 *       allow multiple instances, but it does not have a description
			 *       and/or language, then again the function call will be
			 *       identical to ID3::Tag::text(FrameID&). If no matching frame
			 *       has been found, then the returned Text struct will have empty
			 *       strings for every field.
			 * 
			 * NOTE: The filter function needs to take in two strings and return a
			 *       boolean. The first string parameter is the frame's description
			 *       and the second string parameter is the frame's language. Not
			 *       every frame that will be tested with the function will have
			 *       a description and/or language, in which case an empty string
			 *       will be passed instead.
			 * 
			 * @param frameName  An ID3v2 frame ID.
			 * @param filterFunc The filter function. See the notes above.
			 * @return The matching text content in a Text struct.
			 */
			Text text(const FrameID& frameName,
			          const std::function<bool (const std::string&, const std::string&)>& filterFunc) const;
			
			/**
			 * Return a vector Text structs with the text content, descriptions,
			 * and languages of all the frames on file with the given frame ID.
			 * 
			 * NOTE: The returned vector will always hold at least one Text struct,
			 *       even if no frames of the given frame ID were found on file.
			 *       See ID3::Text::exists(Frames) to check if the requested frame
			 *       ID actually exists in the tag.
			 * 
			 * NOTE: In ID3v2.3 some frames support multiple values, and in ID3v2.4
			 *       all text frames do. See ID3::Tag::textStrings(Frames) to get
			 *       the frame value split by the dividing character.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return A Text struct of the frame content.
			 */
			std::vector<Text> texts(const FrameID& frameName) const;
			
			/**
			 * Return the unprocessed binary data of the frame body of the
			 * requested frame.
			 * 
			 * NOTE: If the requested frame does not exist, the length of the
			 *       returned vector will be 0. The length will also be 0 if the
			 *       frame was created with this ID3::Tag object, and the changes
			 *       haven't been written to the file yet. See
			 *       ID3::Tag::exists(FrameID&) to check if the frame exists.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return The frame's binary data.
			 */
			ByteArray binaryData(const FrameID& frameName) const;
			
			/**
			 * Return the unprocessed binary data of the frame body of all the
			 * requested frames.
			 * 
			 * NOTE: If the requested frame does not exist, the length of the
			 *       returned vector will be 0. For each ByteArray in the returned
			 *       vector, its length will be 0 if the frame was created with
			 *       this ID3::Tag object, and the changes haven't been written to
			 *       the file yet.
			 * 
			 * @param frameName An ID3v2 frame ID.
			 * @return The binary data of all requested frames.
			 */
			std::vector<ByteArray> binaryDatas(const FrameID& frameName) const;
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			////////////////// E N D   F R A M E   G E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			//////////////// S T A R T   F R A M E   S E T T E R S ////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Set the text content of a frame. If the frame ID is not for text
			 * content, this method will do nothing.
			 * 
			 * NOTE: If there are more than one instance of the frame ID in the
			 *       tag, only the first frame will be modified.
			 * 
			 * @param frameID The ID3v2 frame ID.
			 * @param text    A Text struct containing new text content.
			 */
			void text(const FrameID& frameID, const Text& text);
			
			/** @see ID3::Tag::text(FrameID&, Text&) */
			void text(const FrameID& frameID, const std::string& text);
			
			/** @see ID3::Tag::text(FrameID&, Text&) */
			void text(const FrameID& frameID, const std::vector<std::string>& text);
			
			/**
			 * Set the text content of a frame.
			 * 
			 * NOTE: The function takes in a Text struct of the frame currently
			 *       being iterated over, and returns whether the given frame
			 *       should have its values changed to the Text struct given in the
			 *       method call.
			 * 
			 * NOTE: The method will loop over all frames with the given frame ID
			 *       and will not terminate after the first true return value from
			 *       the provided filter function. If no frame matches the given
			 *       filter function, a frame will be created from the Text struct,
			 *       provided the frame ID supports multiple instances of the frame
			 *       or no instance of the frame already exists.
			 * 
			 * @param frameID    The ID3v2 frame ID.
			 * @param text       The text to set.
			 * @param filterFunc The filter function (see above).
			 */
			void text(const FrameID&                           frameID,
			          const Text&                              text,
			          const std::function<bool (const Text&)>& filterFunc);
			
			/**
			 * Set the text content of a frame.
			 * 
			 * NOTE: The first parameter of the filter function is the text content
			 *       of the frame being iterated over, the second paramter is its
			 *       description, and the third its language. It returns whether
			 *       the given frame should have its values changed to the Text
			 *       struct given in the method call.
			 * 
			 * NOTE: The method will loop over all frames with the given frame ID
			 *       and will not terminate after the first true return value from
			 *       the provided filter function. If no frame matches the given
			 *       filter function, a frame will be created from the Text struct,
			 *       provided the frame ID supports multiple instances of the frame
			 *       or no instance of the frame already exists.
			 * 
			 * @param frameID    The ID3v2 frame ID.
			 * @param text       The text to set.
			 * @param filterFunc The filter function (see above).
			 */
			void text(const FrameID& frameID,
			          const Text&    text,
			          const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc);
			
			/**
			 * Set the text content of frames.
			 * 
			 * NOTE: The function takes in a Text struct of the frame currently
			 *       being iterated over, and returns a Text struct of what the
			 *       frame's value should be.
			 * 
			 * @param frameID       The ID3v2 frame ID.
			 * @param transformFunc The transformation function (see above).
			 */
			void text(const FrameID&                           frameID,
			          const std::function<Text (const Text&)>& transformFunc);
			
			/** @see ID3::text(FrameID&, Text&, std::function<bool (Text&)>&) */
			void text(const FrameID&                           frameID,
			          const std::string&                       text,
			          const std::function<bool (const Text&)>& filterFunc);
			
			/**
			 * @see ID3::text(FrameID&,
			 *                Text&,
			 *                std::function<bool (std::string&, std::string&, std::string&)>&)
			 */
			void text(const FrameID&     frameID,
			          const std::string& text,
			          const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc);
			
			/**
			 * Set the text content of frames.
			 * 
			 * NOTE: The function takes in the text content, description, and
			 *       language of the frame currently being iterated over, and
			 *       returns a string of what the frame's text content should be.
			 * 
			 * @param frameID       The ID3v2 frame ID.
			 * @param transformFunc The transformation function (see above).
			 */
			void text(const FrameID& frameID,
			          const std::function<std::string (const std::string&, const std::string&, const std::string&)>& transformFunc);
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			////////////////// E N D   F R A M E   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			/////////////// S T A R T   S P E C I F I C   F R A M E ///////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Get the title tag.
			 * 
			 * @return The title of the tag, or "" if no title is set.
			 */
			inline std::string title() const { return textString(FRAME_TITLE); }
			/**
			 * Set the title tag.
			 * 
			 * @param newTitle The new title.
			 */
			inline void title(const std::string& newTitle) { text(FRAME_TITLE, newTitle); }
			
			/**
			 * Get the genre tag.
			 * 
			 * @param process If true, this will process the genre string according
			 *                to the specification of the TCOM frame. It will look
			 *                for a number surrounded by parenthesis. If found, the
			 *                substring will be removed, and if no text follows the
			 *                substring, the number will be parsed and the
			 *                corresponding ID3v1 genre will be returned.
			 *                If false, the raw genre string as it appears on file
			 *                will be returned.
			 * @return The genre of the tag, or "" if no genre is set.
			 */
			std::string genre(bool process=true) const;
			/** @see ID3::Tag::genre(bool)
			 *  @see ID3::Tag::textStrings(Frames) */
			std::vector<std::string> genres(bool process=true) const;
			/**
			 * Set the genre tag.
			 * 
			 * @param newGenre The new genre.
			 */
			inline void genre(const std::string& newGenre) { text(FRAME_GENRE, newGenre); }
			/**
			 * Set the genre tag.
			 * 
			 * @param newGenre The new genre, which is an ID3v1 genre ID.
			 */
			void genre(const ushort newGenre);
			/** @see ID3::Tag::genre(std::string&) */
			inline void genres(const std::vector<std::string>& newGenres) { text(FRAME_GENRE, newGenres); }
			
			/**
			 * Get the artist tag.
			 * 
			 * @return The artist of the tag, or "" if no artist is set.
			 */
			inline std::string artist() const { return textString(FRAME_ARTIST); }
			/** @see ID3::Tag::artist()
			 *  @see ID3::Tag::textStrings(Frames) */
			inline std::vector<std::string> artists() const { return textStrings(FRAME_ARTIST); }
			/**
			 * Set the artist tag.
			 * 
			 * @param newArtist The new artist.
			 */
			inline void artist(const std::string& newArtist) { text(FRAME_ARTIST, newArtist); }
			/** @see ID3::Tag::artist(std::string&) */
			inline void artist(const std::vector<std::string>& newArtists) { text(FRAME_ARTIST, newArtists); }
			/** @see ID3::Tag::artist(std::string&) */
			inline void artists(const std::vector<std::string>& newArtists) { text(FRAME_ARTIST, newArtists); }
			
			/**
			 * Get the album tag.
			 * 
			 * @return The album of the tag, or "" if no album is set.
			 */
			inline std::string album() const { return textString(FRAME_ALBUM); }
			
			/** @see ID3::Tag::album()
			 *  @see ID3::Tag::textStrings(FrameID&) */
			inline std::vector<std::string> albums() const { return textStrings(FRAME_ALBUM); }
			/**
			 * Set the album tag.
			 * 
			 * @param newAlbum The new album.
			 */
			inline void album(const std::string& newAlbum) { text(FRAME_ALBUM, newAlbum); }
			/** @see ID3::Tag::album(std::string&) */
			inline void album(const std::vector<std::string>& newAlbums) { text(FRAME_ALBUM, newAlbums); }
			/** @see ID3::Tag::album(std::string&) */
			inline void albums(const std::vector<std::string>& newAlbums) { text(FRAME_ALBUM, newAlbums); }
			
			/**
			 * Get the album artist/band/orchestra/accompaniment tag.
			 * 
			 * @return The album artist of the tag, or "" if no album artist is set.
			 */
			inline std::string albumArtist() const  { return textString(FRAME_ALBUM_ARTIST); }
			/** @see ID3::Tag::albumArtist()
			 *  @see ID3::Tag::textStrings(FrameID&) */
			inline std::vector<std::string> albumArtists() const { return textStrings(FRAME_ALBUM_ARTIST); }
			/**
			 * Set the album artist/band/orchestra/accompaniment tag.
			 * 
			 * @param newAlbumArtist The new album artist.
			 */
			inline void albumArtist(const std::string& newAlbumArtist) { text(FRAME_ALBUM_ARTIST, newAlbumArtist); }
			/** @see ID3::Tag::albumArtist(std::string&) */
			inline void albumArtist(const std::vector<std::string>& newAlbumArtists) { text(FRAME_ALBUM_ARTIST, newAlbumArtists); }
			/** @see ID3::Tag::albumArtist(std::string&) */
			inline void albumArtists(const std::vector<std::string>& newAlbumArtists) { text(FRAME_ALBUM_ARTIST, newAlbumArtists); }
			
			/**
			 * Get the year tag. It first looks for the original recording time
			 * frame, and if it can't find that then it looks for the year frame.
			 * 
			 * @return The year of the tag, or "" if no year is set.
			 */
			inline std::string year() const { return exists(FRAME_RECORDING_TIME) ?
			                                         textString(FRAME_RECORDING_TIME).substr(0,4) :
			                                         textString(FRAME_YEAR); }
			/**
			 * Set the year tag. This sets both the ID3v2.3 year frame, and the
			 * ID3v2.4 recording time frame.
			 * 
			 * NOTE: The year must be a valid integer number, or else the year will
			 *       be set to a blank string. If it is not a valid integer, then
			 *       the TDRC (recording time) frame will be reset. If the year is
			 *       more than 4 digits long the fifth and beyond digits will be
			 *       trimmed off, and if below and not an empty string 0's will be
			 *       prepended until it is four characters long.
			 * 
			 * @param newYear The new year.
			 */
			void year(const std::string& newYear);
			/** @see ID3::Tag::year(std::string&) */
			inline void year(const ushort newYear) { year(std::to_string(newYear)); }
			
			/**
			 * Get the track tag.
			 * 
			 * NOTE: If there is a slash ('/') in the track, it and the following
			 *       text will not be returned. See ID3::Tag::trackTotal() to get
			 *       the track total.
			 * 
			 * @return The track of the tag, or "" if no track is set.
			 * @see ID3::Tag::trackTotal()
			 */
			std::string track() const;
			/**
			 * Set the track tag.
			 * 
			 * NOTE: The track must be a valid integer. The track total can also be
			 *       set if added after a slash ('/') after the track number, or
			 *       ID3::Tag::trackTotal(std::string&) can be used. The track
			 *       total must also be a valid integer number.
			 * 
			 * @param newTrack The new track.
			 * @see ID3::Tag::trackTotal(std::string&)
			 */
			void track(const std::string& newTrack);
			/** @see ID3::Tag::track(std::string&) */
			inline void track(const ulong newTrack) { track(std::to_string(newTrack)); }
			/**
			 * Get the total number of tracks in the set of the original recording.
			 * This is taken from the TRCK frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @return The track total of the tag, or "" if no track total is set.
			 * @see ID3::Tag::track()
			 */
			std::string trackTotal() const;
			/**
			 * Set the track total of the track tag.
			 * 
			 * NOTE: The track total must be a valid integer.
			 * 
			 * @param newTrackTotal The new track total.
			 * @see ID3::Tag::track(std::string&)
			 */
			void trackTotal(const std::string& newTrackTotal);
			/** @see ID3::Tag::trackTotal(std::string&) */
			inline void trackTotal(const ulong newTrackTotal) { text(FRAME_TRACK, track()+'/'+std::to_string(newTrackTotal)); }
			
			/**
			 * Get the disc number tag.
			 * 
			 * NOTE: If there is a slash ('/') in the disc number, it and the
			 *       following text will not be returned. See ID3::Tag::discTotal()
			 *       to get the disc total.
			 * 
			 * @return The disc number of the tag, or "" if no disc is set.
			 * @see ID3::Tag::discTotal()
			 */
			std::string disc() const;
			/**
			 * Set the disc tag.
			 * 
			 * NOTE: The disc must be a valid integer. The disc total can also be
			 *       set if added after a slash ('/') after the disc number, or
			 *       ID3::Tag::discTotal(std::string&) can be used. The disc
			 *       total must also be a valid integer number.
			 * 
			 * @param newDisc The new disc number.
			 * @see ID3::Tag::discTotal(std::string&)
			 */
			void disc(const std::string& newDisc);
			/** @see ID3::Tag::track(std::string&) */
			inline void disc(const ulong newDisc) { disc(std::to_string(newDisc)); }
			/**
			 * Get the total number of discs in the set of the original recording.
			 * This is taken from the TPOS frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @return The disc total of the tag, or "" if no disc total is set.
			 * @see ID3::Tag::disc()
			 */
			std::string discTotal() const;
			/**
			 * Set the disc total of the disc tag.
			 * 
			 * NOTE: The disc total must be a valid integer.
			 * 
			 * @param newDiscTotal The new disc total.
			 * @see ID3::Tag::track(std::string&)
			 */
			void discTotal(const std::string& newDiscTotal);
			/** @see ID3::Tag::discTotal(std::string&) */
			inline void discTotal(const ulong newDiscTotal) { text(FRAME_DISC, disc()+'/'+std::to_string(newDiscTotal)); }
			
			/**
			 * Get the composer tag.
			 * 
			 * @return The composer of the tag, or "" if no composer is set.
			 */
			inline std::string composer() const { return textString(FRAME_COMPOSER); }
			/** @see ID3::Tag::composer()
			 *  @see ID3::Tag::textStrings(FrameID&) */
			inline std::vector<std::string> composers() const { return textStrings(FRAME_COMPOSER); }
			/**
			 * Set the composer tag.
			 * 
			 * @param newComposer The new composer.
			 */
			inline void composer(const std::string& newComposer) { text(FRAME_COMPOSER, newComposer); }
			/** @see ID3::Tag::composer(std::string&) */
			inline void composer(const std::vector<std::string>& newComposers) { text(FRAME_COMPOSER, newComposers); }
			/** @see ID3::Tag::composer(std::string&) */
			inline void composers(const std::vector<std::string>& newComposers) { text(FRAME_COMPOSER, newComposers); }
			
			/**
			 * Get the BPM tag.
			 * 
			 * @return The BPM of the tag, or "" if no BPM is set.
			 */
			inline std::string bpm() const { return textString(FRAME_BPM); }
			/**
			 * Set the BPM tag.
			 * 
			 * NOTE: The BPM must be a valid integer.
			 * 
			 * @param newBPM The new BPM.
			 */
			inline void bpm(const std::string& newBPM) { text(FRAME_BPM, newBPM); }
			/** @see ID3::Tag::bpm(std::string&) */
			inline void bpm(const ulong newBPM) { text(FRAME_BPM, std::to_string(newBPM)); }
			
			/**
			 * Get the tag comments as a vector of Text structs.
			 * 
			 * @see ID3::Tag::texts(FrameID&)
			 */
			inline std::vector<Text> comments() const { return texts(FRAME_COMMENT); }
			/**
			 * Get a specific comment in the tag as a Text struct.
			 * 
			 * @see ID3::Tag::comments()
			 * @see ID3::Tag::text(FrameID&, std::function&)
			 */
			Text comment(const std::function<bool (const std::string&, const std::string&)>& filterFunc) const { return text(FRAME_COMMENT, filterFunc); }
			/**
			 * Set or create the first comment tag.
			 * 
			 * @see ID3::Tag::text(FrameID&, Text&)
			 */
			inline void comment(const Text& newComment) { text(FRAME_COMMENT, newComment); }
			/**
			 * Set a specific comment tag.
			 * 
			 * @see ID3::Tag::text(FrameID&, Text&, std::function<bool (Text&)>&)
			 */
			inline void comment(const Text& newComment,
			                    const std::function<bool (const Text&)>& filterFunc) { text(FRAME_COMMENT, newComment, filterFunc); }
			/** @see ID3::Tag::comment(Text&, std::function<bool (Text&)>&)
			 *  @see ID3::Tag::text(FrameID&, Text&, std::function<bool (std::string&, std::string&, std::string&)>&) */
			inline void comment(const Text& newComment,
			                    const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc) {
				text(FRAME_COMMENT, newComment, filterFunc);
			}
			/** @see ID3::Tag::comment(Text&, std::function<bool (Text&)>&)
			 *  @see ID3::Tag::text(FrameID&, Text&, std::function<bool (std::string&, std::string&, std::string&)>&) */
			inline void comment(const std::string& newComment,
			                    const std::function<bool (const std::string&, const std::string&, const std::string&)>& filterFunc) {
				text(FRAME_COMMENT, newComment, filterFunc);
			}
			
			/**
			 * Get the first attached picture.
			 * 
			 * @return The attached picture, encapsulated in a ID3::Picture struct.
			 *         If there is no attached picture, or it has an improper
			 *         MIME type, the null field will be true.
			 */
			Picture picture() const;
			/**
			 * Return a Picture struct that matches the given function.
			 * 
			 * NOTE: The filter function needs to take in a string and return a
			 *       boolean. The first parameter is the picture's description, and
			 *       the second is the picture type. If no matching picture is
			 *       found, a "null" Picture struct will be returned instead.
			 * 
			 * @param filterFunc The filter function. See the note above.
			 * @return The matching picture in a Picture struct.
			 */
			Picture picture(const std::function<bool (const std::string&, const PictureType type)>& filterFunc) const;
			/**
			 * Return a Picture struct that matches the given description and type.
			 * 
			 * NOTE: If no picture matches the given description, a "null" Picture
			 *       struct will be returned instead.
			 * 
			 * @param description The description of the picture you wish to retrieve.
			 * @param type        The ID3::PictureType enum picure type value (defaults to the front cover).
			 * @return The matching picture as a Picture struct.
			 */
			inline Picture picture(const std::string& description, const PictureType type=PictureType::FRONT_COVER) const {
				return picture([&description, type](const std::string& desc, const PictureType picType) -> bool {
					return type == picType && description == desc;
				});
			}
			/**
			 * Get a vector of all attached pictures.
			 * 
			 * NOTE: If there are no pictures in the tag then the returned vector
			 *       will be empty.
			 * 
			 * @return A vector of Picture structs.
			 */
			std::vector<Picture> pictures() const;
			/**
			 * Set a picture.
			 * 
			 * NOTE: If a picture already exists with the same description, it will
			 *       be overriden by the given picture.
			 * 
			 * NOTE: If the picture type is FILE_ICON or OTHER_FILE_ICON, it will
			 *       also override any saved picture with that type regardless of
			 *       the description.
			 * 
			 * NOTE: The picture data being given is not validated. If the given
			 *       Picture struct is "null", then it won't be written to file
			 *       when calling a write() method.
			 * 
			 * @param newPicture The new picture to set.
			 * @throws ID3::FrameSizeException when the Picture is too big to fit
			 *         in a frame (at 256MiB).
			 */
			void picture(const Picture& newPicture);
			
			/**
			 * Get the play count.
			 * 
			 * NOTE: This first searches for the Play Count frame. If not found, it
			 *       looks for the first Popularimeter frame. If that is not found
			 *       as well, it returns 0.
			 * 
			 * @return The play count saved on the file.
			 */
			unsigned long long playCount() const;
			/**
			 * Get the play count from a Popularimeter that matches the given function.
			 * 
			 * NOTE: Unlike the ID3::Tag::playCount() method, this method looks
			 *       through the Popularimeter frames first. If no matching
			 *       Popularimeter is found, and no Popularimeters exist on file,
			 *       then it will look for the Play Count frame. Failing finding
			 *       that frame, it will return 0.
			 * 
			 * NOTE: The filter function needs to take in a string and return a
			 *       boolean. The string parameter is the Popularimeter's email.
			 * 
			 * @param filterFunc The filter function. See the notes above.
			 * @return The matching play count, the Play Count frame's value, or 0.
			 */
			unsigned long long playCount(const std::function<bool (const std::string&)>& filterFunc) const;
			/**
			 * Get the play count from a Popularimeter that matches the given function.
			 * 
			 * NOTE: Unlike the ID3::Tag::playCount() method, this method looks
			 *       through the Popularimeter frames first. If no matching
			 *       Popularimeter is found, and no Popularimeters exist on file,
			 *       then it will look for the Play Count frame. Failing finding
			 *       that frame, it will return 0.
			 * 
			 * @param email The email of the popularimeter to get.
			 * @return The matching play count, the Play Count frame's value, or 0.
			 */
			inline unsigned long long playCount(const std::string& email) const {
				return playCount([&email](const std::string& address) -> bool { return email == address; });
			}
			/**
			 * Set the play count of a specific Popularimeter frame.
			 * 
			 * NOTE: This method only updates the PCNT frame, not the POPM frame.
			 *       To update a POPM frame, use
			 *       ID3::Tag::playCount(unsigned long long, std::string&) instead.
			 * 
			 * @param count The new play count.
			 */
			void playCount(const unsigned long long count);
			
			/**
			 * Set the play count.
			 * 
			 * NOTE: If updating the play count with this method, it is recommended
			 *       to use ID3::Tag::playCount(std::function&) or
			 *       ID3::Tag::playCount(std::string&) only when retrieving the
			 *       play count, as this method only updates/creates POPM frames
			 *       and ID3::Tag::playCount() only retrieves the PCNT frame.
			 * 
			 * @param count The new play count.
			 * @param email The email address of the Popularimeter.
			 */
			void playCount(const unsigned long long count, const std::string& email);
			
			/**
			 * Get the 5-star rating from the first Popularimeter frame, or 0 if
			 * none are found.
			 * 
			 * @return The rating on file. The values are 0-5.
			 */
			ushort rating() const;
			/**
			 * Return the rating that matches the given function.
			 * 
			 * NOTE: The filter function needs to take in a string and return a
			 *       boolean. The string parameter is the Popularimeter's email.
			 *       If no matching Popularimeter is found, then 0 will be returned.
			 * 
			 * @param filterFunc The filter function. See the note above.
			 * @return The matching play count or 0.
			 */
			ushort rating(const std::function<bool (const std::string&)>& filterFunc) const;
			/**
			 * Return the rating that matches the given function.
			 * 
			 * NOTE: If no matching Popularimeter is found, then 0 will be returned.
			 * 
			 * @param email The email of the popularimeter to get.
			 * @return The matching play count or 0.
			 */
			inline ushort rating(const std::string& email) const {
				return rating([&email](const std::string& address) -> bool { return email == address; });
			}
			/**
			 * Set the rating of a specific Popularimeter frame.
			 * 
			 * @param rating The new 1-5 star rating.
			 * @param email  The email address of the Popularimeter.
			 */
			void rating(const uint8_t rating, const std::string& email);
			
			/**
			 * Return the value of the given event timing code.
			 * 
			 * NOTE: If the event timing code does not exist on file, the returned
			 *       EventTimingCode will have a value of 0.
			 * 
			 * @param code The event timing code to retrieve.
			 * @return The event timing code, wrapped in a EventTimingCode struct.
			 */
			EventTimingCode timingCode(const TimingCodes code) const;
			/**
			 * Set an event timing code.
			 * 
			 * @param code              The timing code TimingCode enum value.
			 * @param value             The value of the timing code.
			 * @param forceMilliseconds If true, and the existing event timing
			 *                          codes use MPEG frames, they will be cleared.
			 *                          If false, the value will use the existing
			 *                          timestamp format.
			 */
			void timingCode(const TimingCodes code,
			                const ulong       value,
			                const bool        forceMilliseconds=false);
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////// E N D   S P E C I F I C   F R A M E /////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Returns a string representation of the ID3 versions used in the file.
			 * 
			 * @param verbose If true, it will print the flags of the ID3v2 version.
			 * @return The string representation of the versions used.
			 */
			std::string getVersionString(bool verbose=false) const;
			
			/**
			 * @returns The number of frames stored in the Tag object.
			 */
			size_t size() const;
			
			/**
			 * @returns The filename last given in write(std::string&), or the
			 *          filename given in the constructor
			 *          ID3::Tag::Tag(std::string&) if the write method was never
			 *          called, or "" if neither were called.
			 */
			std::string fileName() const;
			
			/**
			 * @returns The size of the file last given in a write method, or the
			 *          size of the file given in the constructor if a write method
			 *          hasn't been called, or 0 if a write method hasn't been
			 *          called and the Tag was created with the empty constructor.
			 */
			ulong fileSize() const;
			
			/**
			 * Print all the tag information.
			 */
			void print() const;
			
		private:
			/**
			 * A struct that records what ID3 versions a file contains.
			 */
			struct TagsOnFile {
				TagsOnFile();
				bool v1;
				bool v1_1;
				bool v1Extended;
				bool v2;
			};
			
			/**
			 * A struct that records the ID3v2 information.
			 */
			struct TagInfo {
				TagInfo();                  //Constructor
				ushort majorVer;            //ID3v2 major version
				ushort minorVer;            //ID3v2 minor version
				bool flagUnsynchronisation; //Unsynchronisation flag
				bool flagExtHeader;         //Extended header flag
				bool flagExperimental;      //Experimental flag
				bool flagFooter;            //Footer flag
				ulong size;                 //Tag size
				ulong totalSize;            //Total tag size (tag size + header size
				                            // + extended header size + footer size)
				ulong paddingStart;         //The byte in which padding starts
			};
			
			/**
			 * A 10-bit struct that captures the structure of the ID3v2.3 extended header.
			 */
			struct V3ExtHeader {
				uint8_t size[4];
				uint8_t flags[2];
				uint8_t paddingSize[4];
			};
			
			/**
			 * An 8-bit struct that captures the structure of the ID3v2.4 extended header.
			 */
			struct V4ExtHeader {
				uint8_t size[4]; //A synchsafe integer
				uint8_t flagBytes;
				uint8_t flags;
			};
			
			/**
			 * Create a Tag by reading from a file, but don't process the frames.
			 * Used internally for the write method.
			 * 
			 * @param fileLoc    The file location.
			 * @param readFrames Whether to read frames or not.
			 */
			Tag(const std::string& fileLoc, const bool readFrames);
			
			/**
			 * Add a frame to the FrameMap. If there already exists a frame with
			 * the same ID, and ID3::allowsMulipleFrames(frameName) returns false,
			 * then the frame will not be added. Frames will also not be added if
			 * they are "null", empty, or if the FramePtr holds a null pointer.
			 * 
			 * @param frameName The 4-letter ID3v2 frame ID.
			 * @param frame     The shared pointer holding a Frame object.
			 * @return true if the frame was succesfully added, false otherwise.
			 */
			bool addFrame(const FrameID& frameName, FramePtr frame);
			
			/**
			 * @see ID3::Tag::addFrame(std::string&, FramePtr)
			 */
			bool addFrame(const FramePair& frameMapPair);
			
			/**
			 * A protected method to get a Frame from the FrameMap.
			 * If the requested frame is not in the map, "null", or if it's not the
			 * same class as the template class, then a null pointer will be
			 * returned. This method uses dynamic_cast() to cast the Frame to the
			 * derived Frame class.
			 * 
			 * If there is more than one Frame with the same frame name in the
			 * map, only the first Frame in the map will be returned.
			 * 
			 * @param frameName      The name of the frame.
			 * @return The Frame in the map, or nullptr.
			 */
			template<typename DerivedFrame>
			DerivedFrame* getFrame(const FrameID& frameName) const;
			
			/**
			 * A protected method to get a Frame from the FrameMap.
			 * If the requested frame is not in the map, "null", or if it's not the
			 * same class as the template class, then a null pointer will be
			 * returned. This method uses dynamic_cast() to cast the Frame to the
			 * derived Frame class.
			 * 
			 * If there is more than one Frame with the same frame name in the
			 * map, only the first Frame in the map will be returned.
			 * 
			 * @param frameName      The name of the frame.
			 * @param mismatchDelete If there is a frame at frameName, but it is a
			 *                       UnknownFrame instead of a DerivedFrame, or it
			 *                       is "null", then delete the frame at FrameID.
			 *                       the class DerivedFrame, delete it.
			 * @return The Frame in the map, or nullptr.
			 */
			template<typename DerivedFrame>
			DerivedFrame* getFrame(const FrameID& frameName,
			                       const bool mismatchDelete=false);
			
			/**
			 * A protected method to get a Frame* vector from the FrameMap.
			 * If the requested frame is not in the map, then an empty vector will
			 * be returned. Additionally, in the range of Frames within the
			 * FrameMap, if the Frame cannot be dynamic_cast-ed to DerivedFrame or
			 * it is "null", then it will not be added to the vector.
			 * 
			 * Not all frames support multiple instances of the frame. For frames
			 * that do not, ID3::Tag::getFrame(Frames) is better to use.
			 * 
			 * @param frameName The name of the frame.
			 * @return A Frame vector of all Frames that were found.
			 */
			template<typename DerivedFrame>
			std::vector<DerivedFrame*> getFrames(const FrameID& frameName) const;
			
			/**
			 * Create a Text struct from a Frame object.
			 * 
			 * @param frame The Frame object.
			 * @return An empty Text struct if the Frame is not a TextFrame, and a
			 *         Text struct with a blank description and language if not a
			 *         DescriptiveTextFrame.
			 */
			inline Text getTextStruct(const Frame* const frame) const;
			
			/**
			 * A constructor helper method that reads the ID3 tags from the file.
			 * 
			 * @param file       The file stream object.
			 * @param readFrames Whether to read frames or not.
			 */
			void readFile(std::istream& file, const bool readFrames=false);
			
			/**
			 * A constructor helper method that reads the ID3v1 tags from the file.
			 * 
			 * @param file The file stream object.
			 * @param readFrames Whether to read frames or not.
			 */
			void readFileV1(std::istream& file, const bool readFrames=false);
			
			/**
			 * A constructor helper method that reads the ID3v2 tags from the file.
			 * 
			 * @param file       The file stream object.
			 * @param readFrames Whether to read frames or not.
			 * @throws ID3::FileFormatException if the ID3v2 tags on file are
			 *         supposedly bigger than the file itself.
			 */
			void readFileV2(std::istream& file, const bool readFrames=false);
			
			/**
			 * A constructor helper method that gets a v1 tag struct and sets the class'
			 * variables to the information in the struct.
			 * 
			 * @param tags The ID3v1 tag struct.
			 * @param zeroCheck If true, this will check to see if the tags is actually
			 *                  set to ID3v1.1.
			 */
			void setTags(const V1::Tag& tags, bool zeroCheck=true);
			
			/**
			 * A constructor helper method that gets a v1.1 tag struct and sets the class'
			 * variables to the information in the struct.
			 * 
			 * @param tags The ID3v1.1 tag struct.
			 * @param zeroCheck If true, this will check to see if the tags is actually
			 *                  set to ID3v1.
			 */
			void setTags(const V1::P1Tag& tags, bool zeroCheck=true);
			
			/**
			 * A constructor helper method that gets a v1 Extended tag struct and sets the class'
			 * variables to the information in the struct. This will override any information
			 * previously set from a v1 or v1.1 tag.
			 * 
			 * @param tags The ID3v1 Extended tag struct.
			 */
			void setTags(const V1::ExtendedTag& tags);
			
			/**
			 * A TagsOnFile struct that records all the ID3 versions
			 * that were found on the file.
			 */
			TagsOnFile tagsSet;
			
			/**
			 * A V2TagInfo struct that records the ID3v2 internal
			 * information that were found on the file.
			 */
			TagInfo v2TagInfo;
			
			/**
			 * A map of all frames stored in the tag.
			 */
			FrameMap frames;
			
			/**
			 * The FrameFactory to create Frame objects.
			 */
			FrameFactory factory;
			
			/**
			 * The filename (if not getting the file via an istream object).
			 * 
			 * @see ID3::Tag::file()		 
			 */
			std::string filename;
			
			/**
			 * The size of the file.
			 * 
			 * @see ID3::Tag::filesize()
			 */
			ulong filesize;
	};
}

#endif
