Senjo
=====

Universal Chess Interface (UCI) adapter by Shawn Chidester <zd3nik@gmail.com>.

Just write your chess engine and let Senjo's UCIAdapter deal with the UCI protocol.

Description
-----------

Senjo is a UCI adapter for C++ chess engines.  It handles the interaction between your chess engine and any UCI compliant user interface.  All you have to do is implement a ChessEngine class that does the "thinking" parts, Senjo will deal with the rest.

I started out with the goal of writing a complete chess GUI and this adapter was a component of that project.  But given the plethora of UCI compatible GUIs available I scaled the project down to being nothing more than the reusable UCI adapter.  Senjo (a Japanese word that Google translates to "battlefield") is the name I was going to use for the chess GUI.

The Senjo UCI adapter also comes with a few extra commands that are not part of the UCI specification.  Execute the 'help' command while running your Senjo executable in a text console to get a complete list.  Here are some key examples:

    * help
    * fen
    * print
    * perft
    * test

In particular the *perft* and *test* commands are very handy for testing and tuning.  A few EPD files are included in this repository for use with these commands.  But of course you can use any EPD file(s) you prefer.

How-To
------

To create a chess engine named "Trout" using the Senjo UCI adapter do the following:

    1. Extend the "ChessEngine" class.

        // TroutEngine.h
        #include "ChessEngine.h"

        class TroutEngine : public senjo::ChessEngine
        {
            // implement required ChessEngine methods
            // see ChessEngine.h for documentation
        };

    2. Start a Senjo UCIAdapter instance, feeding it your TroutEngine class.

        // TroutMain.cpp
        #include "UCIAdapter.h"
        #include "Output.h"
        #include "TroutEngine.h"

        int main(int /*argc*/, char** /*argv*/)
        {
            TroutEngine engine;
            senjo::UCIAdapter adapter;

            if (!adapter.Start(engine)) {
                senjo::Output() << "Unable to start UCIAdapter";
                return 1;
            }

    3. Read one line at a time from stdin and feed each line to the adapter.

            // TroutMain.cpp continued...
            char sbuf[16384];
            memset(sbuf, 0, sizeof(sbuf));

            while (fgets(sbuf, sizeof(sbuf), stdin)) {
                char* cmd = sbuf;
                senjo::NormalizeString(cmd);
                if (!adapter.DoCommand(cmd)) {
                    break;
                }
            }

            return 0;
        }


Notes
-----

The example main function given above uses `fgets` to obtain one line of input at a time from stdin.  This is only an example.  You may get input any way you prefer.  All that is required is that you end up with a NULL terminated char* string, you call senjo::NormalizeString on that string, and then you call the senjo::UCIAdapter's DoCommand method and exit the input loop if DoCommand returns false.

The `senjo::Output` class is very useful for debugging.  Use it anywhere; it's thread safe and it prefixes your output with "string info " so it won't confuse UCI compliant user interfaces.  See `Output.h` for more details.

This repository comes with a `Senjo.pro` file, which is a qmake project file.  However, no Qt specific commands, classes, macros, or syntax are used in the source files.  So you shouldn't have any trouble including the source files in any C++ project/makefile-builder of your choice, or simply hand crafting your own Makefile.  And if you do use a qmake project you can include the Senjo sources directly in your own project rather than including Senjo.pro and linking the resulting Senjo library, if that's what you prefer.  Use Senjo in whatever way is most convenient for you.

The `senjo::ChessEngine` base class will start a thread to periodically output search information and stop the current search if the current time exceeds the value of `senjo::ChessEngine::GetStopTime()`.  If you want to handle timeouts in a different way simply set `senjo::ChessEngine::_stopTime` to 0 in your MyGo() method.  To completely disable the built-in ChessEngine timer thread override the `senjo::ChessEngine::UseTimer()` method to return false.

The `senjo::MoveFinder` class, which is used by the *test* command to translate PGN/EPD formatted moves into coordinate notation, is a work in progress.  It works on all the test positions in the EPD files included in the Senjo repository.  But it's not very efficient and there are probably some corner cases it doesn't handle properly.  Feel free to improve upon the MoveFinder class if you're so inclined.

License
-------

Copyright (c) 2015 Shawn Chidester <zd3nik@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
