#include "bf_interpreter_functions.hpp"
int main(int argc, char *argv[]) {
    const string instructions = pre_process(scan_source(R"(G:\BrainFuck\tester\rw.bf)")); // should be argv[1]
    instructions_check(instructions);
    jump_map right_to_left, left_to_right;
    build_jump_table(right_to_left,left_to_right, instructions);
    RuntimeStack stk(STACK_SIZE);
    execute(instructions, right_to_left, left_to_right, stk);
    return 0;
}
