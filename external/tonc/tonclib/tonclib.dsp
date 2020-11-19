# Microsoft Developer Studio Project File - Name="tonclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=tonclib - Win32 Build
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tonclib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tonclib.mak" CFG="tonclib - Win32 Build"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tonclib - Win32 Build" (based on "Win32 (x86) External Target")
!MESSAGE "tonclib - Win32 Clean" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "tonclib - Win32 Build"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make -f tonclib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonclib.exe"
# PROP BASE Bsc_Name "tonclib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make 2>&1 | sed -e 's/\(\w\+\):\([0-9]\+\):/\1(\2):/'"
# PROP Rebuild_Opt "/a"
# PROP Target_File "libtonc.a"
# PROP Bsc_Name "tonclib.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tonclib - Win32 Clean"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make -f tonclib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonclib.exe"
# PROP BASE Bsc_Name "tonclib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make clean"
# PROP Rebuild_Opt "/a"
# PROP Target_File "libtonc.a"
# PROP Bsc_Name "tonclib.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "tonclib - Win32 Build"
# Name "tonclib - Win32 Clean"

!IF  "$(CFG)" == "tonclib - Win32 Build"

!ELSEIF  "$(CFG)" == "tonclib - Win32 Clean"

!ENDIF 

# Begin Group "hdr"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\tonc.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_bios.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_core.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_input.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_irq.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_math.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_memdef.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_memmap.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_nocash.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_oam.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_surface.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_tte.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_types.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_video.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter "*.c"
# Begin Group "tte"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\tte\ase_drawg.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\bmp16_drawg.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\bmp16_drawg_b1cs.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\bmp8_drawg.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\bmp8_drawg_b1cs.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\bmp8_drawg_b1cts_fast.s
# End Source File
# Begin Source File

SOURCE=.\src\tte\chr4c_drawg_b1cts.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\chr4c_drawg_b1cts_fast.s
# End Source File
# Begin Source File

SOURCE=.\src\tte\chr4c_drawg_b4cts.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\chr4c_drawg_b4cts_fast.s
# End Source File
# Begin Source File

SOURCE=.\src\tte\chr4r_drawg_b1cts.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\obj_drawg.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\se_drawg.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_ase.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_bmp.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_chr4c.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_chr4r.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_obj.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_init_se.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_iohook.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_main.c
# End Source File
# Begin Source File

SOURCE=.\src\tte\tte_types.s
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\font\sys8.png
# End Source File
# Begin Source File

SOURCE=.\src\font\sys8.s
# End Source File
# Begin Source File

SOURCE=.\src\font\verdana10.png
# End Source File
# Begin Source File

SOURCE=.\src\font\verdana10.s
# End Source File
# Begin Source File

SOURCE=.\src\font\verdana9.png
# End Source File
# Begin Source File

SOURCE=.\src\font\verdana9.s
# End Source File
# Begin Source File

SOURCE=.\src\font\verdana9_b4.png
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\tonc_bg.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_bg_affine.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_bmp16.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_bmp8.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_color.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_core.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_input.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_irq.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_math.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_oam.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_obj_affine.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_sbmp16.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_sbmp8.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_schr4c.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_schr4r.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_surface.c
# End Source File
# Begin Source File

SOURCE=.\src\tonc_video.c
# End Source File
# End Group
# Begin Group "asm"

# PROP Default_Filter "*.S;*.s"
# Begin Source File

SOURCE=.\asm\clr_blend_fast.s
# End Source File
# Begin Source File

SOURCE=.\asm\clr_fade_fast.s
# End Source File
# Begin Source File

SOURCE=.\asm\div_lut.s
# End Source File
# Begin Source File

SOURCE=.\asm\sin_lut.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_bios.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_bios_ex.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_isr_master.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_isr_nest.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_memcpy.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_memset.s
# End Source File
# Begin Source File

SOURCE=.\asm\tonc_nocash.s
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base.c
# End Source File
# Begin Source File

SOURCE=.\base.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_legacy.h
# End Source File
# Begin Source File

SOURCE=.\include\tonc_libgba.h
# End Source File
# End Group
# Begin Group "pre13"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\pre1.3\tonc_bitmap.c
# End Source File
# Begin Source File

SOURCE=.\src\pre1.3\tonc_text.c
# End Source File
# Begin Source File

SOURCE=.\include\tonc_text.h
# End Source File
# Begin Source File

SOURCE=.\src\pre1.3\tonc_text_bm.c
# End Source File
# Begin Source File

SOURCE=.\src\pre1.3\tonc_text_map.c
# End Source File
# Begin Source File

SOURCE=.\src\pre1.3\tonc_text_oam.c
# End Source File
# Begin Source File

SOURCE=.\src\pre1.3\toncfont.s
# End Source File
# End Group
# Begin Source File

SOURCE=.\libtonc.chm
# End Source File
# Begin Source File

SOURCE=.\libtonc.dox
# End Source File
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# Begin Source File

SOURCE=.\tonclib.txt
# End Source File
# End Target
# End Project
