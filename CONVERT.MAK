PROJ = CONVERT
PROJFILE = CONVERT.MAK
DEBUG = 1

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
BRFLAGS  =  /o $(PROJ).bsc /Es /Em /Iu
BROWSE	= 0
CFLAGS_G  = /AL /W2 /Gw /Zp /BATCH
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
BIND  = bind
RC  = rc
IMPLIB	= implib
LFLAGS_G  =  /NOD /STACK:8000  /BATCH
LFLAGS_D  =  /CO /NOF /NOP
LFLAGS_R  =  /NOF /NOP
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC  = cl
CFLAGS_D  = /Od /Zi /DDEBUG /Ze
CFLAGS_R  = /Os /Og /Oe /Gs
LLIBS_G  = LIBW.LIB llibcew
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi
CVFLAGS  =  /25 /2 /S

DEF_FILE  = CONVERT.DEF
OBJS  = CONVERT.obj GETWRDW2.obj
LIBS_EXT  = ..\..\..\C600\LIB\WPRINTF.LIB
LIBS  = $(LIBS_EXT)
RESS  = CONVERT.res
SBRS  = CONVERT.sbr GETWRDW2.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .res .obj .c .rc

CONVERT.obj : CONVERT.C convert.h wprintf.h convdlg.h

CONVERT.sbr : CONVERT.C convert.h wprintf.h convdlg.h

CONVERT.res : CONVERT.RC convert.h bible4w.dlg convdlg.h

GETWRDW2.obj : GETWRDW2.C convert.h convdlg.h

GETWRDW2.sbr : GETWRDW2.C convert.h convdlg.h


$(PROJ).bsc : $(SBRS)
	$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(DEF_FILE) $(OBJS) $(LIBS) $(RESS)
!IF $(DEBUG)
	$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
	$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
!IF $(DEBUG)
	$(LINKER) @$(PROJ).lrf
!ELSE
	$(LINKER) @$(PROJ).lrf
!ENDIF
	$(RC) -30 $(RESS) $@


.c.sbr :
!IF $(DEBUG)
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.rc.res :
	$(RC) /r $<

.c.obj :
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF


run: $(PROJ).exe
	WIN $(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
	WIN CVW $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
