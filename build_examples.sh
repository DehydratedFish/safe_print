#!/bin/bash

mkdir -p build
pushd build

gcc -Wall -std=gnu11 -obasic_print ../examples/basic_print.c
gcc -Wall -std=gnu11 -obasic_file_print ../examples/basic_file_print.c
gcc -Wall -std=gnu11 -ochange_file_type ../examples/change_file_type.c
gcc -Wall -std=gnu11 -ochange_number_conversion ../examples/change_number_conversion.c

popd

