#include "command.h"

inline bool match(const vector<string>& term,int index,string value) {
    return (term.size() > index) && (term[index] == value);
}

inline bool ensure(int depth) {
    if (cstack.size() < depth) {
        cout << "Expected " << depth << " element(s) on stack but only " << cstack.size() << " found\n";
        return false;
    }
    return true;
}

/*
 * catch-all
 */
struct CatchAll : public Command {
    Status operator()(const vector<string>& term) {
        if (term.size() > 0) {
            if (term[0][0] == '#') {
                // comments are fine
                return Status::Success;
            }
            cout << "Syntax error (" << term[0] << ")\n";
            return Status::Failed;
        } else {
            // blank lines are fine
            return Status::Success;
        }
    }
} catchAll;

/*
 * POP
 */
struct Pop : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"pop")) return Status::NoMatch;

        // pop
        if (!ensure(1)) return Status::Failed;
        cstack.pop();
        return Status::Success;
    }
} pop;

/*
 * PRINT
 */
struct Print : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"print")) return Status::NoMatch;
        if (!ensure(1)) return Status::Failed;
        if (match(term,1,"cubies")) {
            cout << cstack.top().cube.constructCubies(cstack.top().key) << "\n";
        } else {
            cout << cstack.top().cube.constructFaces(cstack.top().key) << "\n";
        }
        return Status::Success;
    }
} print;

/*
 * PUSH
 */
struct Push : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"push")) return Status::NoMatch;

        // parse cubies of faces depending on number of arguments
        string def;
        for (auto it = ++term.begin(); it != term.end(); it++) def += *it + " ";
        Element e;
        if (!Cube::parseKey(def,e.key)) {
            cout << "Unable to determine color key for cube\n";
            return Status::Failed;
        }
        int rv = 0;
        switch (term.size()) {
            case 7: rv = e.cube.parseFaces(def,e.key); break;
            case 27: rv = e.cube.parseCubies(def,e.key); break;
            default: rv = 0; break;
        }
        if (rv >= 0) {
            cout << "Error at element " << (rv+1) << "\n";
            return Status::Failed;
        }

        // push
        cstack.push(e);
        return Status::Success;
    }
} push;

/*
 * TURN|TWIST
 */
struct Twist : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"turn") && !match(term,0,"twist")) return Status::NoMatch;
        if (!ensure(1)) return Status::Failed;

        // apply turns
        Cube& cube = cstack.top().cube;
        for (auto it = ++term.begin(); it != term.end(); it++) {
            // find turn to apply
            auto t = Cube::lutTurnMap.find(*it);
            if (t == Cube::lutTurnMap.end()) {
                cout << "Invalid turn (" << *it << ")\n";
                return Status::Failed;
            }

            // apply turn
            Cube temp;
            cube.apply(t->second,temp);
            cube = temp;
        }

        // done
        return Status::Success;
    }
} twist;

/*
 * command list
 */
Command *command[] = {
    &pop,
    &print,
    &push,
    &twist,
    &catchAll
};

stack<Element> cstack;
