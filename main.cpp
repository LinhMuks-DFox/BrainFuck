#include <iostream>
#include <set>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <map>

using namespace std;
const static size_t STACK_SIZE = 300;

string scan_source(const string &path) {
    std::ifstream f_i(path, ios::in);
    stringstream ss;
    ss << f_i.rdbuf();
    return ss.str();
}

string pre_process(const string &source) {
    stringstream ss;
    set<char> skips = {'\0', ' ', '\n', '\t'};
    for (const auto &ch: source)
        if (skips.find(ch) == skips.end())
            ss << ch;
    return ss.str();
}

void instructions_check(const string &instructions) {
    set<char> valid_instructions = {'<', '>', ',', '.', '+', '-', '[', ']'};
    stack<char> stk;
    for (const auto &ch: instructions) {
        if (valid_instructions.find(ch) == valid_instructions.end()) {
            string msg = "invalid instruction:";
            msg += ch;
            throw invalid_argument(msg);
        }

        if (ch == '[')
            stk.push('[');
        if (ch == ']') {
            auto top = stk.top();
            stk.pop();
            if (top != '[')
                throw invalid_argument("SyntaxError: Unclosed Loop");
        }
    }
    if (!stk.empty())throw invalid_argument("SyntaxError: Unclosed Loop");
}

void write_console(const char *p) {
    cout << *p << flush;
}

char read_char() {
    char ret;
    cin >> ret;
    return ret;
}

void build_jump_table(map<size_t, size_t> &jump_map, const string &instructions) {
    size_t position = 0;
    stack<size_t> stk;
    auto stk_pop = [&stk]() -> size_t {
        auto ret = stk.top();
        stk.pop();
        return ret;
    };
    for (; position < instructions.size(); ++position) {
        if (instructions[position] == '[')
            stk.push(position);
        if (instructions[position] == ']')
            jump_map[position] = stk_pop();
    }
}

void show_jump_map(const map<size_t, size_t> &jmp_mp) {
    for (const auto &[k, v]: jmp_mp) {
        cout << k << ":" << v << endl;
    }
}

void show_instructions(const string &ins) {
    cout << "Try to interpret: \n{\n\t" << ins << "\n}" << endl;
}

void show_runtime_stk(const char *stk, size_t stk_size = STACK_SIZE) {
    for (size_t i = 0; i < stk_size; ++i) {
        if (i % 10 == 0 && i > 9)
            cout << ",\n";
        cout << '[' << (int) stk[i] << "] " << flush;
    }
}

int main(int argc, char *argv[]) {
    const string instructions = pre_process(scan_source(argv[1])); // should be argv[1]
    instructions_check(instructions);

    /*Pre-interpret*/
    char *runtime_stk = new char[STACK_SIZE];
    for (size_t i = 0; i < STACK_SIZE; ++i) runtime_stk[i] = 0;
    char *p = runtime_stk;
    map<size_t, size_t> rl_jmp_map;
    build_jump_table(rl_jmp_map, instructions);
    map<size_t, size_t> lr_jmp_map;
    for (const auto&[k, v]: rl_jmp_map)
        lr_jmp_map[v] = k;
    // execute

    size_t position = 0;
    while (position < instructions.size()) {
        char ins = instructions[position];
        switch (ins) {
            case '>':
                if (p == runtime_stk + STACK_SIZE)
                    throw overflow_error("RuntimeError:can not move pc pointer anymore, pc pointer is out of stack.");
                p++;
                break;
            case '<':
                if (p == runtime_stk)
                    throw underflow_error("RuntimeError:pc pointer is now points to the top of rt stack.");
                p--;
                break;
            case '+':
                (*p)++;
                break;
            case '-':
                (*p)--;
                break;
            case '.':
                write_console(p);
                break;
            case ',':
                *p = read_char();
                break;
            case '[':
                position = *p == 0 ? lr_jmp_map[position] : position;
                break;
            case ']':
                position = *p != 0 ? rl_jmp_map[position] : position;
                break;
            default:
                cerr << "Interpreter Error" << endl;
                return -1;
        }
        position++;
    }
    // show_runtime_stk(runtime_stk);
    delete[] runtime_stk;
    return 0;
}
