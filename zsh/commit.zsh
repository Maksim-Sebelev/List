#!/bin/bash/

# argument = commit name

if [ "$#" -eq 0 ]; then
    commit=ZoV
elif [ "$#" -eq 1 ]; then
    commit=$1
else
    echo "Too many arguments."
    echo "Expected only commit name."
    exit 0
fi

# copy_bash=bash/git/copy.bash

# if [[ ! -f "${copy_bash}" ]]; then
#     echo "${copy_bash} - doesn't exist."
#     echo "Failed made copy of this repository!"
#     exit 1;
# fi

# bash ${copy_bash} .

# if [ "$?" != "0" ]; then
#     echo "Failed made copy of this repository!"
#     exit 1;
# fi


bash zsh/struct.zsh || exit 1

git rm -r --cached .
git ls-files -ci --exclude-standard -z | xargs -0 git rm --cached # remove .gitignore files, if they on git
git pull origin main
git add --all
git commit -m "${commit}"
git push --all
