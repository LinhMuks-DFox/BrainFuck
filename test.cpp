//
// Created by Mux on 2021/12/8.
//
#include "BrainFuckInterpreter.hpp"

int main() {
    string s = "# test comment\n"
               "++++++++++\n"
               "[\n"
               "    >+++++++>++++++++++>+++>+<<<<-\n"
               "]\n"
               ">++.>+.+++++++..+++.>++.<<+++++++++++++++.\n"
               ">.+++.------.--------.>+.>.\n"
               "[%[-]<]";


    cout << Scanner::scan_string(s) << endl;
}
