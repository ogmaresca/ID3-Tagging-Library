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

#ifndef ID3_H
#define ID3_H

#include <fstream>       //For std::ifstream
#include <vector>        //For std::vector
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
 * 
 * @todo Add write support.
 * @todo Test it on a greater variety of ID3 files, and unit tests.
 * @todo Read the ID3v2 Extended Header.
 */
namespace ID3 {
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////// T Y P E D E F S //////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	typedef std::vector<uint8_t> ByteArray;
	typedef std::shared_ptr<Frame> FramePtr;
	typedef std::unordered_multimap<std::string, FramePtr> FrameMap;
	typedef std::pair<std::string, FramePtr> FramePair;
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// E N U M S /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 enums.
	 */
	/*namespace V1 {
		**
		 * An enum of speeds defined in ID3v1 Extended.
		 */
		/*enum ExtendedSpeeds {
			UNSET    = 0,
			SLOW     = 1,
			MEDIUM   = 2,
			FAST     = 3,
			HARDCORE = 4
		};
	}*/
	
	/**
	 * An enum containing the highest value for each star rating for the
	 * Popularimeter (POPM) frame in ID3v2.
	 * 
	 * @todo Properly implement this into a POPM Frame class.
	 */
	/*enum class POPMRatingCutoff {
		ZERO  = 0,   //0-0
		ONE   = 31,  //1-31
		TWO   = 95,  //32-95
		THREE = 159, //96-159
		FOUR  = 223, //160-223
		FIVE  = 255  //224-255
	};*/
	
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
	 * Every field of this struct is constant, you must create a new Picture to
	 * modify a field.
	 * The null variable will be set to true if given an invalid MIME type. The
	 * picture data is not checked to verify that it is a valid image.
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
		const std::string MIME;
		const PictureType type;
		const std::string description;
		const ByteArray   data;
		const bool        null;
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
		 * @param textContent The content string of the frame.
		 * @param descText    The description of the frame.
		 * @param langText    The language of the frame. It should be a ISO 639-2
		 *                    language code.
		 */
		Text(const std::string& textContent="",
		     const std::string& descText="",
		     const std::string& langText="") : text(textContent),
		                                       description(descText),
		                                       language(langText) {}
		const std::string text;
		const std::string description;
		const std::string language;
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
		friend class FrameFactory;
		
		public:
			/**
			 * Constructor that takes a filename and opens the file.
			 * 
			 * @param fileLoc The file path.
			 */
			Tag(const std::string& fileLoc);
			
			/**
			 * Constructor that takes an ifstream file object and reads the ID3 data.
			 * NOTE: This does NOT close the file. You must call file_obj.close() after
			 *       instantiating an ID3::Tag object with this constructor.
			 * NOTE: The file must be open to read the tags.
			 * NOTE: This constructor does not check to see if the file
			 *       is an MP3 file or not.
			 * 
			 * @param file The ifstream file object.
			 */
			Tag(std::ifstream& file);
			
			/**
			 * A constructor to set default values.
			 * Creating a frame with this constructor will result in a
			 * "null" frame.
			 */
			Tag();
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			//////////////////////// S T A R T   F R A M E ////////////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Check if a frame exists.
			 * 
			 * @param frameName A FrameID object, or either optionally a string
			 *                  containing an ID3 frame ID or a Frames enum value.
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
			 * @param frameName A Frames enum variable that represents a frame ID.
			 * @return The text content, or "" if the frame is not found, "null",
			 *         or not a text frame.
			 */
			std::string textString(const Frames frameName) const;
			
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
			 * @param frameName A Frames enum variable that represents a frame ID.
			 * @return A string vector of the text content.
			 */
			std::vector<std::string> textStrings(const Frames frameName) const;
			
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
			 * @param frameName A Frames enum variable that represents a frame ID.
			 * @return A Text struct of the frame content.
			 */
			Text text(const Frames frameName) const;
			
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
			 * @param frameName A Frames enum variable that represents a frame ID.
			 * @return A Text struct of the frame content.
			 */
			std::vector<Text> texts(const Frames frameName) const;
			
			/**
			 * Get the title tag.
			 * 
			 * @return The title of the music file, or "" if there is
			 *         no title set.
			 */
			std::string title() const;
			
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
			 * @return The genre of the music file, or "" if there is
			 *         no title set.
			 */
			std::string genre(bool process=true) const;
			
			/**
			 * Get the genre tag as a vector of genre string(s).
			 * 
			 * @see ID3::Tag::genre(bool)
			 * @see ID3::Tag::textStrings(Frames)
			 */
			std::vector<std::string> genres(bool process=true) const;
			
			/**
			 * Get the artist tag.
			 * 
			 * @return The artist of the music file, or "" if there is
			 *         no title set.
			 */
			std::string artist() const;
			
			/**
			 * Get the artist tag as a vector of artist string(s).
			 * 
			 * @see ID3::Tag::artist()
			 * @see ID3::Tag::textStrings(Frames)
			 */
			std::vector<std::string> artists() const;
			
			/**
			 * Get the album tag.
			 * 
			 * @return The album of the music file, or "" if there is
			 *         no title set.
			 */
			std::string album() const;
			
			/**
			 * Get the album tag as a vector of album string(s).
			 * 
			 * @see ID3::Tag::album()
			 * @see ID3::Tag::textStrings(Frames)
			 */
			std::vector<std::string> albums() const;
			
			/**
			 * Get the album artist/band/orchestra/accompaniment tag.
			 * 
			 * @return The Album artist of the music file, or "" if
			 *         there is no tag set.
			 */
			std::string albumArtist() const;
			
			/**
			 * Get the album artist/band/orchestra/accompaniment tag as a vector of
			 * album artist string(s).
			 * 
			 * @see ID3::Tag::albumArtist()
			 * @see ID3::Tag::textStrings(Frames)
			 */
			std::vector<std::string> albumArtists() const;
			
			/**
			 * Get the year tag.
			 * 
			 * @return The year of the music file, or "" if there is
			 *         no year set.
			 */
			std::string year() const;
			
			/**
			 * Get the track tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                It will also ignore any text after a slash
			 *                ('/') in the frame content, 
			 *                If false, the raw track string as it
			 *                appears in the TRCK frame on file will be
			 *                returned.
			 * @return The track of the music file, or "" if there is
			 *         no track set.
			 * @see ID3::Tag::trackTotal(bool)
			 */
			std::string track(bool process=true) const;
			
			/**
			 * Get the total number of tracks in the set of the original recording.
			 * This is taken from the TRCK frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the value is not an integer.
			 *                If false, the raw string as it appears
			 *                on file after a slash will be returned.
			 * @return The track total of the music file, or "" if there
			 *         is no track total set.
			 * @see ID3::Tag::track(bool)
			 */
			std::string trackTotal(bool process=true) const;
			
			/**
			 * Get the disc tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                It will also ignore any text after a slash
			 *                ('/') in the frame content, 
			 *                If false, the raw disc string as it
			 *                appears in the TPOS frame on file will be
			 *                returned.
			 * @return The disc number of the music file, or "" if there
			 *         is no disc number set.
			 * @see ID3::Tag::discTotal(bool)
			 */
			std::string disc(bool process=true) const;
			
			/**
			 * Get the total number of discs in the set of the original recording.
			 * This is taken from the TPOS frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the value is not an integer.
			 *                If false, the raw string as it appears
			 *                on file after a slash will be returned.
			 * @return The disc total of the music file, or "" if there is
			 *         no disc total set.
			 * @see ID3::Tag::disc(bool)
			 */
			std::string discTotal(bool process=true) const;
			
			/**
			 * Get the composer tag.
			 * 
			 * @return The composer of the music file, or "" if there is
			 *         no composer set.
			 */
			std::string composer() const;
			
			/**
			 * Get the composer tag as a vector of composer string(s).
			 * 
			 * @see ID3::Tag::composer()
			 * @see ID3::Tag::textStrings(Frames)
			 */
			std::vector<std::string> composers() const;
			
			/**
			 * Get the BPM tag.
			 * 
			 * @return The bpm of the music file, or "" if there is
			 *         no bpm set.
			 */
			std::string bpm() const;
			
			/**
			 * Get the tag comments as a vector of Text structs.
			 * 
			 * @see ID3::Tag::texts(Frames)
			 */
			std::vector<Text> comments() const;
			
			/**
			 * Get the attached picture.
			 * 
			 * @return The attached picture, encapsulated in a ID3::Picture struct.
			 *         If there is no attached picture, or it has an improper
			 *         MIME type, the null field will be true.
			 */
			Picture picture() const;
			
			/**
			 * Get a vector of all attached pictures.
			 * 
			 * NOTE: Unlike the getter methods for text frames, if there are no
			 *       pictures stored in the tag then the returned vector will be
			 *       empty.
			 * 
			 * @return A vector of Picture structs.
			 */
			std::vector<Picture> pictures() const;
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			////////////////////////// E N D   F R A M E //////////////////////////
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
			 * @returns true if the given file is a valid MP3 file, false
			 *          otherwise.
			 */
			const bool null() const;
			
			/**
			 * Print all the tag information.
			 */
			void print() const;
			
		protected:
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
				ushort majorVer;             //ID3v2 major version
				ushort minorVer;             //ID3v2 minor version
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
			 * The filesize of the music file.
			 */
			ulong filesize;
			
			/**
			 * The filename (if not getting the file via an ifstream object).			 
			 */
			std::string filename;
			
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
			bool addFrame(const std::string& frameName, FramePtr frame);
			
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
			 * @param frameName The name of the frame.
			 * @return The Frame in the map, or nullptr.
			 */
			template<typename DerivedFrame>
			DerivedFrame* getFrame(FrameID frameName) const;
			
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
			std::vector<DerivedFrame*> getFrames(FrameID frameName) const;
			
			/**
			 * A constructor helper method that gets the tag information from the given file.
			 * 
			 * @param file The file object.
			 */
			void readFile(std::ifstream& file);
			
			/**
			 * A constructor helper method that reads the ID3v1 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void readFileV1(std::ifstream& file);
			
			/**
			 * A constructor helper method that reads the ID3v2 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void readFileV2(std::ifstream& file);
			
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
			 * This will be true if the given file is a valid MP3 file, and
			 * false otherwise. Call null() to check the value externally.
			 * 
			 * @see ID3::Tag::null()
			 */
			bool isNull;
			
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
			 * A map of all frames created from ID3v1 tags and read from
			 * ID3v2 frames.
			 */
			FrameMap frames;
	};
}

#endif
