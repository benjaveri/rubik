#include "cube.h"

/*
    U
    UFL UF UFR UR UBR UB UBL UL
    FL  F  FR  R  BR  B  BL  L
    DFL DF DFR DR DBR DB DBL DL
    D
*/
// program used to parse above cube definition
const int Cube::cubeDefPgm[][2] = {
        { 0, 0, },
        { 2, 0 }, { 1, 0 }, { 2, 1 }, { 1, 1 }, { 2, 2 }, { 1, 2 }, { 2, 3 }, { 1, 3 },
        { 1, 4 }, { 0, 4 }, { 1, 5 }, { 0, 8 }, { 1, 6 }, { 0, 12 }, { 1, 7 }, { 0, 16 },
        { 2, 4 }, { 1, 8 }, { 2, 5 }, { 1, 9 }, { 2, 6 }, { 1, 10 }, { 2, 7 }, { 1, 11 },
        { 0, 20 }
};

// a few basic turns from which we can compose all turns
const char *Cube::seed[] = {
    // reference
    "R "
    "RYG RY RYB RB RWB RW RWG RG "
    "YG  Y  YB  B  WB  W  WG  G "
    "OYG OY OYB OB OWB OW OWG OG "
    "O",
    // U
    "R "
    "RBY RB RBW RW RGW RG RGY RY "
    "YG  Y  YB  B  WB  W  WG  G "
    "OYG OY OYB OB OWB OW OWG OG "
    "O",
    // E
    "R "
    "RBY RB RBW RW RGW RG RGY RY "
    "GW  G  GY  Y  BY  B  BW  W "
    "OYG OY OYB OB OWB OW OWG OG "
    "O",
    // X
    "Y "
    "YOG YO YOB YB YRB YR YRG YG "
    "OG  O  OB  B  RB  R  RG  G "
    "WOG WO WOB WB WRB WR WRG WG "
    "W",
    // Y
    "R "
    "RBY RB RBW RW RGW RG RBY RY "
    "BY  B  BW  W  GW  G  GY  Y "
    "OBY OB OBW OW OGW OG OGY OY "
    "O",
    // done
    NULL
};

// express all turns in terms of U, E, X and Y. allow new definitions to be used
//  right away
const char *Cube::combinations[] = {
    // U, U2, U'
    "U->U", "U2->U U", "U'->U2 U",
    // E, E2, E'
    "E->E", "E2->E E", "E'->E2 E",
    // X, X2, X'
    "X->X", "X2->X X", "X'->X2 X",
    // Y, Y2, Y'
    "Y->Y", "Y2->Y Y", "Y'->Y2 Y",
    // Z, Z2, Z'
    "Z->Y' X Y", "Z2->Z Z", "Z'->Z2 Z",
    // D, D2, D'
    "D->X2 U X2", "D2->D D", "D'->D2 D",
    // L, L2, L'
    "L->Z U Z'", "L2->L L", "L'->L2 L",
    // M, M2, M'
    "M->Z E' Z'", "M2->M M", "M'->M2 M",
    // R, R2, R'
    "R->Z D Z'", "R2->R R", "R'->R2 R",
    // F, F2, F'
    "F->X U X'", "F2->F F", "F'->F2 F",
    // S, S2, S'
    "S->X E' X'", "S2->S S", "S'->S2 S",
    // B, B2, B'
    "B->X D X'", "B2->B B", "B'->B2 B",
    // Uw, Uw2, Uw' 
    "Uw->U E'", "Uw2->Uw Uw", "Uw'->Uw2 Uw",
    // Dw, Dw2, Dw'
    "Dw->D E", "Dw2->Dw Dw", "Dw'->Dw2 Dw",
    // Lw, Lw2, Lw' 
    "Lw->L M", "Lw2->Lw Lw", "Lw'->Lw2 Lw",
    // Rw, Rw2, Rw'
    "Rw->R M'", "Rw2->Rw Rw", "Rw'->Rw2 Rw",
    // Fw, Fw2, Fw' 
    "Fw->F S", "Fw2->Fw Fw", "Fw'->Fw2 Fw",
    // Bw, Bw2, Bw'
    "Bw->B S'", "Bw2->Bw Bw", "Bw'->Bw2 Bw",
    // done
    NULL
};

const char *Cube::turnNames[(int)Turn::TOTAL] = {
    "U", "U2", "U'", "E", "E2", "E'", "D", "D2", "D'",
    "L", "L2", "L'", "M", "M2", "M'", "R", "R2", "R'",
    "F", "F2", "F'", "S", "S2", "S'", "B", "B2", "B'",
    "X", "X2", "X'", "Y", "Y2", "Y'", "Z", "Z2", "Z'"
    "Uw", "U2w", "Uw'", "Dw", "Dw2", "Dw'",
    "Lw", "L2w", "Lw'", "Rw", "Rw2", "Rw'",
    "Fw", "F2w", "Fw'", "Bw", "Bw2", "Bw'",
};

Cube::twist_t Cube::twist[(int)Turn::TOTAL];

void Cube::generateTables() {
    // parse seed cubes
    Cube c[5];
    map<char,byte> key;
    for (int i = 0; i < 5; i++) parseKey(seed[i],key);
    for (int i = 0; i < 5; i++) c[i].parseCubies(seed[i],key);
}

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

    // find wildcards
    vector<char> wc;
    for (auto cb = cubie.begin(); cb != cubie.end(); cb++) {
        for (auto ch = cb->begin(); ch != cb->end(); ch++) {
            auto it = key.find(*ch);
            if (it == key.end()) wc.push_back(*ch);
        }
    }

    // sort wildcards ascii-etically
    sort(wc.begin(),wc.end());

    // add wildcards to key
    for (auto ch = wc.begin(); ch != wc.end(); ch++) key[*ch] = (byte)key.size();

    // done
    return true;
}


void Cube::apply(Turn move,Cube& dest) {

}

