#!/bin/sh
exec ./bin/fsmconv "$1" --to-nfa --format=dot
