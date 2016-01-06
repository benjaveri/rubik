#ifndef GO_COMMAND_H
#define GO_COMMAND_H

#include "base.h"
#include "cube.h"

/*
 * commands
 */
enum class Status {
    Success, Failed, NoMatch
};

struct Command {
    virtual Status operator()(const vector<string>& term) = 0;
};

extern Command *command[];

/*
 * stack
 */
struct Element {
    Key key;
    Cube cube;
};

extern stack<Element> cstack;


#endif //GO_COMMAND_H
