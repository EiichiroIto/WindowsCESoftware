# Microsoft Developer Studio Project File - Name="Train" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (WCE MIPS) Application" 0x0a01
# TARGTYPE "Win32 (WCE SH) Application" 0x0901

CFG=Train - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Train.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Train.mak" CFG="Train - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Train - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Train - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Train - Win32 (WCE MIPS) Release" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "Train - Win32 (WCE MIPS) Debug" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "Train - Win32 (WCE SH) Release" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE "Train - Win32 (WCE SH) Debug" (based on "Win32 (WCE SH) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP WCE_Configuration "H/PC Ver. 1.00"

!IF  "$(CFG)" == "Train - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\kctrl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\kctrl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG" /d WS_EX_CAPTIONOKBTN=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\kctrl\debug\kctrl.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WMIPSRel"
# PROP BASE Intermediate_Dir "WMIPSRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WMIPSRel"
# PROP Intermediate_Dir "WMIPSRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=clmips.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /ML /W3 /O2 /I "..\kctrl" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib ..\kctrl\wmipsrel\kctrl.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WMIPSDbg"
# PROP BASE Intermediate_Dir "WMIPSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WMIPSDbg"
# PROP Intermediate_Dir "WMIPSDbg"
# PROP Target_Dir ""
CPP=clmips.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /MLd /W3 /Zi /Od /I "..\kctrl" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WCESHRel"
# PROP BASE Intermediate_Dir "WCESHRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WCESHRel"
# PROP Intermediate_Dir "WCESHRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=shcl.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /ML /W3 /O2 /I "..\kctrl" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib ..\kctrl\wceshrel\kctrl.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WCESHDbg"
# PROP BASE Intermediate_Dir "WCESHDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WCESHDbg"
# PROP Intermediate_Dir "WCESHDbg"
# PROP Target_Dir ""
CPP=shcl.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /MLd /W3 /Zi /Od /I "..\kctrl" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ENDIF 

# Begin Target

# Name "Train - Win32 Release"
# Name "Train - Win32 Debug"
# Name "Train - Win32 (WCE MIPS) Release"
# Name "Train - Win32 (WCE MIPS) Debug"
# Name "Train - Win32 (WCE SH) Release"
# Name "Train - Win32 (WCE SH) Debug"
# Begin Source File

SOURCE=.\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\APPMAIN.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_APPMA=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	".\appsub.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\graph.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_APPMA=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	".\appsub.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\graph.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\APPSUB.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_APPSU=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	".\appsub.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\graph.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_APPSU=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	".\appsub.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\graph.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BASE.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_BASE_=\
	".\base.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_BASE_=\
	".\base.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BASEWIN.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_BASEW=\
	".\basewin.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_BASEW=\
	".\basewin.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BIGLIST.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_BIGLI=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_BIGLI=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DIALOG.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_DIALO=\
	"..\kctrl\kctrl.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\fileopen.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_DIALO=\
	"..\kctrl\kctrl.h"\
	".\base.h"\
	".\basewin.h"\
	".\controls.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILEOPEN.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_FILEO=\
	".\basewin.h"\
	".\fileopen.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GRAPH.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_GRAPH=\
	".\base.h"\
	".\graph.h"\
	".\lfunc.h"\
	".\pqueue.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_GRAPH=\
	".\base.h"\
	".\graph.h"\
	".\lfunc.h"\
	".\pqueue.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LABEL.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_LABEL=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_LABEL=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LCOMMON.C

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_LCOMM=\
	".\lfunc.h"\
	".\ROMA.C"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_LCOMM=\
	".\lfunc.h"\
	".\ROMA.C"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lineedit.cpp

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_LINEE=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_LINEE=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LWINDOWS.C

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_LWIND=\
	"..\kctrl\kctrl.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_LWIND=\
	"..\kctrl\kctrl.h"\
	".\lfunc.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MAIN.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_MAIN_=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_MAIN_=\
	"..\kctrl\kctrl.h"\
	".\appmain.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.rc

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PQUEUE.CPP

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_PQUEU=\
	".\lfunc.h"\
	".\pqueue.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_PQUEU=\
	".\lfunc.h"\
	".\pqueue.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\timeedit.cpp

!IF  "$(CFG)" == "Train - Win32 Release"

!ELSEIF  "$(CFG)" == "Train - Win32 Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Release"

DEP_CPP_TIMEE=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Release"

DEP_CPP_TIMEE=\
	"..\kctrl\kctrl.h"\
	".\controls.h"\
	

!ELSEIF  "$(CFG)" == "Train - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\train.bmp
# End Source File
# End Target
# End Project
