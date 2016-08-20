ID3-Tagging-Library
===================

This is a C++ library to read ID3 tags from MP3 files.

ID3-Tagging-Library is currently in alpha. I do not recommend using this library for a full product at the moment. Testing is currently minimal, and it is lacking in features.

This library been developed and tested exclusively on Linux, I give no guarantees that it will work on Windows, OS X, or another OS.

It has been compiled exclusively with g++ and C++14. Add `-std=c++14` to the g++ compilation command to compile with C++14.

##Dependencies
[ICU](http://site.icu-project.org/) - [license](http://source.icu-project.org/repos/icu/icu/trunk/LICENSE).

ID3-Tagging-Library requires the icu-uc package. Add "\`pkg-config icu-uc --cflags --libs\`" to the g++ command when compiling.

##What ID3-Tagging-Library does do
- Read ID3v1, ID3v1.1, and ID3v1 Extended tags.
- Support 148 ID3v1 and ID3v1.1 genres.
- Support reading the ID3v2 text frames.

##What ID3-Tagging-Library does not do (yet)
- Extensive testing.
- Give documentation.
- Have support for editing/adding new tags.
- Support non-text frames.
- Support multiple values in text frames.
- Process the ID3v2 extended header or footer.

##License
ID3-Tagging-Library is licensed under the GNU Public License v3 (GPLv3). View `LICENSE.txt` for more information.
