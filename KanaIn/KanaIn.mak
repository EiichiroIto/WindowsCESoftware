# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE x86em) Application" 0x0a01
# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (WCE MIPS) Application" 0x0801
# TARGTYPE "Win32 (WCE SH) Application" 0x0901

!IF "$(CFG)" == ""
CFG=KanaIn - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to KanaIn - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "KanaIn - Win32 Release" && "$(CFG)" != "KanaIn - Win32 Debug"\
 && "$(CFG)" != "KanaIn - Win32 (WCE x86em) Release" && "$(CFG)" !=\
 "KanaIn - Win32 (WCE x86em) Debug" && "$(CFG)" !=\
 "KanaIn - Win32 (WCE MIPS) Release" && "$(CFG)" !=\
 "KanaIn - Win32 (WCE MIPS) Debug" && "$(CFG)" !=\
 "KanaIn - Win32 (WCE SH) Release" && "$(CFG)" !=\
 "KanaIn - Win32 (WCE SH) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "KanaIn.mak" CFG="KanaIn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KanaIn - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "KanaIn - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "KanaIn - Win32 (WCE x86em) Release" (based on\
 "Win32 (WCE x86em) Application")
!MESSAGE "KanaIn - Win32 (WCE x86em) Debug" (based on\
 "Win32 (WCE x86em) Application")
!MESSAGE "KanaIn - Win32 (WCE MIPS) Release" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "KanaIn - Win32 (WCE MIPS) Debug" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "KanaIn - Win32 (WCE SH) Release" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE "KanaIn - Win32 (WCE SH) Debug" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "KanaIn - Win32 Debug"

!IF  "$(CFG)" == "KanaIn - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\KanaIn.exe"

CLEAN : 
	-@erase ".\Release\KanaIn.exe"
	-@erase ".\Release\kanain.obj"
	-@erase ".\Release\KanaIn.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/KanaIn.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /machine:I386 /out:"$(OUTDIR)/KanaIn.exe" 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.res"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "KanaIn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "KanaIn__"
# PROP BASE Intermediate_Dir "KanaIn__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "KanaIn__"
# PROP Intermediate_Dir "KanaIn__"
# PROP Target_Dir ""
OUTDIR=.\KanaIn__
INTDIR=.\KanaIn__

ALL : "$(OUTDIR)\KanaIn.exe"

CLEAN : 
	-@erase ".\KanaIn__\vc40.pdb"
	-@erase ".\KanaIn__\vc40.idb"
	-@erase ".\KanaIn__\KanaIn.exe"
	-@erase ".\KanaIn__\kanain.obj"
	-@erase ".\KanaIn__\KanaIn.res"
	-@erase ".\KanaIn__\KanaIn.ilk"
	-@erase ".\KanaIn__\KanaIn.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\KanaIn__/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/KanaIn.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /debug /machine:I386 /out:"$(OUTDIR)/KanaIn.exe" 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.res"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "x86emRel"
# PROP BASE Intermediate_Dir "x86emRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x86emRel"
# PROP Intermediate_Dir "x86emRel"
# PROP Target_Dir ""
OUTDIR=.\x86emRel
INTDIR=.\x86emRel

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\x86emRel\KanaIn.trg"
	-@erase ".\x86emRel\KanaIn.exe"
	-@erase ".\x86emRel\kanain.obj"
	-@erase ".\x86emRel\KanaIn.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE" /D "_X86_" /YX /c
# ADD CPP /nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE" /D "_X86_" /YX /c
CPP_PROJ=/nologo /ML /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT"\
 /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE" /D "_X86_"\
 /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\x86emRel/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "PEGASUS" /d "UNDER_NT" /d "TARGET_NT" /d "NDEBUG"
# ADD RSC /l 0x411 /d "PEGASUS" /d "UNDER_NT" /d "TARGET_NT" /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/KanaIn.res" /d "PEGASUS" /d "UNDER_NT" /d\
 "TARGET_NT" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /machine:I386 /out:"$(OUTDIR)/KanaIn.exe" 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.res"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

EMPFILE=empfile.exe
# ADD BASE EMPFILE -NOSHELL -COPY
# ADD EMPFILE -NOSHELL -COPY
EMPFILE_FLAGS=-NOSHELL -COPY 
EMPFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(EMPFILE_FILES)
    $(EMPFILE) $(EMPFILE_FLAGS) .\x86emRel\KanaIn.exe\
 "$(EMPFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "KanaIn_0"
# PROP BASE Intermediate_Dir "KanaIn_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "KanaIn_0"
# PROP Intermediate_Dir "KanaIn_0"
# PROP Target_Dir ""
OUTDIR=.\KanaIn_0
INTDIR=.\KanaIn_0

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\KanaIn_0\vc40.pdb"
	-@erase ".\KanaIn_0\vc40.idb"
	-@erase ".\KanaIn_0\KanaIn.ilk"
	-@erase ".\KanaIn_0\kanain.obj"
	-@erase ".\KanaIn_0\KanaIn.res"
	-@erase ".\KanaIn_0\KanaIn.pdb"
	-@erase ".\KanaIn_0\KanaIn.trg"
	-@erase ".\KanaIn_0\KanaIn.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE" /D "_X86_" /YX /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE" /D "_X86_" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "STRICT" /D "PEGASUS" /D "UNDER_NT" /D "TARGET_NT" /D "UNICODE" /D "_UNICODE"\
 /D "_X86_" /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\KanaIn_0/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "PEGASUS" /d "UNDER_NT" /d "TARGET_NT" /d "_DEBUG"
# ADD RSC /l 0x411 /d "PEGASUS" /d "UNDER_NT" /d "TARGET_NT" /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/KanaIn.res" /d "PEGASUS" /d "UNDER_NT" /d\
 "TARGET_NT" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /debug /machine:I386 /out:"$(OUTDIR)/KanaIn.exe" 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.res"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

EMPFILE=empfile.exe
# ADD BASE EMPFILE -NOSHELL -COPY
# ADD EMPFILE -NOSHELL -COPY
EMPFILE_FLAGS=-NOSHELL -COPY 
EMPFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(EMPFILE_FILES)
    $(EMPFILE) $(EMPFILE_FLAGS) .\KanaIn_0\KanaIn.exe\
 "$(EMPFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WMIPSRel"
# PROP BASE Intermediate_Dir "WMIPSRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WMIPSRel"
# PROP Intermediate_Dir "WMIPSRel"
# PROP Target_Dir ""
OUTDIR=.\WMIPSRel
INTDIR=.\WMIPSRel

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\WMIPSRel\KanaIn.trg"
	-@erase ".\WMIPSRel\KanaIn.exe"
	-@erase ".\WMIPSRel\kanain.obj"
	-@erase ".\WMIPSRel\KanaIn.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=clmips.exe
# ADD BASE CPP /nologo /W3 /O2 /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D "UNICODE" /YX /QMRPeg /c
# ADD CPP /nologo /W3 /O2 /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D "UNICODE" /YX /QMRPeg /c
CPP_PROJ=/nologo /W3 /O2 /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D\
 "UNICODE" /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /QMRPeg /c 
CPP_OBJS=.\WMIPSRel/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "PEGASUS" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "PEGASUS" /d "NDEBUG"
RSC_PROJ=/l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "MIPS" /d "_MIPS_" /d\
 "PEGASUS" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib libc.lib /nologo /machine:MIPS /subsystem:pegasus
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib libc.lib /nologo /machine:MIPS /subsystem:pegasus
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=commctrl.lib coredll.lib libc.lib /nologo\
 /entry:"WinMainCRTStartup" /base:"0x00010000" /align:"4096" /incremental:no\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /machine:MIPS /out:"$(OUTDIR)/KanaIn.exe"\
 /subsystem:pegasus 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.rsc"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
PFILE_FLAGS=COPY 
PFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WMIPSRel\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

DOWNLOAD : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WMIPSRel\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Debug"

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
OUTDIR=.\WMIPSDbg
INTDIR=.\WMIPSDbg

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\WMIPSDbg\vc40.pdb"
	-@erase ".\WMIPSDbg\KanaIn.ilk"
	-@erase ".\WMIPSDbg\kanain.obj"
	-@erase ".\WMIPSDbg\KanaIn.rsc"
	-@erase ".\WMIPSDbg\KanaIn.pdb"
	-@erase ".\WMIPSDbg\KanaIn.trg"
	-@erase ".\WMIPSDbg\KanaIn.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=clmips.exe
# ADD BASE CPP /nologo /W3 /Zi /Od /D "DEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D "UNICODE" /YX /QMRPeg /c
# ADD CPP /nologo /W3 /Zi /Od /D "DEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D "UNICODE" /YX /QMRPeg /c
CPP_PROJ=/nologo /W3 /Zi /Od /D "DEBUG" /D "MIPS" /D "_MIPS_" /D "PEGASUS" /D\
 "UNICODE" /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /QMRPeg\
 /c 
CPP_OBJS=.\WMIPSDbg/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "PEGASUS" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d "PEGASUS" /d "DEBUG"
RSC_PROJ=/l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "MIPS" /d "_MIPS_" /d\
 "PEGASUS" /d "DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib libcd.lib /nologo /debug /machine:MIPS /subsystem:pegasus
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib libcd.lib /nologo /debug /machine:MIPS /subsystem:pegasus
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=commctrl.lib coredll.lib libcd.lib /nologo\
 /entry:"WinMainCRTStartup" /base:"0x00010000" /align:"4096" /incremental:yes\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /debug /machine:MIPS /out:"$(OUTDIR)/KanaIn.exe"\
 /subsystem:pegasus 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.rsc"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
PFILE_FLAGS=COPY 
PFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WMIPSDbg\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

DOWNLOAD : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WMIPSDbg\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WCESHRel"
# PROP BASE Intermediate_Dir "WCESHRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WCESHRel"
# PROP Intermediate_Dir "WCESHRel"
# PROP Target_Dir ""
OUTDIR=.\WCESHRel
INTDIR=.\WCESHRel

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\WCESHRel\KanaIn.trg"
	-@erase ".\WCESHRel\KanaIn.exe"
	-@erase ".\WCESHRel\kanain.obj"
	-@erase ".\WCESHRel\KanaIn.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=shcl.exe
# ADD BASE CPP /nologo /W3 /O2 /D "NDEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D "UNICODE" /YX /c
# ADD CPP /nologo /W3 /O2 /D "NDEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D "UNICODE" /YX /c
CPP_PROJ=/nologo /W3 /O2 /D "NDEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D\
 "UNICODE" /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WCESHRel/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "PEGASUS" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "PEGASUS" /d "NDEBUG"
RSC_PROJ=/l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "SH3" /d "_SH3_" /d "PEGASUS"\
 /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib libc.lib /nologo /machine:SH3 /subsystem:pegasus
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib libc.lib /nologo /machine:SH3 /subsystem:pegasus
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=commctrl.lib coredll.lib libc.lib /nologo\
 /entry:"WinMainCRTStartup" /base:"0x00010000" /align:"4096" /incremental:no\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /machine:SH3 /out:"$(OUTDIR)/KanaIn.exe"\
 /subsystem:pegasus 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.rsc"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
PFILE_FLAGS=COPY 
PFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WCESHRel\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

DOWNLOAD : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WCESHRel\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Debug"

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
OUTDIR=.\WCESHDbg
INTDIR=.\WCESHDbg

ALL : "$(OUTDIR)\KanaIn.exe" "$(OUTDIR)\KanaIn.trg"

CLEAN : 
	-@erase ".\WCESHDbg\vc40.pdb"
	-@erase ".\WCESHDbg\KanaIn.ilk"
	-@erase ".\WCESHDbg\kanain.obj"
	-@erase ".\WCESHDbg\KanaIn.rsc"
	-@erase ".\WCESHDbg\KanaIn.pdb"
	-@erase ".\WCESHDbg\KanaIn.trg"
	-@erase ".\WCESHDbg\KanaIn.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=shcl.exe
# ADD BASE CPP /nologo /W3 /Zi /Od /D "DEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D "UNICODE" /YX /c
# ADD CPP /nologo /W3 /Zi /Od /D "DEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D "UNICODE" /YX /c
CPP_PROJ=/nologo /W3 /Zi /Od /D "DEBUG" /D "SH3" /D "_SH3_" /D "PEGASUS" /D\
 "UNICODE" /Fp"$(INTDIR)/KanaIn.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WCESHDbg/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "PEGASUS" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "SH3" /d "_SH3_" /d "PEGASUS" /d "DEBUG"
RSC_PROJ=/l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "SH3" /d "_SH3_" /d "PEGASUS"\
 /d "DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/KanaIn.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib libcd.lib /nologo /debug /machine:SH3 /subsystem:pegasus
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib libcd.lib /nologo /debug /machine:SH3 /subsystem:pegasus
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=commctrl.lib coredll.lib libcd.lib /nologo\
 /entry:"WinMainCRTStartup" /base:"0x00010000" /align:"4096" /incremental:yes\
 /pdb:"$(OUTDIR)/KanaIn.pdb" /debug /machine:SH3 /out:"$(OUTDIR)/KanaIn.exe"\
 /subsystem:pegasus 
LINK32_OBJS= \
	"$(INTDIR)/kanain.obj" \
	"$(INTDIR)/KanaIn.rsc"

"$(OUTDIR)\KanaIn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
PFILE_FLAGS=COPY 
PFILE_FILES= \
	"$(OUTDIR)/KanaIn.exe"

"$(OUTDIR)\KanaIn.trg" : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WCESHDbg\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

DOWNLOAD : "$(OUTDIR)" $(PFILE_FILES)
    $(PFILE) $(PFILE_FLAGS) .\WCESHDbg\KanaIn.exe\
 "$(PFILE_DEST)WCE:KanaIn.exe">"$(OUTDIR)\KanaIn.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "KanaIn - Win32 Release"
# Name "KanaIn - Win32 Debug"
# Name "KanaIn - Win32 (WCE x86em) Release"
# Name "KanaIn - Win32 (WCE x86em) Debug"
# Name "KanaIn - Win32 (WCE MIPS) Release"
# Name "KanaIn - Win32 (WCE MIPS) Debug"
# Name "KanaIn - Win32 (WCE SH) Release"
# Name "KanaIn - Win32 (WCE SH) Debug"

!IF  "$(CFG)" == "KanaIn - Win32 Release"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 Debug"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\kanain.c

!IF  "$(CFG)" == "KanaIn - Win32 Release"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 Debug"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Release"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Debug"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Release"

NODEP_CPP_KANAI=\
	".\Itoh"\
	

"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Debug"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Release"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Debug"


"$(INTDIR)\kanain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\KanaIn.rc

!IF  "$(CFG)" == "KanaIn - Win32 Release"


"$(INTDIR)\KanaIn.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 Debug"


"$(INTDIR)\KanaIn.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Release"


"$(INTDIR)\KanaIn.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE x86em) Debug"


"$(INTDIR)\KanaIn.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Release"


"$(INTDIR)\KanaIn.rsc" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "MIPS" /d "_MIPS_" /d\
 "PEGASUS" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE MIPS) Debug"


"$(INTDIR)\KanaIn.rsc" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "MIPS" /d "_MIPS_" /d\
 "PEGASUS" /d "DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Release"


"$(INTDIR)\KanaIn.rsc" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "SH3" /d "_SH3_" /d\
 "PEGASUS" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "KanaIn - Win32 (WCE SH) Debug"


"$(INTDIR)\KanaIn.rsc" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /r /fo"$(INTDIR)/KanaIn.rsc" /d "SH3" /d "_SH3_" /d\
 "PEGASUS" /d "DEBUG" $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
