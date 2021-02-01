#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include "convert.h"

union ch2i2
{
    int I2IXC;
    char CHIX[2];
} chrs;

union i22i1
{
    unsigned int I2IX1;
    unsigned char I1IX1[2];
} int1;

union i22i4
{
    int I2IX4[2];
    long int I4IX4;
} int4;

union i42i2
{
    int I2IX2[2];
    long int I4IX2;
} int24;

VOID FAR GETWRD ( int* , long int );
VOID FAR PUTWRD ( int , long int );
VOID NEAR READrec ( long int );
VOID NEAR WRITErec ( long int );
VOID NEAR READrec2 ( long int );
VOID NEAR WRITErec2 ( long int );


void FAR CHPWRD ( char I1ARAY[] , long int LEN , long int IAD )
{
    long int INA , I;

    if ( LEN % 2 != 0 ) exit ( 8989 );

    INA = IAD - 1;
    for ( I = 1 ; I <= LEN ; I++ , I++ )
    {
	INA++;
	chrs.CHIX[0] = I1ARAY[I];
	chrs.CHIX[1] = I1ARAY[I+1];
	PUTWRD ( chrs.I2IXC , INA );
    }
//  100   CONTINUE
    return;
}

void FAR I1PWRD ( char I1ARAY[] , long int LEN , long int IAD )
{
    long int INA , I;
    if ( LEN % 2 != 0 ) exit ( 8989 );
    INA = IAD - 1;
    for ( I = 1 ; I <= LEN ; I++ , I++ )
    {
	INA++;
	int1.I1IX1[0] = I1ARAY[I];
	int1.I1IX1[1] = I1ARAY[I+1];
	PUTWRD ( int1.I2IX1 , INA );
    }
//  100   CONTINUE
    return;
}

void FAR I4PWRD ( long int I4WRD , long int IAD )
{
    int4.I4IX4 = I4WRD;
    PUTWRD ( int4.I2IX4[0] , IAD );
    PUTWRD ( int4.I2IX4[1] , IAD + 1 );
    return;
}

void FAR CHGWRD ( char huge I1ARAY[] , long int LEN , long int IAD )
{
    long int INA , I;
    if ( LEN % 2 != 0 ) exit ( 8989 );
    INA = IAD - 1;
    for ( I = 1 ; I <= LEN ; I++ , I++ )
    {
	INA++;
	GETWRD ( &chrs.I2IXC , INA );
	I1ARAY[I-1] = chrs.CHIX[0];
	I1ARAY[I] = chrs.CHIX[1];
    }
//  100 CONTINUE
    return;
}

void FAR I1GWRD ( char I1ARAY[] , long int LEN , long int IAD )
{
    long int INA , I;
    int xxx;
//    long int IND , IRECN;
    if ( LEN % 2 != 0 ) exit ( 8989 );
    INA = IAD - 1;
//    IND = ( ( INA - 1 ) % 256 ) + 1;
//    IRECN = ( INA - 1 ) / 256 + 1;
//    printf ( " in I1GWRD, LEN,IAD,IND,IRECN= %li %li %li %li\n" ,
//			       LEN,IAD,IND,IRECN );
//    incode=1;
    for ( I = 1 ; I <= LEN ; I++ , I++ )
    {
	INA++;
	GETWRD ( &xxx , INA );
	int1.I2IX1 = xxx;
	I1ARAY[I] = int1.I1IX1[0];
	I1ARAY[I+1] = int1.I1IX1[1];
//  100   CONTINUE
    }
//    incode=0;
    return;
}

void FAR I4GWRD ( long int *I4WRD , long int IAD )
{
    GETWRD ( &int24.I2IX2[0] , IAD );
    GETWRD ( &int24.I2IX2[1] , IAD + 1 );
    *I4WRD = int24.I4IX2;
    return;
}

void FAR GETWRD ( int *IHS , long int IAD )
{
//	  COMMON /BUF/ I2IX(256) , LASTAD , IRECIN , IRMOD , I2ZERO
//	  INTEGER*2 I2ZERO
//	  INTEGER*2 I2IX
    long int IND , IRECN;
    IND = ( ( IAD - 1 ) % 256 ) + 1;
    IRECN = ( IAD - 1 ) / 256 + 1;
    if ( IRECN != IRECIN )
    {
	if ( IRMOD != 0 ) WRITErec ( IRECIN );
	IRECIN = IRECN;
	READrec ( IRECIN );
	IRMOD = 0;
    }
    *IHS = I2IX[IND];
    return;
}

void FAR PUTWRD ( int IHS , long int IAD )
{
//    COMMON /BUF/ I2IX(256) , LASTAD , IRECIN , IRMOD , I2ZERO
//    INTEGER*2 I2ZERO
//    INTEGER*2 I2IX
    long int IND , IRECN , IADX;
    if ( IAD <= LASTAD2 )
    {
	IND = ( ( IAD - 1 ) % 256 ) + 1;
	IRECN = ( IAD - 1 ) / 256 + 1;
	if ( IRECN != IRECIN2 )
	{
	    if ( IRMOD2 != 0 ) WRITErec2 ( IRECIN2 );
	    IRECIN2 = IRECN;
	    READrec2 ( IRECIN2 );
	}
	IRMOD2 = 1;
	I2IX22[IND] = IHS;
	return;
    }
    else
    {
	IND = ( ( LASTAD2 - 1 ) % 256 ) + 1;
	IRECN = ( LASTAD2 - 1 ) / 256 + 1;
	if ( IRECN != IRECIN2 )
	{
	    if ( IRMOD2 != 0 ) WRITErec2 ( IRECIN2 );
	    IRECIN2 = IRECN;
	    READrec2 ( IRECIN2 );
	}
	for ( IADX = LASTAD2 + 1 ; IADX <= IAD ; IADX++ )
	{
	    IND = ( ( IADX - 1 ) % 256 ) + 1;
	    IRECN = ( IADX - 1 ) / 256 + 1;
	    if ( IRECN != IRECIN2 )
	    {
		if ( IRMOD2 != 0 ) WRITErec2 ( IRECIN2 );
		IRECIN2 = IRECN;
	    }
	    IRMOD2 = 1;
	    I2IX22[IND] = I2ZERO;
	}
//  100    CONTINUE
	LASTAD2 = IAD;
	I2IX22[IND] = IHS;
	return;
    }
}

VOID NEAR READrec ( long int recno )
{
    _llseek ( IUNIT10 , (long int)( ( recno - 1 ) * 512 ) , 0 );
    if ( _lread ( IUNIT10 , (LPSTR)&I2IX[1] , 512 ) <= 0 )
    {
	exit ( 97 );
    }
    return;
}

VOID NEAR WRITErec ( long int recno )
{
    _llseek ( IUNIT10 , (long int)( ( recno - 1 ) * 512 ) , 0 );
//    if ( _lwrite ( IUNIT10 , (LPSTR)&I2IX[1] , 512 ) == -1 ) exit ( 99 );
    exit (98);
    return;
}

VOID NEAR READrec2 ( long int recno )
{
    int it;

    _llseek ( IUNIT11 , (long int)( ( recno - 1 ) * 512 ) , 0 );
    if ( _lread ( IUNIT11 , (LPSTR)&I2IX22[1] , 512 ) <= 0 )
    {
	for ( it = 1 ; it <= 256 ; it++ )
	{
	    I2IX22[it] = 0;
	}
    }
    return;
}

VOID NEAR WRITErec2 ( long int recno )
{
    long int iTempRec;
    iTempRec = _llseek ( IUNIT11 , (long int)( ( recno - 1 ) * 512 ) , 0 );
    if ( iTempRec != (long int)( ( recno - 1 ) * 512 ) )
						   WRITErec2 ( recno - 1 );
    if ( _lwrite ( IUNIT11 , (LPSTR)&I2IX22[1] , 512 ) == -1 )
    {
	 exit ( 99 );
    }
    return;
}
