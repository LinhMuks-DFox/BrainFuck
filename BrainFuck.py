'''
Brain Fuck Interpreter Python Edition
Python version should greater than Python 3.10.0
Author: Mux
Copy Right: Mux, 2021
'''



try:
    import sys
    if sys.version_info.major >= 10:
        print("Python version should greater than 3.10")
        exit(-1)
except ImportError as ierr:
    print("Python do not install correctly, reinstall it and run this again.")
    exit(-1)

import os
import re
__all__ = [
    "BrainFuckInterpreter"
]

import sys

IGNORE_CHARACTER = {
    '\n', '\t', ' ', '\0', '\r'
}

SYNTAX = {
    '+': "Increase byte at the data pointer by one.",
    '-': "Decrease byte at the data pointer by one.",
    '.': "Output the byte at the data pointer.",
    ',': "Accept one byte of input, storing its value in the byte at the data pointer.",
    '<': "Move the data pointer to point to the next cell to the left.",
    '>': "Move the data pointer to point to the next cell to the right.",
    '[': "Jump to the next instruction of the matching ] if the current data pointer is pointing to 0",
    ']': "Jump to the next instruction of the matching [, if the data pointed to by the current data pointer is not 0",
    # Non-standard feature, provided and implemented by Mux.
    # For simplifying the debug process of brainfuck programs
    ';': "Display the entire run stack",
    ':': "Display the current position of the data pointer",
    '%': "Display the contents of the runtime stack from the start to the data pointer position",
    '#': "Comment"
}


class SourceScanner:
    def __init__(self, source_input: str):

        if not os.path.exists(source_input):
            raise IOError(f"Source file {source_input} is not exist. use --help to see document")

        if not source_input.endswith(".bf"):
            raise IOError(f"Source file{source_input} is not a valid brainfuck-script use --help to see document")

        with open(source_input, "r") as f:
            source = f.read()

        # To skip comment, only allowed single line.
        source = re.sub(r'(?m)^ *#.*\n?', '', source)
        self.processed_ = ""
        for index, char in enumerate(source):
            if char in IGNORE_CHARACTER:
                continue
            if char not in SYNTAX.keys():
                raise SyntaxError(f"The {index} character(value '{char}') is not a valid instruction")
            self.processed_ += char

        # loop check:
        stk = []
        for char in self.processed_:
            if char == '[':
                stk.append('[')
            elif char == ']':
                if stk.pop() != '[':
                    raise SyntaxError("Unclosed loop")
        if len(stk) != 0:
            raise SyntaxError("Unclosed loop")

    def get_instructions(self):
        return self.processed_


class Executor:
    def __init__(self, instructions: str, runtime_stk_size=300):
        self.instructions_ = instructions
        self._runtime_stk = [0] * runtime_stk_size
        # build jump map
        self.lr_map = dict()
        self.rl_map = dict()
        stk = []
        for index, ins in enumerate(instructions):
            if ins == '[':
                stk.append(index)
            if (ins == ']'):
                self.rl_map[index] = stk.pop()
        for k, v in self.rl_map.items():
            self.lr_map[v] = k

    def show_rt_stk(self, i, j):
        # print("".join([f"\n{index}:[{content}], " if index % 10 == 0 and index > 9 else f"{index}:[{content}], "
        #                for index, content in enumerate(self._runtime_stk)]))
        print("".join([
            f"\n{idx}:[{self._runtime_stk[idx]}], " if idx % 10 == 0 and idx > 9 else f"{idx}:[{self._runtime_stk[idx]}], "
            for idx in range(i, j)]))

    def execute(self) -> int:
        dp, ins_position = 0, 0
        while ins_position < len(self.instructions_):
            ins = self.instructions_[ins_position]
            match ins:
                case '+':
                    self._runtime_stk[dp] += 1
                case '-':
                    self._runtime_stk[dp] -= 1
                case '<':
                    if dp == 0:
                        raise IndexError("Cannot move data pointer any more.")
                    dp -= 1
                case '>':
                    if dp == len(self._runtime_stk):
                        raise IndexError("Cannot move data pointer any more")
                    dp += 1
                case '[':
                    ins_position = self.lr_map[ins_position] if self._runtime_stk[dp] == 0 else ins_position
                case ']':
                    ins_position = self.rl_map[ins_position] if self._runtime_stk[dp] != 0 else ins_position
                case ',':
                    self._runtime_stk[dp] = ord(sys.stdin.read(1))
                case '.':
                    sys.stdout.write(chr(self._runtime_stk[dp]))
                case ';':
                    self.show_rt_stk(0, len(self._runtime_stk))
                case ':':
                    print(dp, end='')
                case '%':
                    self.show_rt_stk(0, dp)
            ins_position += 1
        return 0


class BrainFuckInterpreter:
    def __call__(self, path: str, runtime_stk_size=300, *args, **kwargs):
        return Executor(SourceScanner(path).get_instructions(), runtime_stk_size).execute()


if __name__ == "__main__":
    try:
        exit(BrainFuckInterpreter()(sys.argv[1]))
    except IndexError as idxerr:
        print("No file was inputted.")
        print("usage: python BrainFuck.py [Brain Fuck Module].bf")