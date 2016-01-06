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