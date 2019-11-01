Senjo
=====

Universal Chess Interface (UCI) adapter by Shawn Chidester <zd3nik@gmail.com>.

Just write your chess engine and let Senjo's UCIAdapter deal with the UCI protocol.  See [Clubfoot](https://github.com/zd3nik/Clubfoot) for an example chess engine that uses Senjo.

Description
-----------

Senjo is a UCI adapter for C++ chess engines.  It handles the interaction between your chess engine and any UCI compliant user interface.  All you have to do is implement a ChessEngine class that does the "thinking" parts, Senjo will deal with the rest.

The Senjo UCI adapter comes with a few extra commands that are not part of the UCI specification.  Here are some examples:

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
        #include "senjo/ChessEngine.h"

        class TroutEngine : public senjo::ChessEngine {
            // implement required ChessEngine methods
            // see ChessEngine.h for documentation
        };

    2. Wrap TroutEngine in a Senjo UCIAdapter and feed it one line of input from stdin at a time.

        // TroutMain.cpp
        #include "TroutEngine.h"
        #include "senjo/UCIAdapter.h"
        #include "senjo/Output.h"

        int main(int /*argc*/, char** /*argv*/) {
            try {
                TroutEngine engine;
                senjo::UCIAdapter adapter(engine);

                std::string line;
                line.reserve(16384);

                while (std::getline(std::coin, line)) {
                    if (!adapter.doCommand(line)) {
                        break;
                    }
                }

                return 0;
            }
            catch (const std::exception& e) {
                senjo::Output() << "ERROR: " << e.what();
                return 1;
            }
        }


Notes
-----

This example uses `std::getline` to obtain one line of input at a time from stdin.  This is only an example.  You may get input any way you prefer.  All that is required is that you assign each line of input to a std::string, pass it to the senjo::UCIAdapter's doCommand() method, and exit the input loop if doCommand() returns false.

The `senjo::Output` class (from Output.h) is very useful for debugging.  Use it anywhere; it's thread safe and it prefixes your output with "string info " so it won't confuse UCI compliant user interfaces.  See `Output.h` for more details.

The senjo source directory contains a `CMakelists.txt` file, which is a cmake project file.  If you're using cmake simply add the senjo directory to your project with `add_subirectory(senjo)`.  If you're not using cmake simply remove he CMakeLists.txt file and include the senjo source files in your project in whatever way is most convenient for you.

License
-------

Copyright (c) 2015-2019 Shawn Chidester <zd3nik@gmail.com>

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
