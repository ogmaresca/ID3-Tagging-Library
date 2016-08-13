ID3-Tagging-Library
===================

This is a C++ library to read ID3 tags from MP3 files.

DO NOT USE THIS. This is currently in very early alpha. Testing has been minimal, and it is lacking in features.
It has also been developed and tested exclusively on Linux. I give no guarantees that it will work at all on Windows, OS X, or another OS.

It has been compiled exclusively with g++ and C++14. Add -std=c++14 to the g++ compilation command to compile with C++14.

##Dependencies
[ICU](http://site.icu-project.org/) - [license](http://source.icu-project.org/repos/icu/icu/trunk/LICENSE).
ID3-Tagging-Library requires the icu-uc package. Add `pkg-config icu-uc --cflags --libs` to the g++ command when compiling.

##What ID3-Tagging-Library does do
- Read ID3v1, ID3v1.1, and ID3v1 Extended tags.
- Support 148 ID3v1 and ID3v1.1 genres.
- Iterate over all ID3v2.3 and ID3v2.4 tags.
- Support reading the ID3 title, genre, artist, album, year, track, disc/cd, composer, and BPM tags.

##What ID3-Tagging-Library does not do (yet)
- Extensive testing.
- Give documentation.
- Support non-MP3 files.
- Support the LATIN-1 encoding beyond the first 128 ASCII characters.
- Have support for editing/adding new tags.
- Support non-text frames.
- Support text frames that are not listed above.
- Support multiple values in text frames.
- Process the ID3v2 extended header, footer, or frame flags.

##License
ID3-Tagging-Library is licensed under the GNU Public License v3 (GPLv3). View `LICENSE.txt` for more information.
