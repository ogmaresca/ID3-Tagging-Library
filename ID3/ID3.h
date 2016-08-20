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
#include <functional>

#include "ID3Frame.h"

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
 * ID3v2.3.0 standard: @link http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: @link http://id3.org/id3v2.4.0-structure
 * 
 * @todo Add write support.
 * @todo Test it on a greater variety of ID3 files, and unit tests.
 * @todo Add support for saving multiple Frames on the same file.
 * @todo Add support for multiple values.
 * @todo Add support for non-text frames.
 * @todo Read the ID3v2 Extended Header.
 * @todo Read the ID3v2 Footer.
 */
namespace ID3 {
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////// T Y P E D E F S //////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	typedef std::vector<char> ByteArray;
	typedef std::shared_ptr<Frame> FramePtr;
	typedef std::unordered_map<std::string, FramePtr> FrameMap;
	typedef std::pair<std::string, FramePtr> FramePair;
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////// C O N S T A N T S /////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
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
	
	/**
	 * The maximum size allowed for ID3v2 tags.
	 * The ID3v2 size field is 4 bytes long, and each byte is unsigned
	 * and synchsafe (the most significant bit is always 0).
	 * The value is therefore 2^28 - 1, ~268MB, or ~256MiB.
	 */
	extern const long MAX_TAG_SIZE;
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// E N U M S /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
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
		ENCODING_LATIN1   = 0, //AKA ISO-8859-1
		ENCODING_UTF16BOM = 1, //AKA UCS-2
		ENCODING_UTF16    = 2, //ID3v2.4+ only, ID3-Tagging-Library will read in ID3v2.3
		ENCODING_UTF8     = 3  //ID3v2.4+ only, ID3-Tagging-Library will read in ID3v2.3
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
	 * 
	 * @see http://id3.org/id3v2.3.0
	 * @see http://id3.org/id3v2.4.0-frames
	 * @todo Add values for each frame ID.
	 */
	enum Frames {
		ALBUM=0,//TALB - The 'Album/Movie/Show title' frame is intended for the
		        //title of the recording(/source of sound) which the audio in the
		        //file is taken from. 
		        //ID3::Tag::album()
		ALBUMARTIST=1,//TPE2 - The 'Band/Orchestra/Accompaniment' frame is used
		        //for additional information about the performers in the recording.
		        //ID3::Tag::albumArtist()
		ARTIST=2,//TPE1 - The 'Lead artist(s)/Lead performer(s)/Soloist(s)/
		        //Performing group' is used for the main artist(s)
		        //ID3::Tag::artist()
		BPM=3,  //TBPM - The 'BPM' frame contains the number of beats per minute
		        //in the main part of the audio. The BPM is an integer and
		        //represented as a numerical string.
		        //ID3::Tag::bpm()
		COMMENT=4,//COMM - This frame is indended for any kind of full text
		        //information that does not fit in any other frame. It consists of
		        //a frame header followed by encoding, language and content
		        //descriptors and is ended with the actual comment as a text
		        //string. Newline characters are allowed in the comment text
		        //string. There may be more than one comment frame in each tag,
		        //but only one with the same language and content descriptor.
		        ///@todo ID3::Tag::comment()
		COMPOSER=5,//TCOM - The 'Composer(s)' frame is intended for the name of the composer(s).
		        //ID3::Tag::composer()
		CONDUCTOR=14,//TPE3 - The 'Conductor' frame is used for the name of the conductor.
		        ///@todo ID3::Tag::conductor()
		COPYRIGHT=6,//TCOP - The 'Copyright message' frame, which must begin with
		        //a year and a space character (making 5 characters), is intended
		        //for the copyright holder of the original sound, not the audio
		        //file itself. The absence of this frame means only that the
		        //copyright information is unavailable or has been removed, and
		        //must not be interpreted to mean that the sound is public domain.
		        //Every time this field is displayed the field must be preceded with "Copyright © ".
		        ///@todo ID3::Tag::copyright()
		DATE=15,//TDAT - The 'Date' frame is a numeric string in the DDMM format
		        //containing the date for the recording. This field is always four characters long.
		        ///@todo ID3::Tag::date(bool)
		DESCRIPTION=21,//TIT3 - The 'Subtitle/Description refinement' frame is
		        //used for information directly related to the contents title
		        //(e.g. "Op. 16" or "Performed live at Wembley").
		        ///@todo ID3::Tag::description()
		DISC=7, //TPOS - The 'Part of a set' frame is a numeric string that
		        //describes which part of a set the audio came from. This frame is
		        //used if the source described in the "TALB" frame is divided into
		        //several mediums, e.g. a double CD. The value may be extended with
		        //a "/" character and a numeric string containing the total number
		        //of parts in the set. E.g. "1/2".
		        //ID3::Tag::disc(bool) ID3::Tag::discTotal(bool)
		ENCODEDBY=16,//TENC - The 'Encoded by' frame contains the name of the
		        //person or organisation that encoded the audio file. This field
		        //may contain a copyright message, if the audio file also is copyrighted by the encoder.
		        ///@todo ID3::Tag::encodedBy()
		ENCODINGSETTINGS=34,//TSSE - The 'Software/Hardware and settings used for
		        //encoding' frame includes the used audio encoder and its settings
		        //when the file was encoded. Hardware refers to hardware encoders,
		        //not the computer on which a program was run.
		        ///@todo ID3::Tag::encodingSettings()
		FILEOWNER=30,//TOWN - The 'File owner/licensee' frame contains the name of
		        //the owner or licensee of the file and it's contents.
		        ///@todo ID3::Tag::fileOwner()
		FILETYPE=18,//TFLT - The 'File type' frame indicates which type of audio
		        //this tag defines. The following type and refinements are defined:
		        //    MPG       MPEG Audio
		        //    MPG/1     MPEG 1/2 layer I
		        //    MPG/2     MPEG 1/2 layer II
		        //    MPG/2.5   MPEG 2.5
		        //    MPG/3     MPEG 1/2 layer III
		        //    MPG/AAC   Advanced Audio Compression
		        //    VQF       Transform-domain Weighted Interleave Vector Quantization
		        //    PCM       Pulse Code Modulated audio
		        //but other types may be used, not for these types though. This is
		        //used in a similar way to the predefined types in the "TMED" frame,
		        //but without parentheses. If this frame is not present audio type is assumed to be "MPG".
		        ///@todo ID3::Tag::fileType(bool)
		GENRE=8,//TCON - The 'Content type'. ID3v1 genres can be added to the
		        //beginning wrapped around parenthesis, optionally followed by genre text.
		        //ID3::Tag::genre(bool)
		GROUPING=20,//TIT1 - The 'Content group description' frame is used if the
		        //sound belongs to a larger category of sounds/music. For example,
		        //classical music is often sorted in different musical sections
		        //(e.g. "Piano Concerto", "Weather - Hurricane").
		        ///@todo ID3::Tag::grouping()
		INVOLVEDPEOPLE=38,//IPLS - Since there might be a lot of people
		        //contributing to an audio file in various ways, such as musicians
		        //and technicians, the 'Text information frames' are often
		        //insufficient to list everyone involved in a project. The
		        //'Involved people list' is a frame containing the names of those
		        //involved, and how they were involved. The body simply contains a
		        //terminated string with the involvement directly followed by a
		        //terminated string with the involvee followed by a new involvement
		        //and so on. There may only be one "IPLS" frame in each tag.
		ISRC=35,//TSRC - The 'ISRC' frame should contain the International
		        //Standard Recording Code (ISRC) (12 characters).
		        ///@todo ID3::Tag::isrc()
		LENGTH=23,//TLEN - The 'Length' frame contains the length of the audiofile
		        //in milliseconds, represented as a numeric string.
		        ///@todo ID3::Tag::length(bool)
		LYRICIST=9,//TEXT - The 'Lyricist(s)/Text writer(s)' frame is intended for
		        //the writer(s) of the text or lyrics in the recording.
		        ///@todo ID3::Tag::lyricist()
		MEDIATYPE=24,//TMED - The 'Media type' frame describes from which media
		        //the sound originated. This may be a text string or a reference
		        //to the predefined media types found in the list below. References
		        //are made within "(" and ")" and are optionally followed by a text
		        //refinement, e.g. "(MC) with four channels". If a text refinement
		        //should begin with a "(" character it should be replaced with "(("
		        //in the same way as in the "TCO" frame. Predefined refinements is
		        //appended after the media type, e.g. "(CD/A)" or "(VID/PAL/VHS)". 
		        //See http://id3.org/id3v2.3.0 for the different predefined types.
		        ///@todo ID3::Tag::mediaType()
		MODIFIEDBY=10,//TPE4 - The 'Interpreted, remixed, or otherwise modified by
		        //frame contains more information about the people behind a remix
		        //and similar interpretations of another existing piece.
		        ///@todo ID3::Tag::modifiedBy()
		MUSICALKEY=22,//TKEY - The 'Initial key' frame contains the musical key in
		        //which the sound starts. It is represented as a string with a
		        //maximum length of three characters. The ground keys are
		        //represented with "A","B","C","D","E", "F" and "G" and halfkeys
		        //represented with "b" and "#". Minor is represented as "m".
		        //Example "Cbm". Off key is represented with an "o" only.
		        ///@todo ID3::Tag::musicalKey(bool)
		ORIGINALALBUM=25,//TOAL - The 'Original album/movie/show title' frame is
		        //intended for the title of the original recording (or source of
		        //sound), if for example the music in the file should be a cover
		        //of a previously released song.
		        ///@todo ID3::Tag::originalAlbum()
		ORIGINALARTIST=26,//TOPE - The 'Original artist(s)/performer(s)' frame is
		        //intended for the performer(s) of the original recording, if for
		        //example the music in the file should be a cover of a previously
		        //released song. The performers are seperated with the "/" character.
		        ///@todo ID3::Tag::originalArtist()
		ORIGINALFILENAME=27,//TOFN - The 'Original filename' frame contains the
		        //preferred filename for the file, since some media doesn't allow
		        //the desired length of the filename. The filename is case
		        //sensitive and includes its suffix.
		        ///@todo ID3::Tag::originalFilename()
		ORIGINALLYRICIST=28,//TOLY - The 'Original lyricist(s)/text writer(s)'
		        //frame is intended for the text writer(s) of the original
		        //recording, if for example the music in the file should be a
		        //cover of a previously released song. The text writers are
		        //seperated with the "/" character.
		        ///@todo ID3::Tag::originalLyricist()
		ORIGINALYEAR=29,//TORY - The 'Original release year' frame is intended for
		        //the year when the original recording, if for example the music
		        //in the file should be a cover of a previously released song, was
		        //released. The field is formatted as in the "TYER" frame.
		        ///@todo ID3::Tag::originalYear()
		PLAYLISTDELAY=17,//TDLY - The 'Playlist delay' defines the numbers of
		        //milliseconds of silence between every song in a playlist. The
		        //player should use the "ETC0" frame, if present, to skip initial
		        //silence and silence at the end of the audio to match the
		        //'Playlist delay' time. The time is represented as a numeric string.
		        ///@todo ID3::Tag::playlistDelay()
		RADIOSTATION=30,//TRSN - The 'Internet radio station name' frame contains
		        //the name of the internet radio station from which the audio is streamed.
		        ///@todo ID3::Tag::radioStation()
		RADIOSTATIONOWNER=31,//TRSO - The 'Internet radio station owner' frame
		        //contains the name of the owner of the internet radio station
		        //from which the audio is streamed.
		        ///@todo ID3::Tag::radioStationOwner()
		RECORDINGDATES=32,//TRDA - The 'Recording dates' frame is a intended to be
		        //used as complement to the "TYER", "TDAT" and "TIME" frames. E.g.
		        //"4th-7th June, 12th June" in combination with the "TYER" frame.
		        ///@todo ID3::Tag::recordingDates()
		SIZE=36,//TSIZ - The 'Size' frame contains the size of the audiofile in
		        //bytes, excluding the ID3v2 tag, represented as a numeric string.
		        ///@todo ID3::Tag::size()
		TIME=19,//TIME - The 'Time' frame is a numeric string in the HHMM format
		        //containing the time for the recording. This field is always four characters long.
		        ///@todo ID3::Tag::time(bool)
		TITLE=11,//TIT2 - The 'Title/Songname/Content description' frame is the
		        //actual name of the piece (e.g. "Adagio", "Hurricane Donna").
		        //ID3::Tag::title()
		TRACK=12,//TRCK - The 'Track number/Position in set' frame is a numeric
		        //string containing the order number of the audio-file on its
		        //original recording. This may be extended with a "/" character
		        //and a numeric string containing the total numer of
		        //tracks/elements on the original recording. E.g. "4/9".
		        //ID3::Tag::track(bool) ID3::Tag::trackTotal(bool)
		USERINFO=37,//TXXX - This frame is intended for one-string text
		        //information concerning the audiofile in a similar way to the
		        //other "T"-frames. The frame body consists of a description of
		        //the string, represented as a terminated string, followed by the
		        //actual string. There may be more than one "TXXX" frame in each
		        //tag, but only one with the same description.
		        ///@todo ID3::Tag::userInfo()
		YEAR=13 //TYER - The 'Year' frame is a numeric string with a year of the
		        //recording. This frames is always four characters long (until the year 10000). 
		        //ID3::Tag::year()
	};
	
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
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// C L A S S E S ///////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
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
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			//////////////////////// S T A R T   F R A M E ////////////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Get the text content of a frame.
			 * 
			 * NOTE: Not all frames support text content. If the given
			 *       frame name does not, or the music file does not
			 *       contain an ID3v2 frame of that type, an empty
			 *       string will be returned.
			 * 
			 * NOTE: No formatting will be done, so if the frame has special
			 *       formatting then you should call the relevant method instead.
			 * 
			 * @param frameName A Frames enum variable that represents
			 *                  an ID3v2 frame ID.
			 * @return The text content, or "" if the frame is not found,
			 *         "null", or not a text frame.
			 */
			std::string textContent(Frames frameName) const;
			
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
			 * Get the album artist/band/orchestra/accompaniment tag.
			 * 
			 * @return The Album artist of the music file, or "" if
			 *         there is no tag set.
			 */
			std::string albumArtist() const;
			
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
			 * Get the BPM tag.
			 * 
			 * @return The bpm of the music file, or "" if there is
			 *         no bpm set.
			 */
			std::string bpm() const;
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			////////////////////////// E N D   F R A M E //////////////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
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
				unsigned long size;
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
			void readFile(std::ifstream& file, bool close=true);
			
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
			FrameMap frames;
	};
}

#endif
