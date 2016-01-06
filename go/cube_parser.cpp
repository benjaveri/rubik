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

/*
 *     U
 * UFL UF UFR UR UBR UB UBL UL
 * FL  F  FR  R  BR  B  BL  L
 * DFL DF DFR DR DBR DB DBL DL
 *     D
 *
 * { 0,1,2 -> face, edge, corner; index }
 */
const int Cube::lutCubieMap[][2] = {
        // U
        { 0, 0, },
        // UFL      UF        UFR       UR        UBR       UB        UBL       UL
        { 2, 0 }, { 1, 0 }, { 2, 1 }, { 1, 1 }, { 2, 2 }, { 1, 2 }, { 2, 3 }, { 1, 3 },
        // FL       F         FR        R         BR        B         BL        L
        { 1, 4 }, { 0, 1 }, { 1, 5 }, { 0, 2 }, { 1, 6 }, { 0, 3 }, { 1, 7 }, { 0, 4 },
        // DFL      DF        DFR       DR        DBR       DB         DBL       DL
        { 2, 4 }, { 1, 8 }, { 2, 5 }, { 1, 9 }, { 2, 6 }, { 1, 10 }, { 2, 7 }, { 1, 11 },
        // D
        { 0, 5 }
};

/*
 * UUUUUUUUU FFFFFFFFF RRRRRRRRR BBBBBBBBB LLLLLLLLL DDDDDDDDD
 *
 * { 0,1,2 -> face, edge, corner; index; bit shift }
 */
const int Cube::lutFaceMap[][3] = {
        // U
        { 2, 3, 8 }, { 1, 2, 4 }, { 2, 2, 8 },
        { 1, 3, 4 }, { 0, 0, 0 }, { 1, 1, 4 },
        { 2, 0 ,8 }, { 1, 0, 4 }, { 2, 1, 8 },
        // F
        { 2, 0, 4 }, { 1, 0, 0 }, { 2, 1, 4 },
        { 1, 4, 4 }, { 0, 1, 0 }, { 1, 5, 4 },
        { 2, 4, 4 }, { 1, 8, 0 }, { 2, 5, 4 },
        // R
        { 2, 1, 0 }, { 1, 1, 0 }, { 2, 2, 0 },
        { 1, 5, 0 }, { 0, 2, 0 }, { 1, 6, 0 },
        { 2, 5, 0 }, { 1, 9, 0 }, { 2, 6, 0 },
        // B
        { 2, 2, 4 }, { 1, 2, 0 }, { 2, 3, 4 },
        { 1, 6, 4 }, { 0, 3, 0 }, { 1, 7, 4 },
        { 2, 6, 4 }, { 1, 10, 0 }, { 2, 7, 4 },
        // L
        { 2, 3, 0 }, { 1, 3, 0 }, { 2, 0, 0 },
        { 1, 7, 0 }, { 0, 4, 0 }, { 1, 4, 0 },
        { 2, 7, 0 }, { 1, 11, 0 }, { 2, 4, 0 },
        // D
        { 2, 4, 8 }, { 1, 8, 4 }, { 2, 5, 8 },
        { 1, 11, 4 }, { 0, 5, 0 }, { 1, 9, 4 },
        { 2, 7, 8 }, { 1, 10, 4 }, { 2, 6, 8 },
};

/*
 * validate the face & cubie programs are good
 */
//bool Cube::testParserDefs() {
//    Cube cube;
//    Key key;
//
//    // map only one color but expect it to occupy all bits
//    key['x'] = 15;
//
//    // validate that there is a unique mapping for cubies
//    memset(&cube,0,sizeof(cube));
//    assert(-1 == cube.parseCubies("x xxx xx xxx xx xxx xx xxx xx xx x xx x xx x xx x xxx xx xxx xx xxx xx xxx xx x",key));
//    for (int i = 0; i < 6; i++) assert(cube.face[i] == 15);
//    for (int i = 0; i < 12; i++) assert(cube.edge[i] == 255);
//    for (int i = 0; i < 8; i++) assert(cube.corner[i] == 4095);
//
//    // validate that there is a unique mapping for faces
//    memset(&cube,0,sizeof(cube));
//    assert(-1 == cube.parseFaces("xxxxxxxxx xxxxxxxxx xxxxxxxxx xxxxxxxxx xxxxxxxxx xxxxxxxxx",key));
//    for (int i = 0; i < 6; i++) assert(cube.face[i] == 15);
//    for (int i = 0; i < 12; i++) assert(cube.edge[i] == 255);
//    for (int i = 0; i < 8; i++) assert(cube.corner[i] == 4095);
//
//    return true;
//}


/*
 * parse 26 cubies or 6 faces and determine a consistent key for use when multiple cubes needs to be
 *  parsed with compatible mappings
 */
bool Cube::parseKey(const string& def,Key& key) {
    // split into separate cubies
    istringstream iss(def);
    vector<string> term{istream_iterator<string>{iss},istream_iterator<string>{}};

    // enumerate all colors
    vector<char> wc;
    for (auto cb = term.begin(); cb != term.end(); cb++) {
        for (auto ch = cb->begin(); ch != cb->end(); ch++) {
            wc.push_back(*ch);
        }
    }

    // sort colors for consistency when parsing other cubes
    sort(wc.begin(),wc.end());

    // add to key
    for (auto ch = wc.begin(); ch != wc.end(); ch++) {
        auto it = key.find(*ch);
        if (it == key.end()) {
            key[*ch] = (byte)key.size();
        }
    }

    // done
    return key.size() < 16;
}

/*
 * parse 26 cubies into cube state using & extending current key
 *  string should be whitespace separated
 *  key extends on first come basis - so for consistency use the
 *   the parseKey method that tries to make a decent map upfront
 */
int Cube::parseCubies(const string& def,Key& key) {
    // split into separate cubies
    istringstream iss(def);
    vector<string> cubie{istream_iterator<string>{iss},istream_iterator<string>{}};
    if (cubie.size() != 26) return (int)cubie.size();

    // parse and insert
    for (int i = 0; i < 26; i++) {
        // parse cubie
        string& c = cubie[i];
        unsigned acc = 0;
        for (auto ch = c.begin(); ch != c.end(); ch++) {
            auto it = key.find(*ch);
            assert (it != key.end());
            acc = (acc << 4) | it->second;
        }

        // place in state structure
        const int *pgm = lutCubieMap[i];
        switch (pgm[0]) {
            case 0: {
                if (c.length() != 1) return i;
                face[pgm[1]] = (byte)acc;
                break;
            }
            case 1: {
                if (c.length() != 2) return i;
                edge[pgm[1]] = (byte)acc;
                break;
            }
            case 2: {
                if (c.length() != 3) return i;
                corner[pgm[1]] = (word)acc;
                break;
            }
            default: return i;
        }
    }

    // success
    return -1;
}

/*
 * parse six 3x3x faces into cube state using & extending current key
 *  string should be whitespace separated
 *  key extends on first come basis - so for consistency use the
 *   the parseKey method that tries to make a decent map upfront
 */
int Cube::parseFaces(const string& def,Key& key) {
    // split into separate cubies
    istringstream iss(def);
    vector<string> fd{istream_iterator<string>{iss},istream_iterator<string>{}};
    if (fd.size() != 6) return (int) fd.size();

    // parse and insert
    memset(this,0,sizeof(*this)); // zero out state since we place keys with logical or
    for (int f = 0; f < 6; f++ ) {
        for (int i = 0; i < 3*3; i++) {
            // parse face
            const char ch = fd[f][i];
            auto it = key.find(ch);
            assert (it != key.end());

            // place in state structure
            const int *pgm = lutFaceMap[f*9+i];
            switch (pgm[0]) {
                case 0: {
                    face[pgm[1]] |= (byte)(it->second << pgm[2]);
                    break;
                }
                case 1: {
                    edge[pgm[1]] |= (byte)(it->second << pgm[2]);
                    break;
                }
                case 2: {
                    corner[pgm[1]] |= (word)(it->second << pgm[2]);
                    break;
                }
                default:
                    return f*9+i;
            }
        }
    }

    // success
    return -1;
}

/*
 * create a string representing cube state
 */
string Cube::constructCubies(Key& key) const {
    map<byte,char> rkey;
    for (auto it = key.begin(); it != key.end(); it++) rkey[it->second] = it->first;

    string s;
    for (int i = 0; i < 26; i++) {
        if (i) s.push_back(' ');
        int ctype = lutCubieMap[i][0];
        int index = lutCubieMap[i][1];
        switch (ctype) {
            case 0: {
                s.push_back(rkey.find(face[index])->second);
                break;
            }
            case 1: {
                s.push_back(rkey.find((edge[index] >> 4) & 15)->second);
                s.push_back(rkey.find((edge[index] >> 0) & 15)->second);
                break;
            }
            case 2: {
                s.push_back(rkey.find((corner[index] >> 8) & 15)->second);
                s.push_back(rkey.find((corner[index] >> 4) & 15)->second);
                s.push_back(rkey.find((corner[index] >> 0) & 15)->second);
                break;
            }
        }
    }
    return s;
}

string Cube::constructFaces(Key& key) const {
    map<byte,char> rkey;
    for (auto it = key.begin(); it != key.end(); it++) rkey[it->second] = it->first;

    string s;
    for (int f = 0; f < 6; f++) {
        if (f) s.push_back(' ');
        for (int i = 0; i < 3 * 3; i++) {
            const int *pgm = lutFaceMap[f * 9 + i];
            int ctype = pgm[0];
            int index = pgm[1];
            int shift = pgm[2];
            switch (ctype) {
                case 0: {
                    s.push_back(rkey.find((face[index] >> shift) & 15)->second);
                    break;
                }
                case 1: {
                    s.push_back(rkey.find((edge[index] >> shift) & 15)->second);
                    break;
                }
                case 2: {
                    s.push_back(rkey.find((corner[index] >> shift) & 15)->second);
                    break;
                }
            }
        }
    }
    return s;
}
