#pragma message( "Compiling " __FILE__ " on " __DATE__ " at " __TIME__ )
#pragma message( "File last modified on " __TIMESTAMP__ )
#pragma message( "  ")
#pragma title( "Bible for Windows convert program 1.0" )
#pragma subtitle( "Main program module" )
#pragma check_stack()
#pragma comment( compiler )
#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ ".  Last modified on " __TIMESTAMP__ )
#define APPNAME "Convert"

/***************************************************************************
 *									   *
 *  PROGRAM	: convert.c						   *
 *									   *
 *  PURPOSE	: To convert the Bible for Windows data base file from the *
 *		  original hash table based format to an alphabetical word *
 *		  order so that a binary search algorithm can be used.	   *
 *									   *
 *  FUNCTIONS	: WinMain()	      - Calls the initialization function  *
 *					and enters the message loop.	   *
 *									   *
 *		  Bible4wInit()       - Registers the app. window class.   *
 *									   *
 *									   *
 ***************************************************************************/

		  /*  + + + + +  Includes  + + + + +  */

#include <windows.h>
//#include <windowsx.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <search.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>
#include "convert.h"
#include "wprintf.h"

		   /*  + + + + +  Macros  + + + + +  */

		  /*  + + + + +  Functions  + + + + +  */

BOOL FAR PASCAL Bible4wInit(HANDLE, HANDLE);
void NEAR GETOC ( long int , long int* );
void NEAR GETWL ( long int );
void FAR FNDWRD ( int* );
void FAR OpenBible ( HWND );
void FAR MakeHelpPathName(char*);  /* Function deriving help file path */
void FAR OutOfMem ( void );
short FAR BWError(HWND hwnd, WORD bFlags, WORD quit, WORD id, ...);
VOID FAR wexit ( int );
VOID FAR OPEN_KJB ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_CLIP ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_WORDS1 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_WORDS2 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_WORDSp1 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_WORDSp2 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
VOID FAR OPEN_ListSizes ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode );
int fCompareWords( unsigned *elem1, unsigned *elem2 );


		/*  + + + + +  Static Globals  + + + + +  */

BOOL	fDebug = FALSE;


HANDLE	 hInst;
HWND	 hWnd;
long int ID;
GLOBALHANDLE hiTmp     = NULL;
int FAR *lpTmp;
GLOBALHANDLE hOldNumber     = NULL;
int FAR *lpOldNumber;
GLOBALHANDLE hPoNewWords1     = NULL;
unsigned int FAR *lpPoNewWords1;
GLOBALHANDLE hPoNewWords2     = NULL;
unsigned int FAR *lpPoNewWords2;
GLOBALHANDLE hNewWords1     = NULL;
char FAR *lpNewWords1;
GLOBALHANDLE hNewWords2     = NULL;
char FAR *lpNewWords2;
GLOBALHANDLE hStartOfVerse     = NULL;
long int huge *hplStartOfVerse;
GLOBALHANDLE hXRefPointers     = NULL;
long int huge *hplXRefPointers;

char    szDatabaseName[120];


OFSTRUCT ofh1;
OFSTRUCT ofKJB;
OFSTRUCT ofCLIP;
OFSTRUCT ofWords1;
OFSTRUCT ofWords2;
OFSTRUCT oflpWords1;
OFSTRUCT oflpWords2;
OFSTRUCT oflpListSizes;

char szHelpFileName[EXE_NAME_MAX_SIZE+1];    /* Help file name*/

unsigned char IWL[maxwrd_dim];
//char a[maxlin_dim];		  what was this used for ????
char AWORD[33];
//char ALN[82]; 		  what was this used for ????
//char huge (*hpWORDS)[ndimw_dim][nlenw_dim];
char huge *hpWORDS;
GLOBALHANDLE hWORDS;
//char FNAME[11];		  what was this used for ????
HFILE IUNIT10;
HFILE IUNIT11;
HFILE h1;
HFILE IUNITWords1;
HFILE IUNITWords2;
HFILE IUNITWordsp1;
HFILE IUNITWordsp2;
HFILE IUNITListSizes;
int I2WRD , IBSHI , IBSLOW , I2WRDt;
//long int LWS[257];		  what was this used for ????
//long int huge (*hpIWS)[maxwrd_dim];
long int huge *hpIWS;
GLOBALHANDLE hIWS;
int ITVERS , IVERS , ICHAP;
// unsigned char LWL[257] , NWL;  what was this used for ????

long int IR,IVSG,IVS,IVE,IVW;
unsigned char ICAP;
unsigned char GOODNA;

long int LAWORD , LASTWD , LASTSB , NHASH;
long int IER;
int I2ZERO;
long int IADSOV;
long int LASTAD , IRECIN , IRMOD;
int I2IX[257];		      // buffer used by getwrd routines
long int LASTAD2 , LASTAD3 , IRECIN2 , IRMOD2;
int I2IX22[257];		// buffer used by getwrd routines

long int IBASVR;
typedef struct tagVERSEREFS
 {
	     long int VerseStart;
	     long int VerseEnd;
	     int  VerseNum;
	     BOOL picked;
 } VERSEREFS;

HDC hdc;
RECT rc;
LPSTR lpBuffer;

union chps
{
    int I2AC[2402];
    long int IADCHP[1201];
}  chap;
union bks
{
    int I2BK[136];
    long int IADBOK[68];
}  book;
unsigned int unBook2Chap[68];

long int LASTCH;

long int IADIN , IADOFF;
int I2WORD;
int I2REF;
int I2WOLD;
int I2NREF;

//  unsigned char LV[2401];	  what was this for ???????

int I2VER;
union ptts
{
    int I2POI[2];
    long int IPOINT;
}  ipoint;

union lohi
{
    int I2HILO[2];
    long int I4HILO;
}  hilo;

/****************************************************************************
 *									    *
 *  FUNCTION   : WinMain(HANDLE, HANDLE, LPSTR, int)			    *
 *									    *
 *  PURPOSE    : Creates the main app. window, calls an initialization	    *
 *		 function and enters the message loop.			    *
 *									    *
 ****************************************************************************/
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow)
{
    MSG   msg;
    HANDLE hAccel;

/* Save the Instance handel */

    hInst = hInstance;

#ifdef DEBUG
    h1 = OpenFile("debug.txt",&ofh1,OF_READWRITE | OF_CREATE
						 | OF_SHARE_DENY_NONE );
    fDebug = TRUE;
    dprintf("%C");
#endif

/* Register main window class if this is the first instance of the app. */

    if ( !Bible4wInit (hInstance,hPrevInstance ) ) return NULL;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    while (GetMessage (&msg, NULL, NULL, NULL))
    {
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
    }
    return((int)msg.wParam);
}

/****************************************************************************
 *									    *
 *  FUNCTION   : About (hDlg, message, wParam, lParam)			    *
 *									    *
 *  PURPOSE    : Dialog function for the About Bible4w... dialog.	    *
 *									    *
 ****************************************************************************/
BOOL CALLBACK _export About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_INITDIALOG:
	    return(TRUE);
	    break;

	case WM_COMMAND:
	    switch (wParam)
	    {
	    case IDOK:
	    case IDCANCEL:
		EndDialog(hDlg,NULL);
		return(TRUE);
		break;

	    }
	    break;

    }
    return(FALSE);
}

/****************************************************************************
 *									    *
 *  FUNCTION   : Bible4WndProc (hWnd, message, wParam, lParam)		    *
 *									    *
 *  PURPOSE    : Window function for the main app. window. Processes all the*
 *		 menu selections and other messages.			    *
 *									    *
 ****************************************************************************/
#define Frame_DefProc   DefWindowProc
LRESULT CALLBACK _export Bible4WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DLGPROC lpProc;
    HDC hdc;



    switch (message){
	case WM_COMMAND:
	    switch (wParam){
		 case IDM_EXIT:
		   DestroyWindow (hWnd);
		   break;

		 case IDM_ABOUT:
		   /* Bring up the about dialog box */
		   lpProc = (DLGPROC)MakeProcInstance ((FARPROC)About, hInst);
		   DialogBox (hInst,
			      "AboutBox",
			      hWnd,
			      lpProc);
		   FreeProcInstance ((FARPROC)lpProc);
		   break;

	    }
	    break;

       // case WM_PAINT:
	    // Bible4wPaint ( hWnd );
	    // break;

	case WM_CREATE:
	    OpenBible ( hWnd );
	    break;

	case WM_DESTROY:
	    PostQuitMessage (0);
	    break;

	default:
	    return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return(NULL);
}

VOID FAR OpenBible ( hWndpar )
HWND hWndpar;
{
long int LOCTOC,LOCTOC2,LOCWL,I,ioutadd,ioutadd2,IAD,savevrs[513],iNewIADOFF;
UINT iSizeWord1, iSizeWord2, iSizeWordp1, iSizeWordp2;
long int IS1 , IS2 , IO , IOS , iSize1 , iSize2;
char lpszNextBufLoc[55], cFirstChar, szFirstChar[2];
int ILAST , iTotalNumber , ixword , iFirstd,is,ie,ibok,ich,ivr;
int iOldID;
int iii,lastverse,loadvrs,iNumRefs,iNextRef,iNextXref;
DWORD dwX;
#define NUMWORDS 13857


	if ( hiTmp != NULL ) GlobalFree ( hiTmp );
	if ( ( hiTmp = GlobalAlloc ( GMEM_MOVEABLE ,
			 sizeof ( int ) * NUMWORDS ) ) == NULL ) OutOfMem ();
	lpTmp = (int FAR*)GlobalLock ( hiTmp );

	if ( hOldNumber != NULL ) GlobalFree ( hOldNumber );
	if ( ( hOldNumber = GlobalAlloc ( GMEM_MOVEABLE ,
			   sizeof ( int ) * 25000 ) ) == NULL ) OutOfMem ();
	lpOldNumber = (int FAR*)GlobalLock ( hOldNumber );

	NHASH = 0;
	I2ZERO = 0;
//	  LASTVR = 0;		  what was this used for ????
	LASTCH = 0;
	LASTAD = 10;
	ITVERS = 0;
	IRMOD = 0;
	IRECIN = -1;
	IRMOD2 = 0;
	IRECIN2 = -1;
	ID = 0;

	for ( I = 1 ; I <= MAXWRD ; I ++ ) IWL[I] = 0;

	LASTWD = 0;
	LASTSB = 1;

	OPEN_CLIP ( &IUNIT11 , szHelpFileName , OF_READWRITE | OF_CREATE
						      | OF_SHARE_DENY_NONE );
	OPEN_WORDS1 ( &IUNITWords1 , "words1.dat" , OF_READWRITE | OF_CREATE
						      | OF_SHARE_DENY_NONE );
	OPEN_WORDS2 ( &IUNITWords2 , "words2.dat" , OF_READWRITE | OF_CREATE
						      | OF_SHARE_DENY_NONE );
	OPEN_WORDSp1 ( &IUNITWordsp1 , "wordsp1.dat" , OF_READWRITE | OF_CREATE
						      | OF_SHARE_DENY_NONE );
	OPEN_WORDSp2 ( &IUNITWordsp2 , "wordsp2.dat" , OF_READWRITE | OF_CREATE
                                                      | OF_SHARE_DENY_NONE );
        GetProfileString("Bible4W", "DatabaseName", "KJB2p0.dat",
                                   szDatabaseName, sizeof(szDatabaseName) ) ;
        OPEN_KJB ( &IUNIT10 , szDatabaseName , OF_EXIST );
        OPEN_KJB ( &IUNIT10 , szDatabaseName , OF_READ
                                            | OF_SHARE_DENY_WRITE );

  //      OPEN_KJB ( &IUNIT10 , "KJB2P0.DAT" , OF_EXIST );
  //      OPEN_KJB ( &IUNIT10 , "KJB2P0.DAT" , OF_READ
  //                                          | OF_SHARE_DENY_WRITE );

	LASTAD = 1;
	LASTAD2 = 1;
	CHGWRD ( lpszNextBufLoc , 6 , LASTAD );
	lpszNextBufLoc[6] = '\000';
	dprintf ( "Bible text database version = %s\n" , lpszNextBufLoc );
	LASTAD3 = 1;
	CHPWRD ( "xBIB3P0" , 6 , LASTAD3 );
	LASTAD3 = LASTAD = 4;
	I4GWRD ( &LOCTOC , LASTAD );
	I4PWRD ( 12L , LASTAD3 );
	LASTAD3 = LASTAD = 6;
	I4GWRD ( &LOCWL , LASTAD );
	I4PWRD ( 0L , LASTAD3 );
	LASTAD3 = LASTAD = 8;
	I4GWRD ( &IADOFF , LASTAD );
	I4PWRD ( 0L , LASTAD3 );

	LASTAD3 = 12;
	GETOC ( LOCTOC , &LASTAD3 );
	IADSOV = LASTAD3;
	dprintf ( " IADSOV = %li\n" , IADSOV );

	GETWL ( LOCWL );

	LASTAD3 = 10;
	I4PWRD ( IADSOV , LASTAD3 );
	dprintf ( "2 IADSOV = %li\n" , IADSOV );

	ID = 0;
	I2NREF = (int)chap.IADCHP[(int)book.IADBOK[67]-1];
	ICAP = FALSE;
	IR = IVSG = IVS = IVE = IVW = 0;

	hpIWS = ( long int huge * )LockResource ( hIWS );
	hpWORDS = ( char huge * )LockResource ( hWORDS );

	I2WRD = 9;
	IS1 = *(hpIWS+I2WRD);
	_fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 , IWL[I2WRD] );
	lpszNextBufLoc[IWL[I2WRD]] = '\000' ;
	dprintf ( "word %i = %s\n" , I2WRD , lpszNextBufLoc );

	iTotalNumber = 1;
	for ( I2WRD = 1 ; I2WRD < 25000 ; I2WRD++ )
	{
	   if ( IWL[I2WRD] == 0 ) continue;
  /*	   switch (I2WRD)
	   {
	      case 33:
	      case 40:
	      case 41:
	      case 44:
	      case 46:
	      case 58:
	      case 59:
	      case 63:
		continue;

	      default:
		break;
	   }
   */
	   *(lpTmp+iTotalNumber) = I2WRD;
	   iTotalNumber++;
	   IS1 = *(hpIWS+I2WRD);
	   if ( IS1 == 0 )
	   {
	      dprintf ( " bad pointer in IWS array!... I2WRD = %i\n" , I2WRD );
	   }
	   ILAST = I2WRD;
  //	     if ( I2WRD < 15 )
  //	     {
  //		_fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 , IWL[I2WRD] );
  //		lpszNextBufLoc[IWL[I2WRD]] = '\000' ;
  //		dprintf ( "word %i = %s\n" , I2WRD , lpszNextBufLoc );
  //	     }
	}
	I2WRD = ILAST;
	IS1 = *(hpIWS+I2WRD);
	_fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 , IWL[I2WRD] );
	lpszNextBufLoc[IWL[I2WRD]] = '\000' ;
	dprintf ( "word %i = %s\n" , I2WRD , lpszNextBufLoc );
	dprintf ( "Total number of words in list = %i\n" , iTotalNumber );

	if ( iTotalNumber > NUMWORDS )
	{
	    dprintf ( " Error ... iTotalNumber > NUMWORDS !!!!\n" );
	    return;
	}
	qsort( (void FAR*)((LPWORD)lpTmp+8), (size_t)(iTotalNumber-8),
					      sizeof( int ), fCompareWords );

	for ( I2WRD = 1 ; I2WRD < iTotalNumber ; I2WRD++ )
	{
	   if ( IWL[*(lpTmp+I2WRD)] == 0 )
	   {
	    dprintf ( " Error ... IWL[*(lpTmp+I2WRD)] == 0 !!!!\n" );
	    return;
	   }
	   IS1 = *(hpIWS+*(lpTmp+I2WRD));
	   if ( IS1 == 0 )
	   {
	      dprintf ( " Error : bad pointer in IWS array!... I2WRD = %i\n"
								   , I2WRD );
	   }
	   *(lpOldNumber+*(lpTmp+I2WRD)) = I2WRD;
	   if ( I2WRD < 30 || I2WRD > (iTotalNumber-1) || I2WRD ==
				 ( iTotalNumber / 2 ) )
	   {
	      _fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 ,
						       IWL[*(lpTmp+I2WRD)] );
	      lpszNextBufLoc[IWL[*(lpTmp+I2WRD)]] = '\000' ;
	      ixword = *(lpTmp+I2WRD);
	      dprintf ( "old word %i = new word %i = %s\n" , ixword , I2WRD ,
						   lpszNextBufLoc  );
	   }
	}
    // this is for info only...       IS1 = *(hpIWS+*(lpTmp+I2WRD));

    //	  cFirstChar = tolower ( *( hpIWS+ *(lpTmp+iTotalNumber/2) ) );
	I2WRD = *(lpTmp+iTotalNumber/2);
	cFirstChar = ( *(hpWORDS + *( hpIWS+ *(lpTmp+iTotalNumber/2) ) ) );
	cFirstChar = tolower ( cFirstChar );
	szFirstChar[0] = cFirstChar;
	szFirstChar[1] = '\000';
	for ( I2WRD = 0 ; I2WRD < iTotalNumber ; I2WRD++ )
	{
	   IS1 = *(hpIWS+*(lpTmp+I2WRD));
	   if ( *(hpWORDS+IS1) == cFirstChar ||
		*(hpWORDS+IS1) == toupper ( cFirstChar ) )
	   {
	       iFirstd = I2WRD;
	       dprintf ( "first word starting with %s = %i\n" , szFirstChar
								 , iFirstd );
	       break;
	   }
	}

	iSizeWord1 = 0;
	for ( I2WRD = 0 ; I2WRD < iFirstd ; I2WRD++ )
	{
	   iSizeWord1 += IWL[*(lpTmp+I2WRD)];
	}
	dprintf ( "iSizeWord1 = %u\n" , iSizeWord1 );

	iSizeWord2 = 0;
	for ( I2WRD = iFirstd ; I2WRD < iTotalNumber ; I2WRD++ )
	{
	   iSizeWord2 += IWL[*(lpTmp+I2WRD)];
	}
	dprintf ( "iSizeWord2 = %u\n" , iSizeWord2 );

	if ( hNewWords1 != NULL ) GlobalFree ( hNewWords1 );
	dprintf ( "Size of GlobalAlloc request = %li\n" ,
					   ( dwX = (long int)iSizeWord1  ) );
	if ( ( hNewWords1 = GlobalAlloc ( GMEM_MOVEABLE ,
				      dwX  ) ) == NULL ) OutOfMem ();
	dprintf ( "Size of word list1 = %li\n" , GlobalSize ( hNewWords1 ) );
	lpNewWords1 = (char FAR*)GlobalLock ( hNewWords1 );

	if ( hNewWords2 != NULL ) GlobalFree ( hNewWords2 );
	dprintf ( "Size of GlobalAlloc request = %li\n" ,
					   ( dwX = (long int)iSizeWord2  ) );
	if ( ( hNewWords2 = GlobalAlloc ( GMEM_MOVEABLE ,
				      dwX ) ) == NULL ) OutOfMem ();
	dprintf ( "Size of word list2 = %li\n" , GlobalSize ( hNewWords2 ) );
	lpNewWords2 = (char FAR*)GlobalLock ( hNewWords2 );

	if ( hPoNewWords1 != NULL ) GlobalFree ( hPoNewWords1 );
	dprintf ( "Size of GlobalAlloc request = %u\n" ,
	     ( iSizeWordp1 = (UINT)dwX = ( (long int)iFirstd + 1L ) * 2L ) );
	if ( ( hPoNewWords1 = GlobalAlloc ( GMEM_MOVEABLE ,
				      dwX  ) ) == NULL ) OutOfMem ();
	dprintf ( "Size of word pointer list1 = %li\n" ,
					       GlobalSize ( hPoNewWords1 ) );
	lpPoNewWords1 = (unsigned int*)GlobalLock ( hPoNewWords1 );

	if ( hPoNewWords2 != NULL ) GlobalFree ( hPoNewWords2 );
	dprintf ( "Size of GlobalAlloc request = %u\n" , ( iSizeWordp2 =
							      (UINT)dwX =
		    ( (long int)NUMWORDS - (long int)iFirstd + 1L ) * 2L ) );
	if ( ( hPoNewWords2 = GlobalAlloc ( GMEM_MOVEABLE ,
				      dwX ) ) == NULL ) OutOfMem ();
	dprintf ( "Size of word pointer list2 = %li\n" ,
					       GlobalSize ( hPoNewWords2 ) );
	lpPoNewWords2 = (unsigned int*)GlobalLock ( hPoNewWords2 );

	iSize1 = 0;
	for ( I2WRD = 0 ; I2WRD < iFirstd ; I2WRD++ )
	{
	   IS1 = *(hpIWS+*(lpTmp+I2WRD));
	   *(lpPoNewWords1 + I2WRD ) = iSize1;
	   for ( iii = 0 ; iii < IWL[*(lpTmp+I2WRD)] ; iii++ )
	   {
	       *(lpNewWords1+iSize1+iii) = *(hpWORDS+IS1+iii);
	   }
	   iSize1 += IWL[*(lpTmp+I2WRD)];
	}
	*(lpPoNewWords1 + iFirstd ) = iSize1;
	dprintf ( "Size of first word text buffer = %li\n" , iSize1 );

	iSize2 = 0;
	for ( I2WRD = iFirstd ; I2WRD < iTotalNumber ; I2WRD++ )
	{
	   IS1 = *(hpIWS+*(lpTmp+I2WRD));
	   *(lpPoNewWords2 + I2WRD - iFirstd ) = iSize2;
	   for ( iii = 0 ; iii < IWL[*(lpTmp+I2WRD)] ; iii++ )
	   {
	       *(lpNewWords2+iSize2+iii) = *(hpWORDS+IS1+iii);
	   }
	   iSize2 += IWL[*(lpTmp+I2WRD)];
	}
	*(lpPoNewWords2 + iTotalNumber - iFirstd ) = iSize2;
	dprintf ( "Size of second word text buffer = %li\n" , iSize2 );


// ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZzz
  ioutadd = IADSOV;
  dprintf ( "3 IADSOV = %li\n" , IADSOV );
  lastverse = (int)chap.IADCHP[(int)book.IADBOK[67]-1];
  if ( hStartOfVerse != NULL ) GlobalFree ( hStartOfVerse );
  dprintf ( "Size of GlobalAlloc request = %li\n" ,
					 ( dwX = (long int)lastverse * 4 + 4L) );
  if ( ( hStartOfVerse = GlobalAlloc ( GMEM_MOVEABLE ,
					       dwX ) ) == NULL ) OutOfMem ();
  dprintf ( "Size of Start of Verse pointers list = %li\n" ,
					      GlobalSize ( hStartOfVerse ) );
  hplStartOfVerse = (long int huge *)GlobalLock ( hStartOfVerse );

  dprintf ( "reserving verse pointer space... ioutadd = %li\n" , ioutadd );

  for ( I2VER = 0   ; I2VER <= lastverse ; I2VER++ )
  {
	   PUTWRD ( hilo.I2HILO[0] , ioutadd );
	   ioutadd++;
	   PUTWRD ( hilo.I2HILO[1] , ioutadd );
	   ioutadd++;
  }
  dprintf ( "done... ioutadd = %li\n" , ioutadd );
  dprintf ( "book\tchapter\tverse\n" , ibok , ich , ivr );
  for ( I2VER = 1   ; I2VER <= lastverse ; I2VER++ )
  {
    *(hplStartOfVerse+I2VER-1)=ioutadd;
    if ( I2VER % 512 == 1 )
    {
	for ( loadvrs = 0 ; loadvrs < 513 ; loadvrs++ )
	{
	   IAD = 11 + ( (long int)I2VER + (long int)loadvrs - 1 ) * 2;
	   GETWRD ( &hilo.I2HILO[0] , IAD );
	   IAD = IAD + 1;
	   GETWRD ( &hilo.I2HILO[1] , IAD );
	   savevrs[loadvrs] = hilo.I4HILO;
	}
    }
    IVS = savevrs[(I2VER - 1 ) % 512];
    IVE = savevrs[((I2VER - 1 ) % 512) + 1] - 1;
    is = 1;
    ie = 67;
    while ( ie > is + 1 )
    {
	ibok = ( ie + is ) >> 1;
	if ( unBook2Chap[ibok] < (unsigned int)I2VER )
	{
	    is = ibok;
	}
	else
	{
	    ie = ibok;
	}
    }
    if ( unBook2Chap[ie] < (unsigned int)I2VER )
    {
	ibok = ie;
    }
    else
    {
	ibok = is;
    }
    is = (unsigned int)book.IADBOK[ibok] - 1;
    ie = (unsigned int)book.IADBOK[ibok+1] - 1;
    while ( ie > is + 1 )
    {
	ich = ( ie + is ) >> 1;
	if ( chap.IADCHP[ich] < I2VER )
	{
	    is = ich;
	}
	else
	{
	    ie = ich;
	}
    }
    if ( chap.IADCHP[ie] < I2VER )
    {
	ich = ie;
    }
    else
    {
	ich = is;
    }
//  ibok = the book number
    ivr = I2VER - (int)chap.IADCHP[ich];
    ich = ich - (int)book.IADBOK[ibok  ] + 2;
//    dprintf ( "%02i\t%03i\t%03i\r" , ibok , ich , ivr );
    if ( I2VER < 5 || I2VER >= ( lastverse - 1 ) ) dprintf ( "\n" );

//	    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    for ( IVW = IVS ; IVW <= IVE ; IVW++ )
    {
	GETWRD ( &I2WRD , IVW );
	if ( IVW == IVS )
	{
	    if ( I2WRD == 0 ) ICAP = TRUE;
	    I2WRDt = ibok * 256 + ich;
	    PUTWRD ( I2WRDt , ioutadd );
	    ioutadd++;
	    I2WRDt = ivr * 256 + ( ICAP ? 0 : 1 );
	    PUTWRD ( I2WRDt , ioutadd );
	    ioutadd++;
	    if ( I2WRD == 0 ) continue;
	}
	if ( IWL[I2WRD] <=  0 || IWL[I2WRD] > 127 )
	{
	      (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 1 , IDS_BADDATABASE );
	      continue;
	}

//	  IS1 = *(hpIWS+I2WRD);
//	  _fstrncpy ( lpszNextBufLoc , hpWORDS+IS1 , IWL[I2WRD] );

	if ( ICAP )
	{
	    ICAP = FALSE;
	}
	if ( I2WRD < 4 )
	{
	    ICAP = TRUE;
	}

	I2WRD = *(lpOldNumber+I2WRD);

	if ( I2WRD < iFirstd )
	{
    //	     IS1 = *(hpIWS+*(lpTmp+I2WRD));
    //	     IS2 = *(hpIWS+*(lpTmp+I2WRD)+1);
	   IS1 = *(lpPoNewWords1 + I2WRD );
	   IS2 = *(lpPoNewWords1 + I2WRD + 1 );
    //	     _fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 , IS2-IS1 );
	   _fstrncpy( (LPSTR)lpszNextBufLoc , lpNewWords1+IS1 ,
							 (size_t)(IS2-IS1) );
	   lpszNextBufLoc[IS2-IS1] = '\000' ;
	}
	else
	{
     //      IS1 = *(hpIWS+*(lpTmp+I2WRD));
     //      IS2 = *(hpIWS+*(lpTmp+I2WRD)+1);
	   IS1 = *(lpPoNewWords2 + I2WRD - iFirstd );
	   IS2 = *(lpPoNewWords2 + I2WRD - iFirstd + 1 );
     //      _fstrncpy ( (LPSTR)lpszNextBufLoc , hpWORDS+IS1 , IS2-IS1 );
	   _fstrncpy( (LPSTR)lpszNextBufLoc , lpNewWords2+IS1 ,
							 (size_t)(IS2-IS1) );
	   lpszNextBufLoc[IS2-IS1] = '\000' ;
	}
	if ( I2VER < 5 || I2VER >= ( lastverse - 1 ) )
					  dprintf ( " %s" , lpszNextBufLoc );
	PUTWRD ( I2WRD , ioutadd );
	ioutadd++;
    }
  if ( I2VER < 5 ) dprintf ( "\n" );
  }
  dprintf ( "\ndone writing verse text... ioutadd = %li\n" , ioutadd );
  *(hplStartOfVerse+lastverse)=ioutadd;
  ioutadd2 = IADSOV;
  dprintf ( "4 IADSOV = %li\n" , IADSOV );
  dprintf ( "saving start of verse pointer data\n" );
  for ( I2VER = 0   ; I2VER <= lastverse ; I2VER++ )
  {
	   hilo.I4HILO = *(hplStartOfVerse+I2VER);
	   PUTWRD ( hilo.I2HILO[0] , ioutadd2 );
	   ioutadd2++;
	   PUTWRD ( hilo.I2HILO[1] , ioutadd2 );
	   ioutadd2++;
  }
  dprintf ( "done...\n" );
  GlobalUnlock	 ( hStartOfVerse );
  if ( hStartOfVerse != NULL ) GlobalFree ( hStartOfVerse );

  if ( hXRefPointers != NULL ) GlobalFree ( hXRefPointers );
  dprintf ( "Size of GlobalAlloc request = %li\n" ,
				      ( dwX = (long int)iTotalNumber * 4 ) );
  if ( ( hXRefPointers = GlobalAlloc ( GMEM_MOVEABLE ,
					       dwX ) ) == NULL ) OutOfMem ();
  dprintf ( "Size of X-Ref pointers list = %li\n" ,
					      GlobalSize ( hXRefPointers ) );
  hplXRefPointers = (long int huge *)GlobalLock ( hXRefPointers );

  dprintf ( "reformatting X-Refs pointer list...\n" );
  dprintf ( "start of X-Ref list =%li, Byte address = %li\n" , ioutadd	, ioutadd<<1 );
  dprintf ( "oldID\taddress(Byte)\n" );
  for ( iOldID = 9 ; iOldID < MAXWRD ; iOldID++ )
  {
	if ( IWL[iOldID] == 0 ) continue;
	if ( *(lpOldNumber+iOldID) < 9 ) continue;
	*(hplXRefPointers+ *(lpOldNumber+iOldID) ) = ioutadd;
	GETWRD ( &ipoint.I2POI[0] , IADOFF + (long int)iOldID * 2 - 2 );
	GETWRD ( &ipoint.I2POI[1] , IADOFF + (long int)iOldID * 2 - 1 );
	GETWRD ( &iNumRefs , ipoint.IPOINT );
	PUTWRD ( iNumRefs , ioutadd );
  //	dprintf ( "%05i\t%07li\r" , iOldID , ioutadd<<1 );
	if ( iNumRefs > 500 )
	{

	   dprintf ( "iNumRefs,iOldID,OldNumber=%u     %i     %i\n",
					 iNumRefs,iOldID,*(lpOldNumber+iOldID) );
	I2WRD =  *(lpOldNumber+iOldID);
	if ( I2WRD < iFirstd )
	  {
	   IS1 = *(lpPoNewWords1 + I2WRD );
	   IS2 = *(lpPoNewWords1 + I2WRD + 1 );
	   _fstrncpy( (LPSTR)lpszNextBufLoc , lpNewWords1+IS1 ,
							 (size_t)(IS2-IS1) );
	   lpszNextBufLoc[IS2-IS1] = '\000' ;
	  }
	else
	  {
	   IS1 = *(lpPoNewWords2 + I2WRD - iFirstd );
	   IS2 = *(lpPoNewWords2 + I2WRD - iFirstd + 1 );
	   _fstrncpy( (LPSTR)lpszNextBufLoc , lpNewWords2+IS1 ,
							 (size_t)(IS2-IS1) );
	   lpszNextBufLoc[IS2-IS1] = '\000' ;
	  }
	dprintf ( " %s\n" , lpszNextBufLoc );
	}
	ioutadd++;
	for ( iNextRef = 0 ; iNextRef < iNumRefs ; iNextRef++ )
	{
	    GETWRD ( &I2NREF , ipoint.IPOINT + iNextRef + 1 );
	    PUTWRD ( I2NREF , ioutadd );
	    ioutadd++;
	}
  }
  iNewIADOFF = ioutadd;
  dprintf ( "\nlist of pointers to X-Ref lists starts at ioutadd = %li, Byte address = %li\n" ,
						     ioutadd , ioutadd<<1 );
  for ( iNextXref = 0 ; iNextXref < iTotalNumber ; iNextXref++ )
  {
	ipoint.IPOINT = *(hplXRefPointers+iNextXref);
	PUTWRD ( ipoint.I2POI[0] , ioutadd );
	ioutadd++;
	PUTWRD ( ipoint.I2POI[1] , ioutadd );
	ioutadd++;
	if ( iNextXref < 30 )
	    {
		dprintf ( "IPOINT=%li\n" , ipoint.IPOINT );
	    }
  }
  IAD = 8;
  I4PWRD ( iNewIADOFF , IAD );
  PUTWRD ( 0 , ioutadd );
  ioutadd++;
  PUTWRD ( 0 , ioutadd );
  ioutadd++;
  PUTWRD ( 0 , ioutadd );
  ioutadd++;
  dprintf ( "length of file is %li Bytes\n" , ioutadd<<1 );

  dprintf ( "dump of first 12 words of database follows:\n" );
  dprintf ( "5 IADSOV = %li\n" , IADSOV );
  dprintf (" ...\n %li\n ...\n %li\n %li\n ...\n",LASTAD3,iNewIADOFF,IADSOV);

  GlobalUnlock	 ( hXRefPointers );
  if ( hXRefPointers  != NULL ) GlobalFree ( hXRefPointers );

	GlobalUnlock   ( hiTmp );
	if ( hiTmp != NULL ) GlobalFree ( hiTmp );
	GlobalUnlock   ( hOldNumber );
	if ( hOldNumber != NULL ) GlobalFree ( hOldNumber );

	if ( _llseek ( IUNITWords1 , 0L , 0 ) != 0L ) exit (101);
	if ( _lwrite ( IUNITWords1 , lpNewWords1 , iSizeWord1 ) == -1 )
								exit ( 102 );

	if ( _llseek ( IUNITWords2 , 0L , 0 ) != 0L ) exit (103);
	if ( _lwrite ( IUNITWords2 , lpNewWords2 , iSizeWord2 ) == -1 )
								exit ( 104 );

	if ( _llseek ( IUNITWordsp1 , 0L , 0 ) != 0L ) exit (105);
	if ( _lwrite ( IUNITWordsp1 , lpPoNewWords1 , iSizeWordp1 ) == -1 )
								exit ( 106 );

	if ( _llseek ( IUNITWordsp2 , 0L , 0 ) != 0L ) exit (107);
	if ( _lwrite ( IUNITWordsp2 , lpPoNewWords2 , iSizeWordp2 ) == -1 )
								exit ( 108 );

	GlobalUnlock   ( hNewWords1 );
	if ( hNewWords1 != NULL ) GlobalFree ( hNewWords1 );
	GlobalUnlock   ( hNewWords2 );
	if ( hNewWords2 != NULL ) GlobalFree ( hNewWords2 );
	GlobalUnlock   ( hPoNewWords1 );
	if ( hPoNewWords1 != NULL ) GlobalFree ( hPoNewWords1 );
	GlobalUnlock   ( hPoNewWords2 );
	if ( hPoNewWords2 != NULL ) GlobalFree ( hPoNewWords2 );
	UnlockResource ( hIWS );
	UnlockResource ( hWORDS );
	_lclose ( IUNIT10 );
	_lclose ( IUNIT11 );
	_lclose ( IUNITWords1 );
	_lclose ( IUNITWords2 );
	_lclose ( IUNITWordsp1 );
	_lclose ( IUNITWordsp2 );

	OPEN_ListSizes ( &IUNITListSizes , "listsize.dat" , OF_READWRITE | OF_CREATE
						      | OF_SHARE_DENY_NONE );
	if ( _llseek ( IUNITListSizes , 0L , 0 ) != 0L ) exit (71);
	if ( _lwrite ( IUNITListSizes , &iFirstd ,
				  sizeof ( iFirstd ) ) == -1 ) exit ( 72 );
	dprintf ( "iFirstd is %i Bytes\n" , iFirstd );
	if ( _lwrite ( IUNITListSizes , szFirstChar , (UINT)2 ) == -1 )
								 exit ( 73 );
	dprintf ( "szFirstChar is %s\n" , szFirstChar );
	if ( _lwrite ( IUNITListSizes , &iSizeWord1 ,
				sizeof ( iSizeWord1 ) ) == -1 ) exit ( 74 );
	dprintf ( "iSizeWord1 is %u Bytes\n" , iSizeWord1 );
	if ( _lwrite ( IUNITListSizes , &iSizeWord2 ,
				sizeof ( iSizeWord2 ) ) == -1 ) exit ( 75 );
	dprintf ( "iSizeWord2 is %u Bytes\n" , iSizeWord2 );
	if ( _lwrite ( IUNITListSizes , &iSizeWordp1 ,
				sizeof ( iSizeWordp1 ) ) == -1 ) exit ( 76 );
	dprintf ( "iSizeWordp1 is %u Bytes\n" , iSizeWordp1 );
	if ( _lwrite ( IUNITListSizes , &iSizeWordp2 ,
				sizeof ( iSizeWordp2 ) ) == -1 ) exit ( 77 );
	dprintf ( "iSizeWordp2 is %u Bytes\n" , iSizeWordp2 );
	_lclose ( IUNITListSizes );

	return;

}

void NEAR GETWL ( long int IAD )
{
long int MAXW,IAD1,I,LSTWDF,MADD,IS1,IW,IS1A,ID1,IS2;
char temptext[40];

	I4GWRD ( &MAXW , IAD );
	if ( MAXW != MAXWRD ) wexit ( 3000 );
	IAD1 = IAD + 2;
	I1GWRD ( IWL , MAXW , IAD1 );
	IAD1 = IAD1 + MAXW / 2 + ( MAXW % 2 );
        if ( ( hIWS = LoadResource ( hInst , FindResource ( hInst , "wordsizes" ,
				      "WORDSIZES" ) ) ) == NULL )
	      //   NOTE : Actually, these ^^^ are the word START locations !
        {
	    (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 1 , IDS_BADDB2 );
        }
        if ( ( hpIWS = ( long int huge * )LockResource ( hIWS ) ) == NULL )
        {
	    (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 1 , IDS_OUTOFMEM );
        }
	UnlockResource ( hIWS );
	I4GWRD ( &LASTSB , IAD1 );
	IAD1 = IAD1 + 2;
	I4GWRD ( &LASTWD , IAD1 );
	IAD1 = IAD1 + 2;
	for ( I = 1 ; I <= LASTSB - 1 ; I++ )
	{
	    if ( I > NDIMW ) wexit ( 787 );
//	      CHGWRD ( WORDS[I] , NLENW , IAD1 );
	    IAD1 = IAD1 + NLENW / 2 + ( NLENW % 2 );
	}
	if ( LASTWD != 0 )
	{
	    LSTWDF = ( ( LASTWD + 1 ) / 2 ) * 2;
//	      CHGWRD ( WORDS[LASTSB] , LSTWDF , IAD1 );
	}
        if ( ( hWORDS = LoadResource ( hInst , FindResource ( hInst , "wordlist" ,
                                      "WORDLIST" ) ) ) == NULL )
        {
	    (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 1 , IDS_BADDB2 );
        }
        if ( ( hpWORDS = ( char huge * )LockResource ( hWORDS ) ) == NULL )
        {
	    (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 1 , IDS_OUTOFMEM );
        }
	UnlockResource ( hWORDS );
	LASTAD = IAD1 + LASTWD / 2 + ( LASTWD % 2 ) + 1;
	MADD = 0;
	hpIWS = ( long int huge * )LockResource ( hIWS );
	for ( I = 1 ; I <= MAXW ; I++ )
	{
	    if ( IWL[I] != 0 )		   // GO TO 3101
	    {
		IS1 = *(hpIWS+I);
		if ( *(hpIWS+I) > MADD ) IW = I;
		MADD = max ( MADD , IS1 + IWL[I] - 1 );
	    }
	}
	    IS1 = *(hpIWS+IW);
	    UnlockResource ( hIWS );
	    IS1A = ( IS1 % NLENW );
	    ID1 = IS1 / NLENW + 1;
	    if ( IS1A == 0 )
	    {
		ID1 = ID1 - 1;
		IS1A = NLENW;
	    }
	    IS2 = IS1A + IWL[IW] - 1;
	    hpWORDS = ( char huge * )LockResource ( hWORDS );
	    for ( I = 0 ; (unsigned char)I < IWL[IW] ; I++ )
					     temptext[I] = *(hpWORDS+IS1+I);
	    UnlockResource ( hWORDS );


	return;
}

void FAR FNDWRD ( int *I2HASH )
{
long int IHASH,LASTC,I,ILOC;
char *npAWORD;
int comp;
	IHASH = 0;
	*I2HASH = 0;
	LASTC = 1;
	for ( I = 1 ; I <= LAWORD ; I++ )
	{
	    IHASH = IHASH + (long int)AWORD[I] * LASTC;
	    LASTC = (int)AWORD[I];
	}
	IHASH = IHASH % MAXWRD;
	if ( IHASH == 0 ) IHASH = MAXWRD;
	goto L30;
L20:
	IHASH++;
	if ( IHASH > MAXWRD ) IHASH = 1;
L30:
	if ( IWL[IHASH] == 0 )
	{
	    return;
	}
	else
	{
	    if ( IWL[IHASH] != (unsigned char)LAWORD ) goto L20;
	    hpIWS = ( long int huge * )LockResource ( hIWS );
	    ILOC = *(hpIWS+IHASH);
	    UnlockResource ( hIWS );
	    hpWORDS = ( char huge * )LockResource ( hWORDS );
	    hpWORDS += ILOC;
	    npAWORD = &AWORD[1];
	    for ( comp = 0 ; comp < (int)LAWORD ; comp++ )
	    {
		if ( *hpWORDS != *npAWORD ) break;
		hpWORDS++;
		npAWORD++;
	    }
	    UnlockResource ( hWORDS );
	    if ( comp != (int)LAWORD  ) goto L20;
	}
	*I2HASH = (int)IHASH;
	return;
}

void NEAR GETOC ( long int LOCTC , long int* add2 )
{
    long int IAD,I,add3;
    add3 = *add2;

// get the chapter data
    IAD = LOCTC -1;
    for ( I = 2 ; I < 2402 ; I++ )
    {
	IAD++;
	GETWRD ( &chap.I2AC[I] , IAD );
 //	  PUTWRD ( chap.I2AC[I] , IAD );
    }
    for ( I = 2 ; I < 1189 ; I++ )
    {
	chap.IADCHP[I] = chap.IADCHP[I-1] +
		   ( chap.IADCHP[I] - 2 * chap.IADCHP[I-1] );
    }
    chap.IADCHP[1189] = chap.IADCHP[1188] + 21;
    for ( I = 2 ; I < 2402 ; I++ )
    {
	  PUTWRD ( chap.I2AC[I] , add3 );
	  add3++;
    }

// get tha book data
    for ( I = 2 ; I < 136 ; I++ )
    {
	IAD++;
	GETWRD ( &book.I2BK[I] , IAD );
//	  PUTWRD ( book.I2BK[I] , IAD );
    }
    for ( I = 1 ; I <= 67 ; I++ )
    {
	unBook2Chap[I] = (unsigned int)chap.IADCHP[book.IADBOK[I]-1];
    }
    unBook2Chap[0] = (unsigned int)1;
    for ( I = 2 ; I < 136 ; I++ )
    {
	  PUTWRD ( book.I2BK[I] , add3 );
	  add3++;
    }
    *add2 = add3;
    dprintf ( " add3 = %li\n", add3 );
    return;
}

/****************************************************************************

   FUNCTION:   MakeHelpPathName

   PURPOSE:    Bible4w assumes that the .HLP help file is in the same
	       directory as the Bible4w executable.This function derives
               the full path name of the help file from the path of the
               executable.

****************************************************************************/

void FAR MakeHelpPathName(szFileName)
char * szFileName;
{
   char *  pcFileName;
   int     nFileNameLen;

   nFileNameLen = GetModuleFileName(hInst,szFileName,EXE_NAME_MAX_SIZE);
   pcFileName = szFileName + nFileNameLen;

   while (pcFileName > szFileName) {
       if (*pcFileName == '\\' || *pcFileName == ':') {
           *(++pcFileName) = '\0';
           break;
       }
   nFileNameLen--;
   pcFileName--;
   }

   if ((nFileNameLen+13) < EXE_NAME_MAX_SIZE) {
       lstrcat(szFileName, "kjb3p0.dat");
   }

   else {
       lstrcat(szFileName, "?");
   }

   return;
}

void FAR OutOfMem ( void )
{
    int msg;

    MessageBeep ( MB_ICONHAND );
    msg = MessageBox ( hWnd , "Out of Memory - close some windows and try again",
		    "Out of Memory", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );

#if defined ( DEBUG )
    if ( msg == 0 )
    {
	dprintf ( "Out of Memory\n" );
	FatalExit ( -2 );
    }
#else
    //	PostQuitMessage ( 1 );
	FatalExit ( -2 );
#endif

	FatalExit ( -2 );
    return;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : BWError ( hwnd, flags, quit, id, ...)			    *
 *		 hwnd = parent Window handle				    *
 *		 flags = MB_ flags sent to the message box routine	    *
 *		 quit = post quit message flag ( ==1 => post quit)	    *
 *		 id = string id number					    *
 *		 (additional parameters passed on to wsprintf function)     *
 *									    *
 *  PURPOSE    : Flashes a Message Box to the user. The format string is    *
 *		 taken from the STRINGTABLE.				    *
 *									    *
 *  RETURNS    : Returns value returned by MessageBox() to the caller.	    *
 *									    *
 ****************************************************************************/
short FAR BWError(HWND hwnd, WORD bFlags, WORD quit, WORD id, ...)
{
    int msg;
    char sz[160];
    char szFmt[128];

    switch ( quit )
    {
	case 0:
	default:
	   MessageBeep(MB_ICONEXCLAMATION);
	   break;

	case 1:
	   MessageBeep ( MB_ICONHAND );
	   break;
    }
    if ( LoadString (hInst, id, szFmt, sizeof (szFmt)) == 0 )
    {
	msg = MessageBox (hWnd, "Cannot load string resource!", NULL,
	     MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );

#if defined ( DEBUG )
	if ( msg == 0 )
	{
	    dprintf ( "Cannot load string\n" );
	    FatalExit ( -1 );
	}
#endif
	PostQuitMessage ( 1 );
	return 0;
    }

 //   wvsprintf (sz, szFmt, (LPSTR)(&id + 1));
    if ( !(msg = MessageBox (hWnd, sz, NULL, bFlags) ) )
    {
	msg = MessageBox ( hWnd , "Out of Memory (2) - close some windows and try again",
		    "Out of Memory", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );

#if defined ( DEBUG )
	if ( msg == 0 )
	{
	    dprintf ( "Out of Memory\n" );
	    FatalExit ( -1 );
	}
#endif
	PostQuitMessage ( 1 );
	return 0;
    }
//    if ( quit == 1 ) PostQuitMessage ( 1 );
    if ( quit == 1 ) exit ( 1 );
    return msg;
}

VOID FAR wexit(int status)
{
    (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
	  , 1 , IDS_ABORT, status );
    return;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : Bible4wInit (hInstance,hPrevInstance)			    *
 *									    *
 *  PURPOSE    : Registers the main window class.			    *
 *									    *
 *  RETURNS    : TRUE	-  if RegisterClass() went off ok		    *
 *		 FALSE	-  otherwise.					    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL Bible4wInit (hInstance,hPrevInstance)

HANDLE hInstance;
HANDLE hPrevInstance;

{
    HANDLE    hMemory;
    PWNDCLASS pWndClass;
    DWORD TempFlags, TotFreeMem;
    DWORD dwMinFree=400000;
    char lpszTemp[30];
    HANDLE hMemTemp; BOOL bTemp;
    char szAppName[] = APPNAME;
    WORD nWindowsVersion;
    int iKanjiWinHeight, iDBCSEnabled;

#if defined ( DEBUG )
	    dprintf ( "*********************** In Bible4wInit **********************\n" );
#endif

    TempFlags = GetWinFlags ();
    nWindowsVersion = GetVersion();
    iDBCSEnabled    = GetSystemMetrics ( SM_DBCSENABLED );
    iKanjiWinHeight = GetSystemMetrics ( SM_CYKANJIWINDOW );
    //cxScreen =	GetSystemMetrics ( SM_CXSCREEN );
    //cyScreen =	GetSystemMetrics ( SM_CYSCREEN );
    //cxDlgFrame =	GetSystemMetrics ( SM_CXDLGFRAME );
    //cyDlgFrame =	GetSystemMetrics ( SM_CYDLGFRAME );
    //cyCaption =	GetSystemMetrics ( SM_CYCAPTION );

#if defined ( DEBUG )
	    dprintf ( "TempFlags = %li, Windows Version = %i , %i\n",
		       TempFlags,
		       (int)LOBYTE(nWindowsVersion),
		       (int)HIBYTE(nWindowsVersion) );
	    dprintf ( "Kanji Window Height = %i\n", iKanjiWinHeight );
	    dprintf ( "DBCS Enabled = %i\n", iDBCSEnabled );
	    if ( TempFlags & WF_CPU086 )
	       dprintf ( "8086 cpu, " );
	    if ( TempFlags & WF_CPU186 )
	       dprintf ( "80186 cpu, " );
	    if ( TempFlags & WF_CPU286 )
	       dprintf ( "80286 cpu, " );
	    if ( TempFlags & WF_CPU386 )
	       dprintf ( "80386 cpu, " );
	    if ( TempFlags & WF_CPU486 )
	       dprintf ( "80486 cpu, " );
	    if ( !(TempFlags & WF_80x87) )
	       dprintf ( "no " );
	    dprintf ( "80x87 math coprocessor, " );
	    if ( TempFlags & WF_ENHANCED )
	       dprintf ( "386 enhanced " );
	    if ( TempFlags & WF_STANDARD )
	       dprintf ( "standard " );
	    if ( TempFlags & WF_PMODE )
	       dprintf ( "protected mode" );
	    if ( TempFlags & WF_SMALLFRAME )
	       dprintf ( "small-frame EMS" );
	    if ( TempFlags & WF_LARGEFRAME )
	       dprintf ( "large-frame EMS" );
	    dprintf ( "\n" );
#endif

    if ( !( ( TempFlags & WF_ENHANCED ) | ( TempFlags & WF_STANDARD ) ) )
    {
	hMemTemp = GlobalAlloc ( GMEM_MOVEABLE ,
			       TotFreeMem = GlobalCompact ( dwMinFree ) );
	if ( hMemTemp == NULL )
	{
	    (void)BWError ( hWnd,
				    MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
				    , 0 , IDS_CANTCOMPACT );
	}
	GlobalFree ( hMemTemp );
	dprintf ( "Free Memory = %li\n" , TotFreeMem );
    }

    MakeHelpPathName ( szHelpFileName );

    if ( !hPrevInstance )
    {
	/* Initialize the menu window class */
        if ( ( hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS)) ) == NULL )
        {

#if defined ( DEBUG )
	    dprintf ( "Cannot allocate local heap for WndClass stricture\n" );
#endif

            OutOfMem ();
        }
	pWndClass = (PWNDCLASS) LocalLock(hMemory);

	pWndClass->style	 = NULL;
	pWndClass->lpfnWndProc	 = Bible4WndProc;
	pWndClass->hInstance	 = hInstance;
	pWndClass->hIcon	 = LoadIcon (hInstance, APPNAME );
	pWndClass->hCursor	 = LoadCursor (NULL, IDC_ARROW);
	pWndClass->hbrBackground = GetStockObject (WHITE_BRUSH);
	pWndClass->lpszMenuName  = (LPSTR) "ConvertMenu",
	pWndClass->lpszClassName = (LPSTR) APPNAME ;

	bTemp = RegisterClass ( pWndClass );
	LocalUnlock ( hMemory );
	LocalFree ( hMemory );
	if ( !bTemp ) return bTemp;
    }

/* Create the app. window */

    return ( ( hWnd = CreateWindow (
			 APPNAME ,
			 "Bible database convert program",
                         WS_OVERLAPPEDWINDOW | WS_VSCROLL |
                                               ES_AUTOVSCROLL,
			 CW_USEDEFAULT,
			 CW_USEDEFAULT,
			 CW_USEDEFAULT,
			 CW_USEDEFAULT,
			 (HWND) NULL,
			 NULL,
			 hInstance,
			 (LPSTR) NULL
			 ) ) == NULL ? FALSE : TRUE );
}

VOID FAR OPEN_KJB ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&ofKJB,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENB, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_CLIP ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&ofCLIP,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_WORDS1 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&ofWords1,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_WORDS2 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&ofWords2,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_WORDSp1 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&oflpWords1,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_WORDSp2 ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&oflpWords2,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

VOID FAR OPEN_ListSizes ( HFILE FAR *pFile, LPSTR szFileName, WORD szMode )
{
   if ((*pFile = OpenFile(szFileName,&oflpListSizes,szMode)) == (HFILE)-1)
   {
      (void)BWError ( hWnd, MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION
		  , 1 , IDS_CANTOPENS, (LPSTR)szFileName );
   }
}

/* Compares and returns greater than (1), less than (-1), or equal to (0).
 * This function is called by qsort and bsearch. When used with qsort the
 * order of entries is unimportant. When used with bsearch, elem1 is the
 * key to be found, and elem2 is the the current table entry.
 */
int fCompareWords( unsigned *elem1, unsigned *elem2 )
{
    int iLength , i;
    long int IS1p , IS2;
    char char1 , char2;

    IS1p = *( hpIWS + *elem1 );
    IS2 = *( hpIWS + *elem2 );
    iLength = min( IWL[*elem1] , IWL[*elem2] );
    char1 = isalpha ( *(hpWORDS+IS1p) ) ?
			       tolower ( *(hpWORDS+IS1p) ) : *(hpWORDS+IS1p);
    char2 = isalpha ( *(hpWORDS+IS2) ) ?
			       tolower ( *(hpWORDS+IS2) )  : *(hpWORDS+IS2);

    if ( char1 > char2 )
	return 1;
    else if ( char1 < char2 )
	return -1;
    else
    {
	if ( iLength > 1 )
	{
	   for ( i = 1 ; i < iLength ; i++ )
	   {
	      if( *(hpWORDS+IS1p+i) > *(hpWORDS+IS2+i) )
		  return 1;
	      else if( *(hpWORDS+IS1p+i) < *(hpWORDS+IS2+i) )
		  return -1;
	      else
		  continue;
	   }

	   if( IWL[*elem1] > IWL[*elem2] )
	       return 1;
	   else if( IWL[*elem1] < IWL[*elem2] )
	       return -1;
	}

	if( *(hpWORDS+IS1p) > *(hpWORDS+IS2) )
	    return 1;
	else if( *(hpWORDS+IS1p) < *(hpWORDS+IS2) )
	    return -1;
	else
	   return 0;
    }
}

int FAR cdecl printfx	(LPSTR szFmt, WORD id , ...)
{
   char szT[100];
   int lng;
   lng = wsprintf ( szT , szFmt , id , *((&id)+1) , *((&id)+2) , *((&id)+3)
		  , *((&id)+4) , *((&id)+5)
		  );
 //  if ( _llseek ( h1 , 0L  , 0	) != 0L	) exit ( 121 );
   if ( _lwrite ( h1 , szT , lng ) == -1 ) exit ( 122 );
   return 78;
}
