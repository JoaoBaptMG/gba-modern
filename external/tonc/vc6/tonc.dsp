# Microsoft Developer Studio Project File - Name="tonc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=tonc - Win32 Build libtonc
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tonc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tonc.mak" CFG="tonc - Win32 Build libtonc"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tonc - Win32 Build Single" (based on "Win32 (x86) External Target")
!MESSAGE "tonc - Win32 Build All" (based on "Win32 (x86) External Target")
!MESSAGE "tonc - Win32 Build libtonc" (based on "Win32 (x86) External Target")
!MESSAGE "tonc - Win32 Clean Single" (based on "Win32 (x86) External Target")
!MESSAGE "tonc - Win32 Clean All" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "tonc - Win32 Build Single"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonc.exe"
# PROP BASE Bsc_Name "tonc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make -C ../ IDE=msvc"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tonc.exe"
# PROP Bsc_Name "tonc.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tonc - Win32 Build All"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonc.exe"
# PROP BASE Bsc_Name "tonc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make build_all  -C ../"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tonc.exe"
# PROP Bsc_Name "tonc.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tonc - Win32 Build libtonc"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonc.exe"
# PROP BASE Bsc_Name "tonc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make libtonc  -C ../"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tonc.exe"
# PROP Bsc_Name "tonc.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tonc - Win32 Clean Single"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonc.exe"
# PROP BASE Bsc_Name "tonc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make clean  -C ../"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tonc.exe"
# PROP Bsc_Name "tonc.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tonc - Win32 Clean All"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "make"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tonc.exe"
# PROP BASE Bsc_Name "tonc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "make clean_all  -C ../"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tonc.exe"
# PROP Bsc_Name "tonc.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "tonc - Win32 Build Single"
# Name "tonc - Win32 Build All"
# Name "tonc - Win32 Build libtonc"
# Name "tonc - Win32 Clean Single"
# Name "tonc - Win32 Clean All"

!IF  "$(CFG)" == "tonc - Win32 Build Single"

!ELSEIF  "$(CFG)" == "tonc - Win32 Build All"

!ELSEIF  "$(CFG)" == "tonc - Win32 Build libtonc"

!ELSEIF  "$(CFG)" == "tonc - Win32 Clean Single"

!ELSEIF  "$(CFG)" == "tonc - Win32 Clean All"

!ENDIF 

# Begin Group "prehtml"

# PROP Default_Filter "*.htm"
# Begin Group "_preface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\prehtml\intro.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\portal.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\toc.htm
# End Source File
# End Group
# Begin Group "_basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\prehtml\bitmaps.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\first.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\hardware.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\keys.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\objbg.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\regbg.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\regobj.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\setup.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\video.htm
# End Source File
# End Group
# Begin Group "_ext"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\prehtml\affbg.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\affine.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\affobj.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\dma.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\gfx.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\interrupts.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\sndsqr.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\swi.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\timers.htm
# End Source File
# End Group
# Begin Group "_adv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\prehtml\asm.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\lab.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\mode7.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\mode7ex.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\text.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\tte.htm
# End Source File
# End Group
# Begin Group "_appx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\prehtml\edmake.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\fixed.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\log.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\makefile.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\matrix.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\numbers.htm
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\refs.htm
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\prehtml\base.htm
# End Source File
# Begin Source File

SOURCE="..\..\text\tonc-text.txt"
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\tonc.css
# End Source File
# Begin Source File

SOURCE=..\..\prehtml\tonc.js
# End Source File
# End Group
# Begin Group "code"

# PROP Default_Filter ""
# Begin Group "basic"

# PROP Default_Filter ""
# Begin Group "first"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\first\first.bat
# End Source File
# Begin Source File

SOURCE=..\basic\first\first.c
# End Source File
# Begin Source File

SOURCE=..\basic\first\first.gba
# End Source File
# Begin Source File

SOURCE=..\basic\first\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\first\makefile.mak
# End Source File
# Begin Source File

SOURCE=..\basic\first\makefile2.mak
# End Source File
# End Group
# Begin Group "second"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\second\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\second\second.c
# End Source File
# Begin Source File

SOURCE=..\basic\second\second.gba
# End Source File
# Begin Source File

SOURCE=..\basic\second\toolbox.h
# End Source File
# End Group
# Begin Group "bm_modes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\bm_modes\bm_modes.c
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\bm_modes.gba
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\modes.c
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\modes.h
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\bm_modes\toolbox.h
# End Source File
# End Group
# Begin Group "pageflip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\pageflip\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\page_pic.c
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\page_pic.h
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\pageflip.c
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\pageflip.gba
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\pageflip\toolbox.h
# End Source File
# End Group
# Begin Group "key_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\key_demo\gba_pic.c
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\gba_pic.h
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\input.h
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\key_demo.c
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\key_demo.gba
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\key_demo\toolbox.h
# End Source File
# End Group
# Begin Group "obj_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\obj_demo\input.h
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\memdef.h
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\memmap.h
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\metr.c
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\metr.h
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\obj_demo.c
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\obj_demo.gba
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\toolbox.h
# End Source File
# Begin Source File

SOURCE=..\basic\obj_demo\types.h
# End Source File
# End Group
# Begin Group "brin_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\brin_demo\brin.c
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\brin.h
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\brin_demo.c
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\brin_demo.gba
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\input.h
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\brin_demo\toolbox.h
# End Source File
# End Group
# Begin Group "cbb_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\cbb_demo\cbb_demo.c
# End Source File
# Begin Source File

SOURCE=..\basic\cbb_demo\cbb_demo.gba
# End Source File
# Begin Source File

SOURCE=..\basic\cbb_demo\cbb_ids.c
# End Source File
# Begin Source File

SOURCE=..\basic\cbb_demo\cbb_ids.h
# End Source File
# Begin Source File

SOURCE=..\basic\cbb_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\cbb_demo\toolbox.h
# End Source File
# End Group
# Begin Group "sbb_reg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\sbb_reg\input.h
# End Source File
# Begin Source File

SOURCE=..\basic\sbb_reg\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\sbb_reg\sbb_reg.c
# End Source File
# Begin Source File

SOURCE=..\basic\sbb_reg\sbb_reg.gba
# End Source File
# Begin Source File

SOURCE=..\basic\sbb_reg\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\sbb_reg\toolbox.h
# End Source File
# End Group
# Begin Group "hello"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\hello\hello.c
# End Source File
# Begin Source File

SOURCE=..\basic\hello\hello.gba
# End Source File
# Begin Source File

SOURCE=..\basic\hello\Makefile
# End Source File
# End Group
# Begin Group "m3_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\basic\m3_demo\m3_demo.c
# End Source File
# Begin Source File

SOURCE=..\basic\m3_demo\m3_demo.gba
# End Source File
# Begin Source File

SOURCE=..\basic\m3_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\basic\m3_demo\toolbox.c
# End Source File
# Begin Source File

SOURCE=..\basic\m3_demo\toolbox.h
# End Source File
# End Group
# End Group
# Begin Group "ext"

# PROP Default_Filter ""
# Begin Group "obj_aff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\obj_aff\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\obj_aff\metr.c
# End Source File
# Begin Source File

SOURCE=..\ext\obj_aff\metr.h
# End Source File
# Begin Source File

SOURCE=..\ext\obj_aff\obj_aff.c
# End Source File
# Begin Source File

SOURCE=..\ext\obj_aff\obj_aff.gba
# End Source File
# End Group
# Begin Group "oacombo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\oacombo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\oacombo\oac_gfx.c
# End Source File
# Begin Source File

SOURCE=..\ext\oacombo\oac_gfx.h
# End Source File
# Begin Source File

SOURCE=..\ext\oacombo\oacombo.c
# End Source File
# Begin Source File

SOURCE=..\ext\oacombo\oacombo.gba
# End Source File
# Begin Source File

SOURCE=..\ext\oacombo\oe_rs_ex.s
# End Source File
# End Group
# Begin Group "sbb_aff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\sbb_aff\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\sbb_aff\nums.c
# End Source File
# Begin Source File

SOURCE=..\ext\sbb_aff\nums.h
# End Source File
# Begin Source File

SOURCE=..\ext\sbb_aff\sbb_aff.c
# End Source File
# Begin Source File

SOURCE=..\ext\sbb_aff\sbb_aff.gba
# End Source File
# End Group
# Begin Group "mos_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\mos_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\mos_demo\metr.c
# End Source File
# Begin Source File

SOURCE=..\ext\mos_demo\metr.h
# End Source File
# Begin Source File

SOURCE=..\ext\mos_demo\mos_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\mos_demo\mos_demo.gba
# End Source File
# End Group
# Begin Group "bld_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\bld_demo\bld_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\bld_demo\bld_demo.gba
# End Source File
# Begin Source File

SOURCE=..\ext\bld_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\bld_demo\metr.c
# End Source File
# Begin Source File

SOURCE=..\ext\bld_demo\metr.h
# End Source File
# End Group
# Begin Group "win_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\win_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\win_demo\rocket.c
# End Source File
# Begin Source File

SOURCE=..\ext\win_demo\rocket.h
# End Source File
# Begin Source File

SOURCE=..\ext\win_demo\rocket.png
# End Source File
# Begin Source File

SOURCE=..\ext\win_demo\win_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\win_demo\win_demo.gba
# End Source File
# End Group
# Begin Group "dma_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\dma_demo\brin.c
# End Source File
# Begin Source File

SOURCE=..\ext\dma_demo\brin.h
# End Source File
# Begin Source File

SOURCE=..\ext\dma_demo\dma_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\dma_demo\dma_demo.gba
# End Source File
# Begin Source File

SOURCE=..\ext\dma_demo\Makefile
# End Source File
# End Group
# Begin Group "tmr_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\tmr_demo\berk.c
# End Source File
# Begin Source File

SOURCE=..\ext\tmr_demo\berk.h
# End Source File
# Begin Source File

SOURCE=..\ext\tmr_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\tmr_demo\tmr_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\tmr_demo\tmr_demo.gba
# End Source File
# End Group
# Begin Group "irq_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\irq_demo\irq_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\irq_demo\irq_demo.gba
# End Source File
# Begin Source File

SOURCE=..\ext\irq_demo\isr.iwram.c
# End Source File
# Begin Source File

SOURCE=..\ext\irq_demo\Makefile
# End Source File
# End Group
# Begin Group "swi_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\swi_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\swi_demo\swi_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\swi_demo\swi_demo.gba
# End Source File
# End Group
# Begin Group "swi_vsync"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\swi_vsync\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\swi_vsync\metr.c
# End Source File
# Begin Source File

SOURCE=..\ext\swi_vsync\metr.h
# End Source File
# Begin Source File

SOURCE=..\ext\swi_vsync\swi_vsync.c
# End Source File
# Begin Source File

SOURCE=..\ext\swi_vsync\swi_vsync.gba
# End Source File
# End Group
# Begin Group "snd1_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\snd1_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\ext\snd1_demo\snd1_demo.c
# End Source File
# Begin Source File

SOURCE=..\ext\snd1_demo\snd1_demo.gba
# End Source File
# End Group
# End Group
# Begin Group "adv"

# PROP Default_Filter ""
# Begin Group "txt_bm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\txt_bm\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\txt_bm\txt_bm.c
# End Source File
# Begin Source File

SOURCE=..\adv\txt_bm\txt_bm.gba
# End Source File
# End Group
# Begin Group "txt_obj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\txt_obj\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\txt_obj\txt_obj.c
# End Source File
# Begin Source File

SOURCE=..\adv\txt_obj\txt_obj.gba
# End Source File
# End Group
# Begin Group "txt_se1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\txt_se1\border.c
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se1\border.h
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se1\border.png
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se1\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se1\txt_se1.c
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se1\txt_se1.gba
# End Source File
# End Group
# Begin Group "txt_se2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\txt_se2\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se2\txt_se2.c
# End Source File
# Begin Source File

SOURCE=..\adv\txt_se2\txt_se2.gba
# End Source File
# End Group
# Begin Group "m7_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\m7_demo\gfxmake
# End Source File
# Begin Source File

SOURCE=..\adv\m7_demo\source\m7_demo.c
# End Source File
# Begin Source File

SOURCE=..\adv\m7_demo\m7_demo.gba
# End Source File
# Begin Source File

SOURCE=..\adv\m7_demo\source\m7_demo.h
# End Source File
# Begin Source File

SOURCE=..\adv\m7_demo\source\m7_isrs.iwram.c
# End Source File
# Begin Source File

SOURCE=..\adv\m7_demo\Makefile
# End Source File
# End Group
# Begin Group "m7_ex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\m7_ex\build\all_gfx.h
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\gfxmake
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\source\m7_ex.c
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\m7_ex.gba
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\source\mode7.c
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\source\mode7.h
# End Source File
# Begin Source File

SOURCE=..\adv\m7_ex\source\mode7.iwram.c
# End Source File
# End Group
# Begin Group "tte_demo"

# PROP Default_Filter ""
# Begin Group "fonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\tte_demo\fonts\cyber16.h
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\fonts\fonts.h
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\fonts\mssans9.h
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\fonts\tahoma9.h
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\fonts\verdana11.h
# End Source File
# End Group
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\tte_demo\source\main.c
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\source\menu.c
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\include\menu.h
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\source\slideshow.c
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\source\tte_profile.c
# End Source File
# End Group
# Begin Group "grit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\adv\tte_demo\gfx\dlgbox.grit
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\gfx\dungeon01.grit
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\gfx\menu_gfx.grit
# End Source File
# End Group
# Begin Source File

SOURCE=..\adv\tte_demo\gfxmake
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\tte_demo.gba
# End Source File
# Begin Source File

SOURCE=..\adv\tte_demo\build\tte_demo.map
# End Source File
# End Group
# End Group
# Begin Group "lab"

# PROP Default_Filter ""
# Begin Group "octtest"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lab\octtest\Makefile
# End Source File
# Begin Source File

SOURCE=..\lab\octtest\octants.h
# End Source File
# Begin Source File

SOURCE=..\lab\octtest\octants.s
# End Source File
# Begin Source File

SOURCE=..\lab\octtest\octtest.c
# End Source File
# Begin Source File

SOURCE=..\lab\octtest\octtest.gba
# End Source File
# End Group
# Begin Group "bigmap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lab\bigmap\bigmap.c
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\bigmap.gba
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\kakariko.h
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\kakariko.s
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\link.c
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\link.h
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\link_gfx.h
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\link_gfx.s
# End Source File
# Begin Source File

SOURCE=..\lab\bigmap\Makefile
# End Source File
# End Group
# Begin Group "prio_demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lab\prio_demo\id_sort_shell2.s
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\Makefile
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio.iwram.c
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_bg.c
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_bg.h
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_demo.c
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_demo.gba
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_gfx.c
# End Source File
# Begin Source File

SOURCE=..\lab\prio_demo\prio_gfx.h
# End Source File
# End Group
# Begin Group "template"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lab\template\Makefile
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE="..\tonc-code.txt"
# End Source File
# Begin Source File

SOURCE=..\tonc_rules
# End Source File
# End Group
# Begin Source File

SOURCE=..\Makefile
# End Source File
# Begin Source File

SOURCE=..\..\summary.txt
# End Source File
# Begin Source File

SOURCE="..\..\bin\tonc-bin.txt"
# End Source File
# Begin Source File

SOURCE=..\tonc.mak
# End Source File
# Begin Source File

SOURCE=..\..\tonc.txt
# End Source File
# End Target
# End Project
