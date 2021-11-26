#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <map>

using namespace std;
const static size_t STACK_SIZE = 300;
typedef map<size_t, size_t> jump_map;

struct RuntimeStack {
    char* stk = nullptr;
    char* p = nullptr;
    size_t stk_size = 0;
    explicit RuntimeStack(size_t size) {
        stk = new char[size];
        for (size_t i = 0; i < STACK_SIZE; ++i) stk[i] = 0;
        p = stk;
        stk_size = size;
    }

    ~RuntimeStack(){
        delete [] stk;
    }
};

string scan_source(const string &path) {
    std::ifstream f_i(path, ios::in);
    if (!f_i.is_open()) {
        throw runtime_error("Can not open file: " + path);
    }
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
    set<char> valid_instructions = {
            '<', '>', ',', '.', '+', '-', '[', ']',
            ';', ':'
    };
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

void build_jump_table(jump_map &rl, jump_map &lr, const string &instructions) {
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
            rl[position] = stk_pop();
    }

    for (const auto&[k, v]: rl)
        lr[v] = k;
}

void show_jump_map(const jump_map &jmp_mp) {
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

int execute(const string &instructions, jump_map &jmp_rl, jump_map &jmp_lr, RuntimeStack& stk) {
    size_t position = 0;
    char* runtime_stk = stk.stk, *p = stk.p;
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
                position = *p == 0 ? jmp_lr[position] : position;
                break;
            case ']':
                position = *p != 0 ? jmp_rl[position] : position;
                break;

            case ';':
                show_runtime_stk(runtime_stk);
                break;
            case ':':
                cout << p - runtime_stk << flush;
                break;
            default:
                cerr << "Interpreter Error" << endl;
                return -1;
        }
        position++;
    }
    return 0;
}

