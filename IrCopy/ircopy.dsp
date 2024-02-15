# Microsoft Developer Studio Project File - Name="IrCopy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (WCE MIPS) Application" 0x0a01
# TARGTYPE "Win32 (WCE SH) Application" 0x0901

CFG=IrCopy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IrCopy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IrCopy.mak" CFG="IrCopy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IrCopy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "IrCopy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "IrCopy - Win32 (WCE MIPS) Release" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "IrCopy - Win32 (WCE SH) Release" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "IrCopy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_IRDA" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG" /d WS_EX_CAPTIONOKBTN=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /YX /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

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
# ADD BASE CPP /nologo /ML /W3 /O2 /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D "_WIN32_WCE" /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /ML /W3 /O2 /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D "_WIN32_WCE" /D "UNICODE" /YX /QMRWCE /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "_WIN32_WCE" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "_WIN32_WCE" /d "UNICODE" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:windowsce
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib winsock.lib /nologo /machine:MIPS /subsystem:windowsce
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

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
# ADD BASE CPP /nologo /ML /W3 /O2 /D "NDEBUG" /D "SH3" /D "_SH3_" /D "_WIN32_WCE" /D "UNICODE" /YX /c
# ADD CPP /nologo /ML /W3 /O2 /D "NDEBUG" /D "SH3" /D "_SH3_" /D "_WIN32_WCE" /D "UNICODE" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "_WIN32_WCE" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "_WIN32_WCE" /d "UNICODE" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:windowsce
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib winsock.lib /nologo /machine:SH3 /subsystem:windowsce
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ENDIF 

# Begin Target

# Name "IrCopy - Win32 Release"
# Name "IrCopy - Win32 Debug"
# Name "IrCopy - Win32 (WCE MIPS) Release"
# Name "IrCopy - Win32 (WCE SH) Release"
# Begin Source File

SOURCE=.\APPICON.ICO
# End Source File
# Begin Source File

SOURCE=.\APPMAIN.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_APPMA=\
	".\appmain.h"\
	".\appsub.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	".\tools.h"\
	{$(INCLUDE)}"af_irda.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_APPMA=\
	".\appmain.h"\
	".\appsub.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	".\tools.h"\
	{$(INCLUDE)}"af_irda.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\APPSUB.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_APPSU=\
	".\appmain.h"\
	".\appsub.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_APPSU=\
	".\appmain.h"\
	".\appsub.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BASEWIN.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_BASEW=\
	".\basewin.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_BASEW=\
	".\basewin.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DIALOG.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_DIALO=\
	".\appmain.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_DIALO=\
	".\appmain.h"\
	".\basewin.h"\
	".\dialog.h"\
	".\list.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IRCOPY.RC

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LIST.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_LIST_=\
	".\list.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_LIST_=\
	".\list.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MAIN.CPP

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_MAIN_=\
	".\appmain.h"\
	".\list.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_MAIN_=\
	".\appmain.h"\
	".\list.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TOOLS1.C

!IF  "$(CFG)" == "IrCopy - Win32 Release"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 Debug"

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE MIPS) Release"

DEP_CPP_TOOLS=\
	".\tools.h"\
	

!ELSEIF  "$(CFG)" == "IrCopy - Win32 (WCE SH) Release"

DEP_CPP_TOOLS=\
	".\tools.h"\
	

!ENDIF 

# End Source File
# End Target
# End Project
