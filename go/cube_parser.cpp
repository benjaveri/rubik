#include "cube.h"

/*
    U
    UFL UF UFR UR UBR UB UBL UL
    FL  F  FR  R  BR  B  BL  L
    DFL DF DFR DR DBR DB DBL DL
    D
*/
const int Cube::cubeDefPgm[][2] = {
        { 0, 0, },
        { 2, 0 }, { 1, 0 }, { 2, 1 }, { 1, 1 }, { 2, 2 }, { 1, 2 }, { 2, 3 }, { 1, 3 },
        { 1, 4 }, { 0, 1 }, { 1, 5 }, { 0, 2 }, { 1, 6 }, { 0, 3 }, { 1, 7 }, { 0, 4 },
        { 2, 4 }, { 1, 8 }, { 2, 5 }, { 1, 9 }, { 2, 6 }, { 1, 10 }, { 2, 7 }, { 1, 11 },
        { 0, 5 }
};

/*
 * parse 26 cubies into cube state using & extending current key
 *  string should be whitespace separated
 *  key extends on first come basis - so for consistency use the
 *   the parseKey method that tries to make a decent map upfront
 */
int Cube::parseCubies(const string& def,map<char,byte>& key) {
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
            if (it == key.end()) {
                key[c.front()] = (byte) key.size();
                it = key.find(*ch);
            }
            acc = (acc << 4) | it->second;
        }

        // place in state structure
        const int *pgm = cubeDefPgm[i];
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
 * parse 26 cubies and determine a consistent key for use when multiple cubes needs to be
 *  parsed with compatible mappings
 */
bool Cube::parseKey(const string& def,map<char,byte>& key) {
    // split into separate cubies
    istringstream iss(def);
    vector<string> cubie{istream_iterator<string>{iss},istream_iterator<string>{}};
    if (cubie.size() != 26) return false;

    // assign faces
    int fi[] = {0,10,12,14,16,25};
    for (int i = 0; i < 6; i++) {
        auto ch = cubie[fi[i]].front();
        auto it = key.find(ch);
        if (it == key.end()) key[ch] = (byte)i;
    }

    // find wildcards - we only allow one to limit zobrist keys
    vector<char> wc;
    int wccount = 0;
    for (auto cb = cubie.begin(); cb != cubie.end(); cb++) {
        for (auto ch = cb->begin(); ch != cb->end(); ch++) {
            auto it = key.find(*ch);
            if (it == key.end()) {
                wc.push_back(*ch);
            }
        }
    }

    // sort wildcards ascii-etically
    sort(wc.begin(),wc.end());

    // add wildcards to key
    for (auto ch = wc.begin(); ch != wc.end(); ch++) key[*ch] = (byte)key.size();

    // done
    return true;
}
