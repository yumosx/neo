#!/bin/bash

if [ $# -lt 1 ]; then
    echo "At least one column name is required."
    exit 1
fi

echo "SELECT" > prog.sql

if [ -n "$1" ]; then
    echo "$1" >> prog.sql
fi

echo "FROM student" >> prog.sql

if [ -n "$2" ]; then
    echo " $2" >> prog.sql
fi

cat prog.sql

exit 0
