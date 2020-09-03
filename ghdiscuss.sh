#!/bin/sh
./ghdiscuss $@ | emojify | mdv -t random -
