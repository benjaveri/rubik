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
    TOTAL
};

typedef vector<Turn> TurnList;


class Cube {
    //
    // state
    //
public:
    byte face[6];   // U F R B L D each 4 bits
    byte edge[12];  // UF UR UB UL FL FR RB BL DF DR DB DL each 4:4 bits
    word corner[8]; // UFL UFR UBR UBL DFL DFR DBR DBL each 4:4:4 bits


    //
    // turns (cube_turn.cpp)
    //
protected:
    static const int cubeDefPgm[][2];
    static const char *seed[];
    static const char *combinations[];
    static const char *turnNames[(int)Turn::TOTAL];

public:
    static map<string,Turn> turnMap;
    static map<string,Face> faceMap;
    static map<string,Cubie> cubieMap;


public:
    struct twist_t {
        byte faceShuffle[6];
        byte edgeMove[12];
        byte edgeShuffle[12];
        byte cornerMove[8];
        byte cornerShuffle[8];
    } PACKED;

protected:
    static twist_t twist[(int)Turn::TOTAL];

public:
    static void generateTables();

public:
    int parseCubies(const string& def,map<char,byte>& key); // -1 for success, else cubie # where error is
    static bool parseKey(const string& def,map<char,byte>& key);

public:
    void apply(Turn turn,Cube& dest);

} PACKED;

#endif //GO_CUBE_H
