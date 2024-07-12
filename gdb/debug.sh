#!/bin/sh

gdb --init-command=./gdb/.gdbinit --args $1 $2 
