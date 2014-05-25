del .\bin\i386\stdrv01.sys
del .\objfre_win7_x86\i386\*.obj
del .\bin\i386\stdrv01.pdb
ren .\bin\i386\stdrv01.pdb %random%
set WXPBASE=s:\winddk
ddkbuild.cmd -WXP fre .
