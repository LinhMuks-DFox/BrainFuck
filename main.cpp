#include "BrainFuckInterpreter.hpp"
#include "CommandLineParser.hpp"

const static char *BF_INTERPRETER_VERSION = "1.0.1";
const static char *COMPILER =
#ifdef __clang__
        "clang++";
#elif __GNUC__
        "g++";
#elif _MSC_VER
        "MSVC";
#else
    nullptr;
#endif

int main(int argc, char *argv[]) {
    CommandLineParser parser(argc, argv);
    if (argc == 1) {
        /* Todo:
         * Actually, we want when no argument inputted, to launch command line bf-interactive command line interpreter
         * */
        cout << "use brainfuck --help to see how to use." << endl;
        return 0;
    }
    if (parser.cmd_option_exists("-h") ||
        parser.cmd_option_exists("--help") ||
        parser.cmd_option_exists("-help")) {
        cout << "bf [-f: file_path] [-s: stack size] [-h/--help/-help: document] [-v/--version/-version: version]"
             << endl;\
        cout << "Mux Version brain fuck Syntax: " << endl;
        cout << "'+': Increase byte at the data pointer by one." << endl;
        cout << "'-': Decrease byte at the data pointer by one." << endl;
        cout << "'<': Move the data pointer to point to the next cell to the left." << endl;
        cout << "'>': Move the data pointer to point to the next cell to the right." << endl;
        cout << "'[': Jump to the next instruction of the matching ] if the current data pointer is pointing to 0"
             << endl;
        cout
                << "']': Jump to the next instruction of the matching [, if the data pointed to by the current data pointer is not 0"
                << endl;
        cout << "'.': Output the byte at the data pointer." << endl;
        cout << "',': Accept one byte of input, storing its value in the byte at the data pointer." << endl;
        cout << "'#': Comment" << endl;
        cout << "';': Display the entire run stack" << endl;
        cout << "':': Display the current position of the data pointer" << endl;
        cout << "'%': Display the contents of the runtime stack from the start to the data pointer position" << endl;
        return 0;
    }
    if (parser.cmd_option_exists("-v") || parser.cmd_option_exists("--version") ||
        parser.cmd_option_exists("-version")) {
        cout << "Mux BrainFuck Interpreter, CopyRight: Mux 2021" << '[' << COMPILER << "/std:" << __cplusplus << ']'
             << endl;
        cout << "Version: " << BF_INTERPRETER_VERSION << endl;

        return 0;
    }
    string path = parser.cmd_option_exists("-f") ? parser.get_cmd_option("-f") : "";
    if (path.empty()) {
        cerr << "File path is empty." << endl;
        cerr << "Use option -f to tell interpreter where is the script file." << endl;
        cerr << "example: BrainFuck -f test.bf" << endl;
        return 0;
    }
    int stack_size = [&parser]() -> int {
        if (parser.cmd_option_exists("-s")) {
            stringstream ss(parser.get_cmd_option("-s"));
            int ret;
            ss >> ret;
            return ret;
        } else
            return 300;
    }();
    return Interpreter()(path, stack_size);
}
