#!/bin/bash

aclocal -I ../m4 \
&& automake --add-missing -c \
&& autoconf
