@echo off

set GameLinkerFlags= -incremental:no -opt:ref -PDB:toast_%random%.pdb -EXPORT:GameUpdate
set PlatformLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib
set CompilerFlags= -nologo -MTd -fp:fast -WX -W4 -GR- -Gm- -EHa- -Od -Oi -wd4201 -wd4100 -wd4189 -wd4505 -FC -Zi

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

del *.pdb > NUL 2 > NUL
echo WAITING FOR PDB > lock.tmp
cl %CompilerFlags% w:\toast\code\toast.cpp -LD /link %GameLinkerFlags%
del lock.tmp
cl %CompilerFlags% w:\toast\code\win32_toast.cpp /link %PlatformLinkerFlags%
popd