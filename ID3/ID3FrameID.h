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

#ifndef ID3_FRAME_ID_H
#define ID3_FRAME_ID_H

#include <vector>        //For std::vector
#include <unordered_map> //For std::unordered_map and std::pair

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
	 * An enum of different frames used in ID3v2. Use
	 * ID3::getFrameName(ID3::Frames) to get the frame name used in the standard.
	 * 
	 * The enum values are grouped by each frame, and each frame has the same int
	 * value. Each group has at least one value with the name "FRAME_XXXX", where
	 * XXXX is a descriptive title for the frame's function. The last enum value
	 * of the group is "FRAMEID_XXXX", where XXXX is the ID3v2 frame ID. If the
	 * frame is new in ID3v2.4 there will also be a value with the name
	 * "V4FRAMEID_XXXX". If the frame has been deprecated in ID3v2.4, then there
	 * will be "V3FRAMEID_XXXX_SEE_YYYY", where YYYY is the ID3v2.4 frame ID that
	 * the ID3v2.3 frame has been deprecated in favor of, or there will be
	 * "V3FRAMEID_XXXX_DEPRECATED" if there is no replacement frame in ID3v2.4.
	 * The frame groups are sorted by their frame ID values.
	 * 
	 * The enum value "UNKNOWN_V2_2_FRAME" is a a special value used when the
	 * function ID3::convertOldFrameIDToNew(std::string&) does not know the given
	 * ID3v2.2 frame. These frames will be deleted upon a frame write.
	 * 
	 * @see http://id3.org/id3v2.3.0
	 * @see http://id3.org/id3v2.4.0-frames
	 * @todo Add values for each frame ID.
	 */
	enum Frames : ushort {
		FRAME_AUDIO_ENCRYPTION = 0,
		FRAMEID_AENC           = 0,
		
		FRAME_ATTACHED_IMAGE   = 1,
		FRAME_ATTACHED_PICTURE = 1,
		FRAME_IMAGE            = 1,
		FRAME_PICTURE          = 1,
		FRAMEID_APIC           = 1,
		
		FRAME_AUDIO_SEEK_POINT_INDEX = 2,
		FRAME_SEEK_POINT_INDEX       = 2,
		V4FRAMEID_ASPI               = 2,
		FRAMEID_ASPI                 = 2,
		
		FRAME_COMMENT = 3,
		FRAMEID_COMM  = 3,
		
		FRAME_COMMERCIAL = 4,
		FRAMEID_COMR     = 4,
		
		FRAME_ENCRYPTION_METHOD              = 5,
		FRAME_ENCRYPTION_METHOD_REGISTRATION = 5,
		FRAMEID_ENCR                         = 5,
		
		FRAME_EQUALISATION_2 = 6,
		FRAME_EQUALIZATION_2 = 6,
		V4FRAMEID_EQU2       = 6,
		FRAMEID_EQU2         = 6,
		
		FRAME_EQUALISATION      = 7,
		FRAME_EQUALIZATION      = 7,
		V3FRAMEID_EQUA_SEE_EQU2 = 7,
		FRAMEID_EQUA            = 7,
		
		FRAME_EVENT_TIMINGS      = 8,
		FRAME_EVENT_TIMING_CODES = 8,
		FRAME_TIMINGS            = 8,
		FRAMEID_ETCO             = 8,
		
		FRAME_ENCAPSULATED_OBJECT         = 9,
		FRAME_GENERAL_ENCAPSULATED_OBJECT = 9,
		FRAMEID_GEOB                      = 9,
		
		FRAME_GROUP_ID                          = 10,
		FRAME_GROUP_ID_REGISTRATION             = 10,
		FRAME_GROUP_IDENTIFICATION_REGISTRATION = 10,
		FRAMEID_GRID                            = 10,
		
		FRAME_INVOLVED_PEOPLE   = 11,
		V3FRAMEID_IPLS_SEE_TIPL = 11,
		V3FRAMEID_IPLS_SEE_TMCL = 11,
		FRAMEID_IPLS            = 11,
		
		FRAME_LINKED_INFO        = 12,
		FRAME_LINKED_INFORMATION = 12,
		FRAMEID_LINK             = 12,
		
		FRAME_MUSIC_CD_IDENTIFIER = 13,
		FRAMEID_MCDI              = 13,
		
		FRAME_LOCATION_LOOKUO            = 14,
		FRAME_LOCATION_LOOKUP_TABLE      = 14,
		FRAME_MPEG_LOCATION_LOOKUP       = 14,
		FRAME_MPEG_LOCATION_LOOKUP_TABLE = 14,
		FRAMEID_MLLT                     = 14,
		
		FRAME_OWNERSHIP = 15,
		FRAMEID_OWNE    = 15,
		
		FRAME_PLAY_COUNTER = 16,
		FRAMEID_PCNT       = 16,
		
		FRAME_POPULARIMETER = 17,
		FRAMEID_POPM        = 17,
		
		FRAME_POSITION_SYNCHRONISATION = 18,
		FRAME_POSITION_SYNCHRONIZATION = 18,
		FRAMEID_POSS                   = 18,
		
		FRAME_PRIVATE = 19,
		FRAMEID_PRIV  = 19,
		
		FRAME_RECOMMENDED_BUFFER_SIZE = 20,
		FRAMEID_RBUF                  = 20,
		
		FRAME_RELATIVE_VOLUME_ADJUSTMENT_2 = 21,
		V4FRAMEID_RVA2                     = 21,
		FRAMEID_RVA2                       = 21,
		
		FRAME_RELATIVE_VOLUME_ADJUSTMENT = 22,
		V3FRAMEID_RVAD_SEE_RVA2          = 22,
		FRAMEID_RVAD                     = 22,
		
		FRAME_REVERB = 23,
		FRAMEID_RVRB = 23,
		
		FRAME_SEEK     = 24,
		V4FRAMEID_SEEK = 24,
		FRAMEID_SEEK   = 24,
		
		FRAME_SIGNATURE = 25,
		V4FRAMEID_SIGN  = 25,
		FRAMEID_SIGN    = 25,
		
		FRAME_SYNCHED_LYRICS      = 26,
		FRAME_SYNCHED_TEXT        = 26,
		FRAME_SYNCHRONISED_LYRICS = 26,
		FRAME_SYNCHRONIZED_LYRICS = 26,
		FRAME_SYNCHRONISED_TEXT   = 26,
		FRAME_SYNCHRONIZED_TEXT   = 26,
		FRAMEID_SYLT              = 26,
		
		FRAME_SYNCHED_TEMPO_CODES      = 27,
		FRAME_SYNCHRONISED_TEMPO_CODES = 27,
		FRAME_SYNCHRONIZED_TEMPO_CODES = 27,
		FRAMEID_SYTC                   = 27,
		
		FRAME_ALBUM       = 28,
		FRAME_MOVIE_TITLE = 28,
		FRAME_SHOW_TITLE  = 28,
		FRAMEID_TALB      = 28, //ID3::Tag::album()
		
		FRAME_BPM    = 29,
		FRAMEID_TBPM = 29, //ID3::Tag::bpm()
		
		FRAME_COMPOSER = 30,
		FRAMEID_TCOM   = 30, //ID3::Tag::composer()
		
		FRAME_CONTENT_TYPE = 31,
		FRAME_GENRE        = 31,
		FRAMEID_TCON       = 31, //ID3::Tag::genre()
		
		FRAME_COPYRIGHT = 32,
		FRAMEID_TCOP    = 32,
		
		FRAME_DATE                = 33,
		FRAME_RECORDING_TIME_DATE = 33,
		V3FRAMEID_TDAT_SEE_TDRC   = 33,
		FRAMEID_TDAT              = 33,
		
		FRAME_ENCODING_TIME = 34,
		V4FRAMEID_TDEN      = 34,
		FRAMEID_TDEN        = 34,
		
		FRAME_PLAYLIST_DELAY = 35,
		FRAMEID_TDLY         = 35,
		
		FRAME_ORIGINAL_RELEASE_TIME = 36,
		V4FRAMEID_TDOR              = 36,
		FRAMEID_TDOR                = 36,
		
		FRAME_RECORDING_TIME = 37,
		V4FRAMEID_TDRC       = 37,
		FRAMEID_TDRC         = 37,
		
		FRAME_RELEASE_TIME = 38,
		V4FRAMEID_TDRL     = 38,
		FRAMEID_TDRL       = 38,
		
		FRAME_TAGGING_TIME = 39,
		V4FRAMEID_TDTG     = 39,
		FRAMEID_TDTG       = 39,
		
		FRAME_ENCODED_BY = 40,
		FRAMEID_TENC     = 40,
		
		FRAME_LYRICIST    = 41,
		FRAME_TEXT_WRITER = 41,
		FRAMEID_TEXT      = 41,
		
		FRAME_FILETYPE = 42,
		FRAMEID_TFLT   = 42,
		
		FRAME_INVOLVED_PEOPLE_LIST = 43,
		V4FRAMEID_TIPL             = 43,
		FRAMEID_TIPL               = 43,
		
		FRAME_TIME                = 44,
		FRAME_RECORDING_TIME_TIME = 44,
		V3FRAMEID_TIME_SEE_TDRC   = 44,
		FRAMEID_TIME              = 44,
		
		FRAME_CONTENT_GROUP = 45,
		FRAMEID_TIT1        = 45,
		
		FRAME_CONTENT_DESCRIPTION = 46,
		FRAME_NAME                = 46,
		FRAME_TITLE               = 46,
		FRAME_SONG_NAME           = 46,
		FRAMEID_TIT2              = 46, //ID3::Tag::title()
		
		FRAME_DESCRIPTION = 47,
		FRAME_SUBTITLE    = 47,
		FRAMEID_TIT3      = 47,
		
		FRAME_INITIAL_KEY         = 48,
		FRAME_INITIAL_MUSICAL_KEY = 48,
		FRAME_MUSICAL_KEY         = 48,
		FRAMEID_TKEY              = 48,
		
		FRAME_LANGUAGE = 49,
		FRAMEID_TLAN   = 49,
		
		FRAME_LENGTH = 50,
		FRAMEID_TLEN = 50,
		
		FRAME_MUSICIAN_CREDIT_LIST = 51,
		V4FRAMEID_TMCL             = 51,
		FRAMEID_TMCL               = 51,
		
		FRAME_MEDIA_TYPE = 52,
		FRAMEID_TMED     = 52,
		
		FRAME_MOOD     = 53,
		V4FRAMEID_TMOO = 53,
		FRAMEID_TMOO   = 53,
		
		FRAME_ORIGINAL_ALBUM       = 54,
		FRAME_ORIGINAL_MOVIE_TITLE = 54,
		FRAME_ORIGINAL_SHOW_TITLE  = 54,
		FRAME_ORIGINAL_TALB        = 54,
		FRAMEID_TOAL               = 54,
		
		FRAME_ORIGINAL_FILENAME = 55,
		FRAMEID_TOFN            = 55,
		
		FRAME_ORIGINAL_LYRICIST    = 56,
		FRAME_ORIGINAL_TEXT_WRITER = 56,
		FRAME_ORIGINAL_TEXT        = 56,
		FRAMEID_TOLY               = 56,
		
		FRAME_ORIGINAL_ARTIST    = 57,
		FRAME_ORIGINAL_PERFORMER = 57,
		FRAME_ORIGINAL_TPE1      = 57,
		FRAMEID_TOPE             = 57,
		
		FRAME_ORIGINAL_RELEASE_YEAR = 58,
		FRAME_ORIGINAL_YEAR         = 58,
		FRAME_ORIGINAL_TYER         = 58,
		V3FRAMEID_TORY_SEE_TDOR     = 58,
		FRAMEID_TORY                = 58,
		
		FRAME_FILEOWNER = 59,
		FRAME_LICENSEE  = 59,
		FRAMEID_TOWN    = 59,
		
		FRAME_ARTIST           = 60,
		FRAME_LEAD_ARTIST      = 60,
		FRAME_LEAD_PERFORMER   = 60,
		FRAME_PERFORMER        = 60,
		FRAME_PERFORMING_GROUP = 60,
		FRAME_SOLOIST          = 60,
		FRAMEID_TPE1           = 60, //ID3::Tag::artist()
		
		FRAME_ACCOMPANIEMENT = 61,
		FRAME_ALBUM_ARTIST   = 61,
		FRAME_BAND           = 61,
		FRAME_ORCHESTRA      = 61,
		FRAMEID_TPE2         = 61, //ID3::Tag::albumArtist()
		
		FRAME_CONDUCTOR = 62,
		FRAMEID_TPE3    = 62,
		
		FRAME_INTERPRETED_BY = 63,
		FRAME_MODIFIED_BY    = 63,
		FRAME_REMIXED_BY     = 63,
		FRAMEID_TPE4         = 63,
		
		FRAME_DISC     = 64,
		FRAME_SET_PART = 64,
		FRAMEID_TPOS   = 64, //ID3::Tag::disc(bool), ID3::Tag::discTotal(bool)
		
		FRAME_PRODUCED_NOTICE = 65,
		V4FRAMEID_TPRO        = 65,
		FRAMEID_TPRO          = 65,
		
		FRAME_PUBLISHER = 66,
		FRAMEID_TPUB    = 66,
		
		FRAME_SET_POSITION = 67,
		FRAME_TRACK        = 67,
		FRAME_TRACK_NUMBER = 67,
		FRAMEID_TRCK       = 67, //ID3::Tag::track(bool), ID3::Tag::trackTotal(bool)
		
		FRAME_RECORDING_DATES      = 68,
		FRAME_RECORDING_TIME_DATES = 68,
		V3FRAMEID_TRDA_SEE_TDRC    = 68,
		FRAMEID_TRDA               = 68,
		
		FRAME_RADIO_STATION               = 69,
		FRAME_INTERNET_RADIO_STATION      = 69,
		FRAME_INTERNET_RADIO_STATION_NAME = 69,
		FRAMEID_TRSN                      = 69,
		
		FRAME_RADIO_STATION_OWNER          = 70,
		FRAME_INTERNET_RADIO_STATION_OWNER = 70,
		FRAMEID_TRSO                       = 70,
		
		FRAME_ACCOMPANIEMENT_SORT_ORDER = 71,
		FRAME_ALBUM_ARTIST_SORT_ORDER   = 71,
		FRAME_BAND_SORT_ORDER           = 71,
		FRAME_ORCHESTRA_SORT_ORDER      = 71,
		UNOFFICIAL_FRAMEID_TSO2         = 71,
		FRAMEID_TSO2                    = 71,
		
		FRAME_ALBUM_SORT_ORDER       = 72,
		FRAME_MOVIE_TITLE_SORT_ORDER = 72,
		FRAME_SHOW_TITLE_SORT_ORDER  = 72,
		V4FRAMEID_TSOA               = 72,
		FRAMEID_TSOA                 = 72,
		
		FRAME_COMPOSER_SORT_ORDER = 73,
		UNOFFICIAL_FRAMEID_TSOC   = 73,
		FRAMEID_TSOC              = 73,
		
		FRAME_ARTIST_SORT_ORDER    = 74,
		FRAME_PERFORMER_SORT_ORDER = 74,
		FRAME_SOLOIST_SORT_ORDER   = 74,
		V4FRAMEID_TSOP             = 74,
		FRAMEID_TSOP               = 74,
		
		FRAME_CONTENT_DESCRIPTION_SORT_ORDER = 75,
		FRAME_NAME_SORT_ORDER                = 75,
		FRAME_TITLE_SORT_ORDER               = 75,
		FRAME_SONG_NAME_SORT_ORDER           = 75,
		V4FRAMEID_TSOT                       = 75,
		FRAMEID_TSOT                         = 75,
		
		FRAME_SIZE                = 76,
		V3FRAMEID_TSIZ_DEPRECATED = 76,
		FRAMEID_TSIZ              = 76,
		
		FRAME_ISRC   = 77,
		FRAMEID_TSRC = 77,
		
		FRAME_ENCODING_SETTINGS = 78,
		FRAMEID_TSSE            = 78,
		
		FRAME_SET_SUBTITLE = 79,
		V4FRAMEID_TSST     = 79,
		FRAMEID_TSST       = 79,
		
		FRAME_CUSTOM_USER_INFO        = 80,
		FRAME_CUSTOM_USER_INFORMATION = 80,
		FRAME_USER_INFO               = 80,
		FRAMEID_TXXX                  = 80,
		
		FRAME_YEAR                = 81,
		FRAME_RECORDING_TIME_YEAR = 81,
		V3FRAMEID_TYER_SEE_TDRC   = 81,
		FRAMEID_TYER              = 81, //ID3::Tag::year()
		
		FRAME_UNIQUE_FILE_IDENTIFIER = 82,
		FRAMEID_UFID                 = 82,
		
		FRAME_TERMS_OF_USE = 83,
		FRAME_TOU          = 83,
		FRAMEID_USER       = 83,
		
		FRAME_LYRICS                = 84,
		FRAME_TEXT_TRANSCRIPTION    = 84,
		FRAME_TRANSCRIPTION         = 84,
		FRAME_UNSYNCHED_LYRICS      = 84,
		FRAME_UNSYNCHRONISED_LYRICS = 84,
		FRAME_UNSYNCHRONIZED_LYRICS = 84,
		FRAMEID_USLT                = 84,
		
		FRAME_COMMERCIAL_INFO_URL        = 85,
		FRAME_COMMERCIAL_INFORMATION_URL = 85,
		FRAME_URL_COMMERCIAL_INFO        = 85,
		FRAME_URL_COMMERCIAL_INFORMATION = 85,
		FRAMEID_WCOM                     = 85,
		
		FRAME_COPYRIGHT_URL         = 86,
		FRAME_LEGAL_INFO_URL        = 86,
		FRAME_LEGAL_INFORMATION_URL = 86,
		FRAME_URL_COPYRIGHT         = 86,
		FRAME_URL_LEGAL_INFO        = 86,
		FRAME_URL_LEGAL_INFORMATION = 86,
		FRAMEID_WCOP                = 86,
		
		FRAME_OFFICIAL_FILE_URL             = 87,
		FRAME_URL_OFFICIAL_FILE_INFO        = 87, 
		FRAME_URL_OFFICIAL_FILE_INFORMATION = 87,
		FRAMEID_WOAF                        = 87,
		
		FRAME_OFFICIAL_ARTIST_URL    = 88,
		FRAME_OFFICIAL_PERFORMER_URL = 88,
		FRAME_URL_OFFICIAL_ARTIST    = 88,
		FRAME_URL_OFFICIAL_PERFORMER = 88,
		FRAMEID_WOAR                 = 88,
		
		FRAME_OFFICIAL_AUDIO_SOURCE_URL = 89,
		FRAME_URL_OFFICIAL_AUDIO_SOURCE = 89,
		FRAMEID_WOAS                    = 89,
		
		FRAME_OFFICIAL_INTERNET_RADIO_STATION_URL = 90,
		FRAME_URL_OFFICIAL_INTERNET_RADIO_STATION = 90,
		FRAMEID_WORS                              = 90,
		
		FRAME_PAYMENT_URL = 91,
		FRAME_URL_PAYMENT = 91,
		FRAMEID_WPAY      = 91,
		
		FRAME_OFFICIAL_PUBLISHER_URL = 92,
		FRAME_URL_OFFICIAL_PUBLISHER = 92,
		FRAMEID_WPUB                 = 92,
		
		FRAME_USER_DEFINED_URL = 93,
		FRAME_URL_USER_DEFINED = 93,
		FRAMEID_WXXX           = 93,
		
		FRAME_UNKNOWN_FRAME      = 94,
		FRAME_UNKNOWN_V2_2_FRAME = 94,
		FRAMEID_XXXX             = 94
	};
	
	class FrameID {
		public:
			/**
			 * Default constructor. The value defaults to FRAME_UNKNOWN_FRAME.
			 */
			FrameID();
			
			//Allows string literal assignments
			FrameID(const char* frameID);
			FrameID(const std::string& frameID);
			FrameID(const std::string& frameID, const ushort version);
			FrameID(const Frames frameID);
			operator const std::string&() const;
			operator Frames() const;
			bool unknown() const;
			
		private:
			static const std::vector<std::string> FRAME_STR_LIST;
			
			/**
			 * A map that holds frame ID string : Frames enum value pairs that
			 * connects the Frames enum to the actual ID3v2.3+ frame IDs.
			 * 
			 */
			static const std::unordered_map<std::string, Frames> FRAME_STR_ENUM_MAP;
			
			/**
			 * Convert a Frames enum value to the ID3v2 frame name string.
			 * 
			 * @param frameID A Frames enum variable.
			 * @return The ID3v2 frame ID string.
			 */
			static inline std::string getFrameName(const Frames frameID);
			
			/**
			 * Convert a string to its Frames enum value.
			 * 
			 * If the frame ID is unknown, then Frames::FRAME_UNKNOWN_FRAME will be
			 * the value returned.
			 * 
			 * @param frameID A string containing an ID3v2 frame ID.
			 * @return The Frames enum value.
			 */
			static inline Frames getFrameName(const std::string& frameID);
			
			/**
			 * An unordered_map that holds string:FrameID pairs. The key is the
			 * ID3v2.2 frame ID, and the value is its ID3v2.4 equivalent. Note that
			 * for date frames the ID3v2.3 frame IDs are used, as if not since
			 * these frames don't support multiple instances of the frame on file
			 * and only one of these date frames would get saved.
			 */
			static const std::unordered_map<std::string, FrameID> V2_FRAME_CONVERSION_MAP;
			
			/**
			 * Convert an ID3v2.2 or older frame ID to its equivalent ID3v2.4 frame
			 * ID. If the frame ID is unknown, then the "XXXX" ID will be returned.
			 * 
			 * @param v2FrameID The ID3v2.2 frame ID.
			 * @return The equivalent ID3v2.4 frame ID.
			 */
			static inline FrameID convertOldFrameIDToNew(const std::string& v2FrameID);
			
			Frames enumID;
			std::string strID;
	};
}

#endif
