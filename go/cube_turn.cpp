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


const char *Cube::lutTurnName[(int)Turn::TOTAL] = {
    "U", "U2", "U'", "E", "E2", "E'", "D", "D2", "D'",
    "L", "L2", "L'", "M", "M2", "M'", "R", "R2", "R'",
    "F", "F2", "F'", "S", "S2", "S'", "B", "B2", "B'",
    "X", "X2", "X'", "Y", "Y2", "Y'", "Z", "Z2", "Z'",
    "Uw", "Uw2", "Uw'", "Dw", "Dw2", "Dw'",
    "Lw", "Lw2", "Lw'", "Rw", "Rw2", "Rw'",
    "Fw", "Fw2", "Fw'", "Bw", "Bw2", "Bw'",
};

map<string,Turn> Cube::lutTurnNameMap;

/*
    U
    UFL UF UFR UR UBR UB UBL UL
    FL  F  FR  R  BR  B  BL  L
    DFL DF DFR DR DBR DB DBL DL
    D
*/

// a few basic turns from which we can compose all turns
const char *Cube::lutSeedCube[] = {
        // reference
        "RRRRRRRRR YYYYYYYYY BBBBBBBBB WWWWWWWWW GGGGGGGGG OOOOOOOOO",
        // U
        "RRRRRRRRR BBBYYYYYY WWWBBBBBB GGGWWWWWW YYYGGGGGG OOOOOOOOO",
        // E
        "RRRRRRRRR YYYGGGYYY BBBYYYBBB WWWBBBWWW GGGWWWGGG OOOOOOOOO",
        // X
        "YYYYYYYYY OOOOOOOOO BBBBBBBBB RRRRRRRRR GGGGGGGGG WWWWWWWWW",
        // Y
        "RRRRRRRRR BBBBBBBBB WWWWWWWWW GGGGGGGGG YYYYYYYYY OOOOOOOOO",
    // done
    NULL
};

// express all turns in terms of U, E, X and Y. allow new definitions to be used
//  right away
const char *Cube::lutTurnDef[] = {
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


Cube::Twist Cube::lutTwist[(int)Turn::TOTAL];

byte Cube::edgeRotation(byte org,int rot) {
    return rot ? ((org >> 4) | (org << 4)) : org;
}

word Cube::cornerRotation(word org,int rot) {
    switch (rot) {
        case 0: return org;                                                                 // 210
        case 1: return (word)((org & 0xf00) | ((org & 0x0f0) >> 4) | ((org & 0x00f) << 4)); // 201
        case 2: return (word)(((org & 0xf00) >> 4) | ((org & 0x0f0) << 4) | (org & 0x00f)); // 120
        case 3: return (word)(((org & 0xf00) >> 8) | ((org & 0x0ff) << 4));                 // 102
        case 4: return (word)(((org & 0xf00) >> 8) | (org & 0x0f0) | ((org & 0x00f) << 8)); // 012
        case 5: return (word)(((org & 0xff0) >> 4) | ((org & 0x00f) << 8));                 // 021
        default: assert(false); return 0;
    }
}

void Cube::computeTwist(const Cube& r,const Cube& c,Cube::Twist &t) {
    assert(false);
//    // determine face shuffle
//    for (int f = 0; f < 6; f++) {
//        bool found = false;
//        for (int g = 0; g < 6; g++) {
//            if (c.face[f] == r.face[g]) {
//                t.faceShuffle[f] = (byte)g;
//                found = true;
//                break;
//            }
//        }
//        assert(found);
//    }
//
//    // determine edge shuffle
//    for (int f = 0; f < 12; f++) {
//        bool found = false;
//        for (int g = 0; g < 12*2; g++) {
//            if (c.edge[f] ==  edgeRotation(r.edge[g % 12],g/12)) {
//                t.edgeShuffle[f] = (byte)(((g/12) << 4) | (g % 12));
//                found = true;
//                break;
//            }
//        }
//        assert(found);
//    }
//
//    // determine corner shuffle
//    for (int f = 0; f < 8; f++) {
//        bool found = false;
//        for (int g = 0; g < 8*6; g++) {
//            if (c.corner[f] ==  cornerRotation(r.corner[g & 7],g >> 3)) {
//                t.cornerShuffle[f] = (byte)g;
//                found = true;
//                break;
//            }
//        }
//        assert(found);
//    }
}


inline int encodeFace(Face a) { return (int) a; }
inline int encodeEdge(Face a,Face b) { return ((int)a)+6*((int)b); }
inline int encodeCorner(Face a,Face b,Face c) { return ((int)a)+6*((int)b)+36*((int)c); }

const int cornerCubieID[8] = {
    encodeCorner(Face::U,Face::L,Face::F),
    encodeCorner(Face::U,Face::F,Face::R),
    encodeCorner(Face::U,Face::R,Face::B),
    encodeCorner(Face::U,Face::B,Face::L),
    encodeCorner(Face::D,Face::F,Face::L),
    encodeCorner(Face::D,Face::R,Face::F),
    encodeCorner(Face::D,Face::B,Face::R),
    encodeCorner(Face::D,Face::L,Face::B),
};

const int cornerCubieVec[8][3] = {
    { 0,2,1 },
    { 0,1,2 },
    { 0,2,1 },
    { 0,1,2 },
    { 0,2,1 },
    { 0,1,2 },
    { 0,2,1 },
    { 0,1,2 }
};

void Cube::generateTables() {
    // initialize turn name lut
    for (int i = 0; i < (int)Turn::TOTAL; i++) lutTurnNameMap[string(lutTurnName[i])] = (Turn)i;

    // compute tables for looking up cubie position and orientation used for printing and parsing
    for (int org = 0; org < 8; org++) {
        int oid = cornerCubieID[org];
        const int *ovc = cornerCubieVec[org];
        for (int cur = 0; cur < 8; cur++) {
            const int *cvc = cornerCubieVec[org];

            // ori == 0
            table[0*8+cur] = oid;

            // ori = 1
            table[1*8+cur] = ?

            // ori = 2
            table[2*8+cur] = ?
            }
        }
    }

//    // parse seed cubes
//    Cube seedCube[5];
//    Key key;
//    for (int i = 0; i < 5; i++) parseKey(lutSeedCube[i],key);
//    for (int i = 0; i < 5; i++) seedCube[i].parseFaces(lutSeedCube[i],key);
//
//    // use deltas between ref and seed to determine twist algorithm for initial turns
//    const Turn turn[] = { Turn::NONE, Turn::U, Turn::E, Turn::X, Turn::Y };
//    const Cube& r = seedCube[0];
//    for (int i = 1; i < 5; i++) {
//        // operate on these elements
//        const Cube& c = seedCube[i];
//        Twist & t = lutTwist[(int)turn[i]];
//
//        // determine shuffle
//        computeTwist(r,c,t);
//
//        // validate correctness
//        Cube temp;
//        r.apply(turn[i],temp);
//        assert (c == temp);
//    }
//
//    // now compose remaining turns based on table definitions
//    for (const char **combo = lutTurnDef; *combo; combo++) {
//        // parse combination
//        string s = string(*combo);
//        string st = s.substr(0,s.find("->"));
//        istringstream iss(s.substr(st.size()+2));
//        vector<string> sd{istream_iterator<string>{iss},istream_iterator<string>{}};
//
//        Turn td = lookupTurn(st);
//        assert(td != Turn::NONE);
//
//        // operate on these elements
//        Cube c = r;
//
//        // rotate per definition
//        for (auto it = sd.begin(); it != sd.end(); it++) {
//            Turn t = lookupTurn(*it);
//            assert(t != Turn::NONE);
//            Cube d = c;
//            d.apply(t,c);
//        }
//
//        // determine shuffle
//        computeTwist(r,c,lutTwist[(int) td]);
//
//        // validate correctness
//        Cube temp;
//        r.apply(td,temp);
//        assert (c == temp);
//    }
}

void Cube::apply(Turn turn,Cube& dest) const {
    assert(false);
//    Twist & t = lutTwist[(int)turn];
//
//    // faces
//    for (int i = 0; i < 6; i++) dest.face[i] = face[t.faceShuffle[i]];
//
//    // edges
//    for (int i = 0; i < 12; i++) dest.edge[i] = edgeRotation(edge[t.edgeShuffle[i] & 15],t.edgeShuffle[i] >> 4);
//
//    // corners
//    for (int i = 0; i < 8; i++) dest.corner[i] = cornerRotation(corner[t.cornerShuffle[i] & 7],t.cornerShuffle[i] >> 3);
}

