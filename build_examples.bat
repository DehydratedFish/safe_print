@echo off

IF NOT EXIST "build" mkdir build
pushd build

SET sources=..\examples\basic_print.c ..\examples\basic_file_print.c ..\examples\change_file_type.c ..\examples\change_number_conversion.c

cl /FC /nologo /std:c11 /permissive- /Fe"basic_print.exe" ..\examples\basic_print.c
cl /FC /nologo /std:c11 /permissive- /Fe"basic_file_print.exe" ..\examples\basic_file_print.c
cl /FC /nologo /std:c11 /permissive- /Fe"change_file_type.exe" ..\examples\change_file_type.c
cl /FC /nologo /std:c11 /permissive- /Fe"change_number_converison.exe" ..\examples\change_number_conversion.c

popd

