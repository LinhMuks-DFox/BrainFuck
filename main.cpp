#include "BrainFuckInterpreter.hpp"
#include <vector>

// Trying to oop-ify this Interpreter
int main(int argc, char *argv[]) {
    return Interpreter()(argv[1], [&argv, &argc]() -> int {
        vector<string> argv_;
        argv_.reserve(argc);
        for (int i = 0; i < argc; ++i) {
            argv_.emplace_back(argv[i]);
        }
        if (argv_.size() > 2) {
            stringstream ss(argv_[2]);
            int ret;
            ss >> ret;
            return ret;
        }
        return 300;
    }());
}
