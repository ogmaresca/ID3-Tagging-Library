/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 **********************************************************************/

#ifndef ID3_H
#define ID3_H

#include <fstream>
#include <vector>
#include <unordered_map>

#include "ID3Frame.h"

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * Items relating to ID3v1 are kept in the namespace ID3::V1. V2 related
 * items are kept within the ID3 namespace.
 * 
 * All strings are stored in UTF-8. LATIN-1 is not currently supported
 * beyond the first 128 characters of ASCII.
 * 
 * ID3v2.3.0 standard: http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: http://id3.org/id3v2.4.0-structure
 * 
 * @todo Add LATIN-1 support beyond ASCII characters.
 * @todo Add write support.
 * @todo Test it on a greater variety of ID3 files, and unit tests.
 * @todo Properly process the TXXX (User-Defined Text) frame with its
 *       description and content.
 * @todo Add support for multiple values.
 * @todo Add support for non-text frames.
 * @todo Add support for comment (COMM) frames.
 * @todo Create a frame factory and multiple Frame children.
 * @todo Support frame flags.
 * @todo Read the ID3v2 Extended Header.
 * @todo Read the ID3v2 Footer.
 */
namespace ID3 {
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////  C O N S T A N T S ////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 contstants.
	 */
	namespace V1 {
		/**
		 * The number of bytes used in ID3v1 tags.
		 */
		extern const int BYTE_SIZE;
		
		/**
		 * The number of bytes used in ID3v1 Extended tags.
		 */
		extern const int EXTENDED_BYTE_SIZE;
		
		/**
		 * A string vector of the 148 defined genres for ID3v1.
		 */
		extern const std::vector<std::string> GENRES;
	}
	
	/**
	 * The number of bytes used in the ID3v2 headers and the ID3v2 frame header.
	 */
	extern const short HEADER_BYTE_SIZE;
	
	/**
	 * The minimum ID3v2 supported major version. Any music file with a
	 * smaller version will not be read.
	 */
	extern const short MIN_SUPPORTED_VERSION;
	
	/**
	 * The maximum ID3v2 supported major version. Any music file with a
	 * greater version will not be read.
	 */
	extern const short MAX_SUPPORTED_VERSION;
	
	/**
	 * The ID3v2 supported minor version. Any music with an ID3v2 version
	 * that does not use this minor version will not be read. This is
	 * also the minor version that will be used for writing.
	 */
	extern const short SUPPORTED_MINOR_VERSION;
	
	/**
	 * The flag values in found in the ID3v2 header.
	 * Check if the flag is used with flag & ID3::V2_FLAG_XXXX == ID3::V2_FLAG_XXXX.
	 */
	extern const short FLAG_UNSYNCHRONISATION;
	extern const short FLAG_EXT_HEADER;
	extern const short FLAG_EXPERIMENTAL;
	extern const short FLAG_FOOTER;
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////  E N U M S ////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 enums.
	 */
	/*namespace V1 {
		/**
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
	 * An enum of text encodings used in ID3v2 frames.
	 */
	enum FrameEncoding {
		LATIN1   = 0, //AKA ISO-8859-1
		UTF16BOM = 1, //AKA UCS-2
		UTF16BE  = 2, //Supposedly ID3v2.4.0+ only
		UTF8     = 3  //Supposedly ID3v2.4.0+ only
	};
	
	/**
	 * An enum containing the highest value for each star rating for the
	 * Popularimeter (POPM) frame in ID3v2.
	 * 
	 * @todo Properly implement this into a POPM Frame class.
	 */
	/*enum POPMRatingCutoff {
		ZERO  = 0,   //0-0
		ONE   = 31,  //1-31
		TWO   = 95,  //32-95
		THREE = 159, //96-159
		FOUR  = 223, //160-223
		FIVE  = 255  //224-255
	};*/
	
	/**
	 * An enum of different frames used in ID3v2. Use
	 * ID3::getFrameName(ID3::Frames) to get the frame name used in the
	 * standard.
	 */
	enum Frames {
		ALBUM=0,   //TALB - The 'Album/Movie/Show title' frame is
		           //intended for the title of the recording(/source of
		           //sound) which the audio in the file is taken from. 
		ARTIST=1,  //TPE1 - The 'Lead artist(s)/Lead performer(s)/Soloist(s)/
		           //Performing group' is used for the main artist(s)
		BAND=2,    //TPE2 - The 'Band/Orchestra/Accompaniment' frame is used for
		           //additional information about the performers in the recording. 
		BPM=3,     //TBPM - The 'BPM' frame contains the number of beats
		           //per minute in the mainpart of the audio. The BPM is
		           //an integer and represented as a numerical string. 
		COMMENT=4, //COMM - This frame is indended for any kind of full
		           //text information that does not fit in any other frame.
		           //It consists of a frame header followed by encoding,
		           //language and content descriptors and is ended with
		           //the actual comment as a text string. Newline characters
		           //are allowed in the comment text string. There may be
		           //more than one comment frame in each tag, but only
		           //one with the same language and content descriptor. 
		COMPOSER=5,//TCOM - The 'Composer(s)' frame is intended for the
		           //name of the composer(s).
		COPYRIGHT=6,//TCOP - The 'Copyright message' frame, which must
		           //begin with a year and a space character (making 5
		           //characters), is intended for the copyright holder
		           //of the original sound, not the audio file itself.
		           //The absence of this frame means only that the
		           //copyright information is unavailable or has been
		           //removed, and must not be interpreted to mean that
		           //the sound is public domain. Every time this field is
		           //displayed the field must be preceded with "Copyright © ".
		DISC=7,    //TPOS - The 'Part of a set' frame is a numeric string
		           //that describes which part of a set the audio came from.
		           //This frame is used if the source described in the
		           //"TALB" frame is divided into several mediums, e.g.
		           //a double CD. The value may be extended with a "/"
		           //character and a numeric string containing the total
		           //number of parts in the set. E.g. "1/2". 
		GENRE=8,   //TCON - The 'Content type'. ID3v1 genres can be
		           //added to the beginning wrapped around parenthesis,
		           //optionally followed by genre text.
		LYRICIST=9,//TEXT - The 'Lyricist(s)/Text writer(s)' frame is
		           //intended for the writer(s) of the text or lyrics in
		           //the recording.
		TITLE=10,  //TIT2 - The 'Title/Songname/Content description'
		           //frame is the actual name of the piece
		           //(e.g. "Adagio", "Hurricane Donna"). 
		TRACK=11,  //TRCK - The 'Track number/Position in set' frame is a
		           //numeric string containing the order number of the
		           //audio-file on its original recording. This may be
		           //extended with a "/" character and a numeric string
		           //containing the total numer of tracks/elements on the
		           //original recording. E.g. "4/9". 
		YEAR=12    //TYER - The 'Year' frame is a numeric string with a
		           //year of the recording. This frames is always four
		           //characters long (until the year 10000). 
	};
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	//////////////////////////  S T R U C T S //////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
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
		uint8_t size[4]; //A synchsafe integer
	};
	
	/**
	 * A 10-bit struct that captures the structure of the ID3v2 extended header.
	 */
	struct ExtHeader {
		uint8_t size[4]; //A synchsafe integer
		uint8_t flags[2];
		uint8_t paddingSize[4];
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
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	//////////////////////////  C L A S S E S //////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	/**
	 * A class that, given a file or filename, will read its ID3 tags.
	 * Call Tag::null() after instantiation to check if the file was
	 * properly read. Files must be .mp3 files.
	 */	
	class Tag {
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
			 * @param process If true, this will process the genre string
			 *                according to the specification of the TCOM
			 *                frame. It will look for a number surrounded
			 *                by parenthesis. If found the substring will
			 *                be removed, and if the no text follows the
			 *                substring, the number will be parsed and
			 *                the corresponding ID3v1 genre will be returned.
			 *                If false, the raw genre string as it
			 *                appears on file will be returned.
			 * @return The genre of the music file, or "" if there is
			 *         no title set.
			 */
			std::string genre(bool process=true) const;
			
			/**
			 * Get the artist tag.
			 * 
			 * @return The artist of the music file, or "" if there is
			 *         no title set.
			 */
			std::string artist() const;
			
			/**
			 * Get the album tag.
			 * 
			 * @return The album of the music file, or "" if there is
			 *         no title set.
			 */
			std::string album() const;
			
			/**
			 * Get the year tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                If false, the raw year string as it
			 *                appears on file will be returned.
			 * @return The year of the music file, or "" if there is
			 *         no year set.
			 */
			std::string year(bool process=true) const;
			
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
			 * Get the total number of tracks of the original recording.
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
			 * Get the total number of discs of the original recording.
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
			 * Get the BPM tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                If false, the raw bpm string as it
			 *                appears on file will be returned.
			 * @return The bpm of the music file, or "" if there is
			 *         no bpm set.
			 */
			std::string bpm(bool process=true) const;
			
			/**
			 * Returns a string representation of the ID3 versions used in the file.
			 * 
			 * @param verbose If true, it will print the size and flags
			 *                of the ID3v2 version (optional).
			 * @return The string representation of the versions used.
			 */
			std::string getVersionString(bool verbose=false) const;
			
			/**
			 * @returns true if the given file is a valid MP3 file, false
			 *          otherwise.
			 */
			const bool null() const;
			
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
				TagInfo();
				short majorVer;
				short minorVer;
				bool flagUnsynchronisation;
				bool flagExtHeader;
				bool flagExperimental;
				bool flagFooter;
				long size;
			};
			
			/**
			 * The filesize of the music file.
			 */
			unsigned long filesize;
			
			/**
			 * A constructor helper method that gets the tag information from the given file.
			 * 
			 * @param file The file object.
			 * @param close Whether to close the file at the end of the method.
			 *        If using the constructor Tag::Tag(std::ifstream&, const std::string&),
			 *        then this will be set to false.
			 */
			void getFile(std::ifstream& file, bool close=true);
			
			/**
			 * A constructor helper method that reads the ID3v1 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void getFileV1(std::ifstream& file);
			
			/**
			 * A constructor helper method that reads the ID3v2 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void getFileV2(std::ifstream& file);
			
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
			 * Helper method to get text content from a frame.
			 * This does not check first to see if the Frame is a text
			 * frame or not.
			 * 
			 * @param frameID The Frame enum member.
			 * @returns The text content, or "" if the frame does not
			 *          exist or is null.
			 */
			std::string getFrameText(Frames frameID) const;
			
		private:
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
			std::unordered_map<std::string, Frame> frames;
	};
}

#endif
