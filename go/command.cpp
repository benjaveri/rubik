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
#include "command.h"
#include "env.h"

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
 * DUP
 */
struct Duplicate : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"dup")) return Status::NoMatch;
        if (!ensure(1)) return Status::Failed;
        cstack.push(cstack.top());
        return Status::Success;
    }
} duplicate;

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
 * RESET
 */
struct Reset : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"reset")) return Status::NoMatch;
        env.reset();
        return Status::Success;
    }
} reset;

/*
 * SET
 */
struct Set : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"set")) return Status::NoMatch;
        if (term.size() < 3) {
            cout << "Insufficient number of arguments\n";
            return Status::Failed;
        }
        if (match(term,1,"halfturn")) {
            env.halfTurn = match(term,2,"on");
        } else if (match(term,1,"medianturn")) {
            env.medianTurn = match(term,2,"on");
        } else if (match(term,1,"wideturn")) {
            env.wideTurn = match(term,2,"on");
        } else if (match(term,1,"cuberotation")) {
            env.cubeRotation = match(term,2,"on");
        } else if (match(term,1,"showduration")) {
            env.showDuration = match(term,2,"on");
        } else if (match(term,1,"xptable")) {
            stringstream ss(term[2]);
            ss >> env.xpTable;
        } else if (match(term,1,"threads")) {
            stringstream ss(term[2]);
            ss >> env.threads;
        } else {
            cout << "Unknown setting\n";
            return Status::Failed;
        }

        // done
        return Status::Success;
    }
} set;

/*
 * SOLVE
 */
struct Solve : public Command {
    Status operator()(const vector<string>& term) {
        if (!match(term,0,"solve")) return Status::NoMatch;
        if (!ensure(2)) return Status::Failed;

        // get start and finish cubes off stack
        Element st0,st1;
        st0 = cstack.top(); cstack.pop();
        st1 = cstack.top(); cstack.pop();
        cstack.push(st1);
        cstack.push(st0);

        // solve
        auto t0 = chrono::steady_clock::now();
        bool rv = Cube::solve(st0.cube,st1.cube);
        auto t1 = chrono::steady_clock::now();
        if (env.showDuration) {
            cout << "Elapsed time: " << chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() << "ms\n";
        }

        return rv ? Status::Success : Status::Failed;
    }
} solve;

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
    &duplicate,
    &pop,
    &print,
    &push,
    &reset,
    &set,
    &solve,
    &twist,
    &catchAll
};

stack<Element> cstack;
