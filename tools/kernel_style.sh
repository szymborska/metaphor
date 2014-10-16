#!/bin/zsh

# Use indent to change source code to Linux kernel style
indent -kr -i8 -ts8 -sob -l80 -ss -bs -psl $1
