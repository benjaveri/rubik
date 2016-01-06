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
#ifndef GO_CUBE_H
#define GO_CUBE_H
#include "base.h"

enum class Face {
    U, F, R, B, L, D
};

enum class Cubie {
    U,
    UFL, UF, UFR, UR, UBR, UB, UBL, UL,
    FL,  F,  FR,  R,  BR,  B,  BL,  L,
    DFL, DF, DFR, DR, DBR, DB, DBL, DL,
    D
};

enum class Turn {
    U, U2, Up, E, E2, Ep, D, D2, Dp,
    L, L2, Lp, M, M2, Mp, R, R2, Rp,
    F, F2, Fp, S, S2, Sp, B, B2, Bp,
    X, X2, Xp, Y, Y2, Yp, Z, Z2, Zp,
    Uw, Uw2, Uwp, Dw, Dw2, Dwp,
    Lw, Lw2, Lwp, Rw, Rw2, Rwp,
    Fw, Fw2, Fwp, Bw, Bw2, Bwp,
    TOTAL, NONE
};

typedef vector<Turn> TurnList;
typedef map<char,byte> Key;

class Cube {
    //
    // string <-> enum mappings
    //
public:
    static const char *lutTurnName[(int)Turn::TOTAL];
    static map<string,Turn> lutTurnMap;
//    static map<string,Face> faceMap;
//    static map<string,Cubie> cubieMap;

    inline static string lookupTurn(Turn t) { return string(lutTurnName[(int)t]); }
    inline static Turn lookupTurn(string s) { auto it = lutTurnMap.find(s); return (it == lutTurnMap.end()) ? Turn::NONE : it->second; }

    //
    // state
    //
public:
    byte face[6];   // U F R B L D each 4 bits
    byte edge[12];  // UF UR UB UL FL FR RB BL DF DR DB DL each 4:4 bits
    word corner[8]; // UFL UFR UBR UBL DFL DFR DBR DBL each 4:4:4 bits

    qword computeHash() const;

    //
    // turns (cube_turn.cpp)
    //
private:
    static const char *lutSeedCube[]; // seed turn cube definitions
    static const char *lutTurnDef[]; // definitions of all turns starting with seed cubes

private:
    struct Twist {
        byte faceShuffle[6];    // 0:4 bits rotate:shuffle
        byte edgeShuffle[12];   // 1:4 bits rotate:shuffle
        byte cornerShuffle[8];  // 2:4 bits rotate:shuffle
    } PACKED;

    static Twist lutTwist[(int)Turn::TOTAL];

private:
    static byte edgeRotation(byte org,int rot);
    static word cornerRotation(word org,int rot);
    static void computeTwist(const Cube& r,const Cube& c,Twist & t);

public:
    static void generateTables();
    void apply(Turn turn,Cube& dest) const;

    //
    // parsing (cube_parser.cpp)
    //
private:
    static const int lutCubieMap[][2]; // for parsing cubies
    static const int lutFaceMap[][3]; // for parsing faces

public:
    static bool parseKey(const string& def,Key& key);

    int parseCubies(const string& def,Key& key); // -1 for success, else cubie # where error is
    int parseFaces(const string& def,Key& key); // -1 for success, else face index where error is

    string constructCubies(Key& key) const;
    string constructFaces(Key& key) const;

} PACKED;

inline bool operator==(const Cube& lhs,const Cube& rhs) {
    return !memcmp(&lhs,&rhs,sizeof(Cube));
}

#endif //GO_CUBE_H
