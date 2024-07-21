#!/bin/bash

git add .
git commit -m  "`git status -s`"
git push origin dev
