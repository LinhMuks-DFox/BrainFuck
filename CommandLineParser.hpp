#ifndef BRAINFUCK_COMMAND_LINE_PARSER_HPP
#define BRAINFUCK_COMMAND_LINE_PARSER_HPP

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

class CommandLineParser {
public:
    CommandLineParser(int &argc, char **argv) {
        this->tokens.reserve(argc);
        for (int i = 1; i < argc; ++i)
            this->tokens.emplace_back(argv[i]);
    }

    /// @author iain
    [[nodiscard]] string get_cmd_option(const std::string &option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        return "";
    }

    /// @author iain
    [[nodiscard]] bool cmd_option_exists(const std::string &option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option)
               != this->tokens.end();
    }

private:
    std::vector<std::string> tokens;
};

#endif //BRAINFUCK_COMMAND_LINE_PARSER_HPP
