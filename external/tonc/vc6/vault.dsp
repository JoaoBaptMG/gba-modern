# Microsoft Developer Studio Project File - Name="vault" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=vault - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vault.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vault.mak" CFG="vault - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vault - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "vault - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "vault - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Cmd_Line "NMAKE /f vault.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "vault.exe"
# PROP BASE Bsc_Name "vault.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Cmd_Line "NMAKE /f vault.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "vault.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "vault - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Cmd_Line "NMAKE /f vault.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "vault.exe"
# PROP BASE Bsc_Name "vault.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Cmd_Line "NMAKE /f vault.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "vault.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "vault - Win32 Release"
# Name "vault - Win32 Debug"

!IF  "$(CFG)" == "vault - Win32 Release"

!ELSEIF  "$(CFG)" == "vault - Win32 Debug"

!ENDIF 

# Begin Group "man"

# PROP Default_Filter "*.chm"
# Begin Source File

SOURCE="..\..\..\..\..\man\as-2.17.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\bashref-2.05b.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\binutils-2.15.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\cpp-3.3.2.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\gcc-4.1.2.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\ld-2.17.chm"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\man\make-3.81.chm"
# End Source File
# End Group
# Begin Group "libgba"

# PROP Default_Filter ""
# Begin Group "hdr"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\BoyScout.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\erapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\fade.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\fat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_affine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_base.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_compression.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_console.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_dma.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_input.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_interrupt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_multiboot.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_sio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_sprites.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_systemcalls.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_timers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\gba_video.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\mappy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\mbv2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\pcx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\xcomms.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\include\xcomms_cmd.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\AffineSet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\Compression.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\console.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\CpuSet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\fade.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\input.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\interrupt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\IntrWait.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\mappy_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\mb2print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\mbv2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\pcx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\xcomms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\xcomms_print.c
# End Source File
# End Group
# Begin Group "asm"

# PROP Default_Filter "*.s"
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\ArcTan.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\Div.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\DivArm.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\division.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\InterruptDispatcher.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\MultiBoot.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\Reset.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\Sound.s
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\libgba\src\Sqrt.s
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\devkitARM\base_rules
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\devkitARM\gba_rules
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\devkitPro\examples\gba\template\Makefile
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
