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

#ifndef ID3_CONSTANTS_H
#define ID3_CONSTANTS_H

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
		extern const ushort BYTE_SIZE;
		
		/**
		 * The number of bytes used in ID3v1 Extended tags.
		 */
		extern const ushort EXTENDED_BYTE_SIZE;
		
		/**
		 * A string vector of the 191 defined genres for ID3v1 and unofficial
		 * genres supported by Winamp.
		 */
		extern const std::vector<std::string> GENRES;
	}
	
	/**
	 * The number of bytes used in the ID3v2 headers and the ID3v2 frame header.
	 */
	extern const ushort HEADER_BYTE_SIZE;
	
	/**
	 * The minimum ID3v2 supported major version. Any music file with a
	 * smaller version will not be read.
	 */
	extern const ushort MIN_SUPPORTED_VERSION;
	
	/**
	 * The maximum ID3v2 supported major version. Any music file with a
	 * greater version will not be read.
	 */
	extern const ushort MAX_SUPPORTED_VERSION;
	
	/**
	 * The ID3v2 supported minor version. Any music with an ID3v2 version
	 * that does not use this minor version will not be read. This is
	 * also the minor version that will be used for writing.
	 */
	extern const ushort SUPPORTED_MINOR_VERSION;
	
	/**
	 * The ID3v2 major version that ID3_Tagging-Library writes as.
	 */
	extern const ushort WRITE_VERSION;
	
	/**
	 * The flag values in found in the ID3v2 header.
	 * Check if the flag is used with flag & ID3::V2_FLAG_XXXX == ID3::V2_FLAG_XXXX.
	 */
	extern const uint8_t FLAG_UNSYNCHRONISATION;
	extern const uint8_t FLAG_EXT_HEADER;
	extern const uint8_t FLAG_EXPERIMENTAL;
	extern const uint8_t FLAG_FOOTER;
	
	/**
	 * The maximum size allowed for ID3v2 tags.
	 * The ID3v2 size field is 4 bytes long, and each byte is unsigned
	 * and synchsafe (the most significant bit is always 0).
	 * The value is therefore 2^28 - 1, ~268MB, or ~256MiB.
	 */
	extern const ulong MAX_TAG_SIZE;
}

#endif
