#!/bin/sh

git pull origin main --ff
git fetch gitlab
git rebase gitlab/main
git rebase origin/main
git submodule update --recursive
git push
