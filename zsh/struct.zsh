#!/bin/bash/

mkdir -p project_struct/

if [ "$#" -eq 0 ]; then
    tree --gitignore > project_struct/project_struct.txt 
    tree > project_struct/without_gitignore_project_struct.txt
    tree -a > project_struct/all_project_struct.txt


elif [ "$#" -eq 1 ]; then

    if [ "$1" == "-c" ] || [ "$1" == "--console" ]; then
        tree -C --gitignore

    elif [ "$1" == "-a" ] || [ "$1" == "--all" ]; then
        tree -C

    else
        echo "undef arg"
        exit 1
    fi

fi
