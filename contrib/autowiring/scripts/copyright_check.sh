#!/usr/bin/env bash

#
# Enforce LeapMotion copyright notice
#

ENFORCED_FILES="autowiring examples src"
COPYRIGHT_HEADER="// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved."

# Go to root directory
for f in $(find $ENFORCED_FILES -name *.hpp -o -name *.cpp -o -name *.h);
do 
  if [ "$(head -n 1 $f)" != "$COPYRIGHT_HEADER" ];
  then
    if [ "$BAD_FILES" == "" ]
    then
      echo
      echo "Some files are missing the correct copyright header"
      echo
      echo "Please update the following files with the this header:"
      echo $COPYRIGHT_HEADER
      echo
      BAD_FILES=true
    fi

    echo $f
  fi
done

if [ $BAD_FILES ]; then
  echo
  exit 1
fi
