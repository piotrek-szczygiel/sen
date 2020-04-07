#!/bin/bash

code="$(dirname "$(readlink -f "$0")")"
rm -rf "$code/out"
