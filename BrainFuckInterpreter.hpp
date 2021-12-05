#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <map>
#include <memory>

using namespace std;

struct RuntimeStack {
private:
    void release_handle() {
        stk = nullptr;
        p = nullptr;
    }

public:
    char *stk = nullptr;
    char *p = nullptr;
    size_t stk_size = 0;

    explicit RuntimeStack(size_t size) {
        stk = new char[size];
        for (size_t i = 0; i < size; ++i) stk[i] = 0;
        p = stk;
        stk_size = size;
    }

    RuntimeStack(const RuntimeStack &) = delete;

    RuntimeStack &operator=(const RuntimeStack &) = delete;

    RuntimeStack(RuntimeStack &&r_stk) noexcept {
        this->stk = r_stk.stk;
        this->p = r_stk.p;
        r_stk.release_handle();
    }

    RuntimeStack &operator=(RuntimeStack &&r_stk) noexcept {
        this->stk = r_stk.stk;
        this->p = r_stk.p;
        r_stk.release_handle();
        return *this;
    }

    ~RuntimeStack() {
        delete[] stk;
    }

public:
    void show(size_t end) const {
        for (size_t i = 0; i < end; ++i) {
            if (i % 10 == 0 && i > 9)
                cout << ",\n";
            cout << '[' << (int) stk[i] << "] " << flush;
        }
    }
};

struct JumpTable {
    typedef map<size_t, size_t> jump_map;
    jump_map left_right, right_left;

    explicit JumpTable(const string &instructions) {
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
                right_left[position] = stk_pop();
        }

        for (const auto&[k, v]: right_left)
            left_right[v] = k;
    }
};

class Scanner {
private:
    string path;
private:
    string scan_code() {
        std::ifstream f_i(path, ios::in);
        if (!f_i.is_open()) {
            throw runtime_error("Can not open file: " + path);
        }
        stringstream ss;

        while (f_i.good()) {
            if ((char) f_i.peek() == '#') {
                while (f_i.peek() >= 0) {
                    char c = (char) f_i.get();
                    if (c == EOF) break;
                    if (c == '#')
                        continue;
                    if (c == '\n' || c == '\r')
                        break;
                }
            }
            char cur = (char) f_i.get();
            if (cur == EOF) break;
            ss << cur;
        }
        return ss.str();
    };


    static string pre_process(const string &source) {
        stringstream ss;
        set<char> skips = {'\0', ' ', '\n', '\t'};
        for (const auto &ch: source)
            if (skips.find(ch) == skips.end())
                ss << ch;
        return ss.str();
    };

    static string lexical_check(const string &code) {
        set<char> valid_instructions = {
                '<', '>', ',', '.', '+', '-', '[', ']',
                ';', ':', '%', '#'
        };
        stack<char> stk;
        for (const auto &ch: code) {
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

        return code;
    }

public:
    explicit Scanner(const string &path) {
        this->path = path;
    }

    string process() {
        return lexical_check(pre_process(scan_code()));
    }
};

class Executor {
private:
    unique_ptr<JumpTable> table = nullptr;
    string instructions;
    unique_ptr<RuntimeStack> stk = nullptr;
private:
    static void write_console(const char *p) {
        cout << *p << flush;
    }

    void show_runtime_stk() {
        stk->show(stk->stk_size);
    }

    [[maybe_unused]] void show_instructions() const noexcept {
        cout << "Try to interpret: \n{\n\t" << instructions << "\n}" << endl;
    }

    static char read_char() {
        char ret;
        cin >> ret;
        return ret;
    }

public:
    explicit Executor(const string &instructions, size_t stk_size) {
        this->instructions = instructions;
        table = make_unique<JumpTable>(instructions);
        stk = make_unique<RuntimeStack>(stk_size);
    }

    int execute() {
        size_t position = 0;
        char *runtime_stk = stk->stk, *p = stk->p;
        while (position < instructions.size()) {
            char ins = instructions[position];
            switch (ins) {
                case '>':
                    if (p == runtime_stk + this->stk->stk_size)
                        throw overflow_error(
                                "RuntimeError:can not move pc pointer anymore, pc pointer is out of stack.");
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
                    position = *p == 0 ? table->left_right[position] : position;
                    break;
                case ']':
                    position = *p != 0 ? table->right_left[position] : position;
                    break;
                case ';':
                    stk->show(stk->stk_size);
                    break;
                case ':':
                    cout << p - runtime_stk << flush;
                    break;
                case '%':
                    stk->show(p - runtime_stk);
                    break;
                default:
                    cerr << "Interpreter Error" << endl;
                    return -1;
            }
            position++;
        }
        return 0;
    }
};

struct Interpreter {
    int operator()(const string &path, int stack_size = 300) {
        Scanner preprocessor(path);
        Executor executor(preprocessor.process(), stack_size);
        return executor.execute();

    }
};
