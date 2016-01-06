/*
    Copyright (c) 2016, Ben de Waal
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * The names of the contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "cube.h"
#include "command.h"

int main(int argc,const char *argv[]) {
    // init
    Cube::generateTables();

    // for all text files...
    for (int c = 1; c < argc; c++) {
        ifstream f(argv[c]);
        if (!f.is_open()) {
            cout << "Unable to open " << argv[c] << "\n";
            return 1;
        }

        // parse input, one line at a time
        for (string line; getline(f,line);) {
            // echo
            cout << "> " << line << "\n";

            // break into terms on whitespace boundaries
            istringstream iss(line);
            vector<string> term{istream_iterator<string>{iss},istream_iterator<string>{}};

            // interpret
            for (int i = 0; ; i++) {
                switch (command[i]->operator()(term)) {
                    case Status::NoMatch:
                        continue;
                    case Status::Failed:
                        f.close();
                        return 1;
                    case Status::Success:
                        break;
                }
                break;
            }
        }

        // done
        f.close();
    }
    return 0;
}