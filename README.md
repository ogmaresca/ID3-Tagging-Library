ID3-Tagging-Library
===================

This is a C++ library to read ID3 tags from MP3 files.

ID3-Tagging-Library has not had extensive testing done, use it at your own risk. This library been developed and tested exclusively on Linux, I give no guarantees that it will work on Windows, OS X, or another OS.

It has been compiled exclusively with g++ and C++14. Add `-std=c++14` to the g++ compilation command to compile with C++14.

##Dependencies
[ICU](http://site.icu-project.org/) - [license](http://source.icu-project.org/repos/icu/icu/trunk/LICENSE).

ID3-Tagging-Library requires the icu-uc package. Add "\`pkg-config icu-uc --cflags --libs\`" to the g++ command when compiling.

##What ID3-Tagging-Library does do
- Read ID3v1, ID3v1.1, ID3v1 Extended, ID3v2.2, ID3v2.3, and ID3v2.4 tags.
- Edit and write ID3v2.4 tags.
- Support 191 ID3v1 and ID3v1.1 genres.
- Support the ID3v2 text, attached picture, play counter, Popularimeter, and event timing codes frames.

##What ID3-Tagging-Library does not do
- Process the ID3v2 extended header.
- Support compressed or encrypted frames.
- Support ID3v2 tags not located at the beginning of the file.
- Support ID3v2 frame grouping identities, aside from preserving its value.
- Support unsynchronisation in ID3v2.3 tags, and writing unsynchronised frames.
- Support editing tags besides the ones listed above.

##License
ID3-Tagging-Library is licensed under the GNU Public License v3 (GPLv3). View `LICENSE.txt` for more information.
