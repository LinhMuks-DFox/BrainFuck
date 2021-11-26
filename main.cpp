#include "BrainFuckInterpreter.hpp"

// Trying to oop-ify this Interpreter
int main(int argc, char *argv[]) {
    Interpreter interpreter;
    return interpreter(argv[1]);
}
