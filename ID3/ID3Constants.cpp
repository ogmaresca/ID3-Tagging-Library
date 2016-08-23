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

#include "ID3Constants.h"
#include "ID3.h"

using namespace ID3;

///@pkg ID3.h
const ushort ID3::V1::BYTE_SIZE = sizeof(V1::Tag);

///@pkg ID3.h
const ushort ID3::V1::EXTENDED_BYTE_SIZE = sizeof(V1::ExtendedTag);

///@pkg ID3.h
const ushort ID3::HEADER_BYTE_SIZE = sizeof(Header);

///@pkg ID3.h
const ushort ID3::MIN_SUPPORTED_VERSION = 3;

///@pkg ID3.h
const ushort ID3::MAX_SUPPORTED_VERSION = 4;

///@pkg ID3.h
const ushort ID3::SUPPORTED_MINOR_VERSION = 0;

///@pkg ID3.h
const uint8_t ID3::FLAG_UNSYNCHRONISATION = 0b10000000;
const uint8_t ID3::FLAG_EXT_HEADER =        0b01000000;
const uint8_t ID3::FLAG_EXPERIMENTAL =      0b00100000;
const uint8_t ID3::FLAG_FOOTER =            0b00010000;

///@pkg ID3.h
const ulong ID3::MAX_TAG_SIZE = (1UL << 28) - 1;

///@pkg ID3.h
const std::vector<std::string> ID3::V1::GENRES = {
	"Blues",               //0
	"Classic Rock",        //1
	"Country",             //2
	"Dance",               //3
	"Disco",               //4
	"Funk",                //5
	"Grunge",              //6
	"Hip-Hop",             //7
	"Jazz",                //8
	"Metal",               //9
	"New Age",             //10
	"Oldies",              //11
	"Other",               //12
	"Pop",                 //13
	"R&B",                 //14
	"Rap",                 //15
	"Reggae",              //16
	"Rock",                //17
	"Techno",              //18
	"Industrial",          //19
	"Alternative",         //20
	"Ska",                 //21
	"Death Metal",         //22
	"Pranks",              //23
	"Soundtrack",          //24
	"Euro-Techno",
	"Ambient",
	"Trip-Hop",
	"Vocal",
	"Jazz+Funk",
	"Fusion",
	"Trance",
	"Classical",
	"Instrumental",
	"Acid",
	"House",
	"Game",
	"Sound Clip",
	"Gospel",
	"Noise",
	"AlternRock",
	"Bass",
	"Soul",
	"Punk",
	"Space",
	"Meditative",
	"Instrumental Pop",
	"Instrumental Rock",
	"Ethnic",
	"Gothic",
	"Darkwave",
	"Techno-Industrial",
	"Electronic",
	"Pop-Folk",
	"Eurodance",
	"Dream",
	"Southern Rock",
	"Comedy",
	"Cult",
	"Gangsta",
	"Top 40",
	"Christian Rap",
	"Pop/Funk",
	"Jungle",
	"Native American",
	"Cabaret",
	"New Wave",
	"Psychadelic",
	"Rave",
	"Showtunes",
	"Trailer",
	"Lo-Fi",
	"Tribal",
	"Acid Punk",
	"Acid Jazz",
	"Polka",
	"Retro",
	"Musical",
	"Rock & Roll",
	"Hard Rock",
	"Folk",
	"Folk-Rock",
	"National Folk",
	"Swing",
	"Fast Fusion",
	"Bebob",
	"Latin",
	"Revival",
	"Celtic",
	"Bluegrass",
	"Avantgarde",
	"Gothic Rock",
	"Progressive Rock",
	"Psychedelic Rock",
	"Symphonic Rock",
	"Slow Rock",
	"Big Band",
	"Chorus",
	"Easy Listening",
	"Acoustic",
	"Humour",
	"Speech",
	"Chanson",
	"Opera",
	"Chamber Music",
	"Sonata",
	"Symphony",
	"Booty Bass",
	"Primus",
	"Porn Groove",
	"Satire",
	"Slow Jam",
	"Club",
	"Tango",
	"Samba",
	"Folklore",
	"Ballad",
	"Power Ballad",
	"Rhytmic Soul",
	"Freestyle",
	"Duet",
	"Punk Rock",
	"Drum Solo",
	"Acapella",
	"Euro-House",
	"Dance Hall",
	"Goa",
	"Drum & Bass",
	"Club-House",
	"Hardcore",
	"Terror",
	"Indie",
	"BritPop",
	"Negerpunk",
	"Polsk Punk",
	"Beat",
	"Christian Gangsta",
	"Heavy Metal",
	"Black Metal",
	"Crossover",
	"Contemporary C",
	"Christian Rock",
	"Merengue",
	"Salsa",
	"Thrash Metal",
	"Anime",
	"JPop",
	"SynthPop",
	"Abstract",
	"Art Rock",
	"Baroque",
	"Bhangra",
	"Big Beat",
	"Breakbeat",
	"Chillout",
	"Downtempo",
	"Dub",
	"EBM",
	"Eclectic",
	"Electro",
	"Electroclash",
	"Emo",
	"Experimental",
	"Garage",
	"Global",
	"IDM",
	"Illbient",
	"Industro-Goth",
	"Jam Band",
	"Krautrock",
	"Leftfield",
	"Lounge",
	"Math Rock",
	"New Romantic",
	"Nu-Breakz",
	"Post-Punk",
	"Post-Rock",
	"Psytrance",
	"Shoegaze",
	"Space Rock",
	"Trop Rock",
	"World Music",
	"Neoclassical",
	"Audiobook",
	"Audio Theatre",
	"Neue Deutsche Welle", //
	"Podcast",
	"Indie Rock",
	"G-Funk",
	"Dubstep",
	"Garage Rock",
	"Psybient"
};
