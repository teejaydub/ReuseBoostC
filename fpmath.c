// Floating point routines, from SourceBoost, converted from routines provided by Microchip.

#include <system.h>
#include "fpmath.h"

char FPbuff[16]; // Floating-point math buffer


void FPmath(char op)//op=0 int->float
					//op=1 round(float)->int
					//op=2 trunc(float)->int
					//op=3 add
					//op=4 subtract
					//op=5 multiply
					//op=6 divide
{
// registers mapped into FPbuff:
//	char EXP;		// 8 bit biased exponent for argument A
//	char AARGB0;	// most significant byte of argument A
//	char AARGB1;
//	char AARGB2;
//	char AARGB3;
//	char AARGB4;
//	char AARGB5;

//	char BEXP;		// 8 bit biased exponent for argument B 
//	char BARGB0;	// most significant byte of argument B
//	char BARGB1;
//	char BARGB2;
//	char BARGB3;
	
//	char TEMP;		// temporary storage
//	char TEMPB1;	// 

//	char SIGN;		// save location for sign in 7
//	char FPFLAGS;	// floating point library exception flags
	//IOV bit0 = integer overflow flag
	//FOV bit1 = floating point overflow flag
	//FUN bit2 = floating point underflow flag
	//FDZ bit3 = floating point divide by zero flag
	//NAN bit4 = not-a-number exception flag
	//DOM bit5 = domain error exception flag
	//RND bit6 = floating point rounding flag, 0 = truncation
	//			 1 = unbiased rounding to nearest LSB
	//SAT bit7 = floating point saturate flag, 0 = terminate on
	//			 exception without saturation, 1 = terminate on
	//			 exception with saturation to appropriate value

	
	if (op==2){FPbuff[15]=0x80;} // saturation
	 else {FPbuff[15]=0xC0;}     // rounding & saturation
	asm
	{
		MOVF	_op,F
		BTFSC	_status,Z
		GOTO	FLO3232
		MOVLW	0xFF
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	INT3232
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	INT3232
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	FPA32
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	FPS32
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	FPM32
		ADDWF	_op,F
		BTFSC	_status,Z
		GOTO	FPD32
		RETLW	0xFF
		
//----- Normalization routine
NRM3232:					
NRM32:		
		CLRF	_FPbuff+12
		MOVF	_FPbuff+1,W
		BTFSS	_status,Z		
		GOTO	NORM3232		
		MOVF	_FPbuff+2,W
		MOVWF	_FPbuff+1		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+2		
		CLRF	_FPbuff+3		
		BSF		_FPbuff+12,3
					
		MOVF	_FPbuff+1,W
		BTFSS	_status,Z		
		GOTO	NORM3232		
		MOVF	_FPbuff+2,W
		MOVWF	_FPbuff+1		
		CLRF	_FPbuff+2		
		BCF		_FPbuff+12,3
		BSF		_FPbuff+12,4
					
		MOVF	_FPbuff+1,W
		BTFSC	_status,Z		
		GOTO	RES032
					
NORM3232:		
		MOVF	_FPbuff+12,W		
		SUBWF	_FPbuff,F		
		BTFSS	_status,Z		
		BTFSS	_status,C		
		GOTO	SETFUN32		
					
		BCF		_status,C
NORM3232A:	
		BTFSC	_FPbuff+1,7
		GOTO	FIXSIGN32		
		RLF		_FPbuff+3,F
		RLF		_FPbuff+2,F
		RLF		_FPbuff+1,F		
		DECFSZ	_FPbuff,F		
		GOTO	NORM3232A		
		GOTO	SETFUN32
					
MUL32OK:					
FIXSIGN32:
		BTFSS	_FPbuff+14,7
		BCF		_FPbuff+1,7
		RETLW	0
					
RES032:		
		CLRF	_FPbuff
IRES032:		
		CLRF	_FPbuff+4
IRES03224:	
		CLRF	_FPbuff+3		
		CLRF	_FPbuff+2
		CLRF	_FPbuff+1
		RETLW	0	
		
//----- Integer to float conversion 
FLO3232:
		MOVLW	0x9E
		MOVWF	_FPbuff		
		CLRF	_FPbuff+14		
		BTFSS	_FPbuff+1,7
		GOTO	NRM4032		
		COMF	_FPbuff+4,F
		COMF	_FPbuff+3,F		
		COMF	_FPbuff+2,F		
		COMF	_FPbuff+1,F		
		INCF	_FPbuff+4,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
		BSF		_FPbuff+14,7	
		
//----- Normalization routine
		
NRM4032:
		CLRF	_FPbuff+12
		MOVF	_FPbuff+1,W
		BTFSS	_status,Z		
		GOTO	NORM4032		
		MOVF	_FPbuff+2,W
		MOVWF	_FPbuff+1		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+2		
		MOVF	_FPbuff+4,W		
		MOVWF	_FPbuff+3		
		CLRF	_FPbuff+4		
		BSF		_FPbuff+12,3
					
		MOVF	_FPbuff+1,W
		BTFSS	_status,Z		
		GOTO	NORM4032		
		MOVF	_FPbuff+2,W
		MOVWF	_FPbuff+1		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+2		
		CLRF	_FPbuff+3		
		BCF		_FPbuff+12,3
		BSF		_FPbuff+12,4		
					
		MOVF	_FPbuff+1,W
		BTFSS	_status,Z		
		GOTO	NORM4032		
		MOVF	_FPbuff+2,W
		MOVWF	_FPbuff+1		
		CLRF	_FPbuff+2		
		BSF		_FPbuff+12,3
					
		MOVF	_FPbuff+1,W
		BTFSC	_status,Z		
		GOTO	RES032		
					
NORM4032:		
		MOVF	_FPbuff+12,W		
		SUBWF	_FPbuff,F		
		BTFSS	_status,Z		
		BTFSS	_status,C		
		GOTO	SETFUN32		
					
		BCF		_status,C
					
NORM4032A:
		BTFSC	_FPbuff+1,7
		GOTO	NRMRND4032		
		RLF		_FPbuff+4,F
		RLF		_FPbuff+3,F
		RLF		_FPbuff+2,F		
		RLF		_FPbuff+1,F		
		DECFSZ	_FPbuff,F		
		GOTO	NORM4032A		
		GOTO	SETFUN32
					
NRMRND4032:	
		BTFSC	_FPbuff+15,6	// what about large integers?
		BTFSS	_FPbuff+3,0		
		GOTO	FIXSIGN32		
		BTFSS	_FPbuff+4,7
		GOTO	FIXSIGN32		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
		BSF		_status,C
		BTFSS	_status,Z
		GOTO	FIXSIGN32		
		RRF		_FPbuff+1,F
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		INCF	_FPbuff,F		
		BTFSC	_status,Z
		GOTO	SETFOV32		
		GOTO	FIXSIGN32		

//----- Float to integer conversion
INT3232:					
		CLRF	_FPbuff+4		
		MOVF	_FPbuff,W
		BTFSC	_status,Z		
		GOTO	IRES032 
		
		CLRF	_FPbuff+14
		BTFSC	_FPbuff+1,7
		DECF	_FPbuff+14,F
			
		BSF		_FPbuff+1,7
					
		MOVLW	0x9E
		SUBWF	_FPbuff,F	
 
		BTFSS	_status,C
		GOTO	INT3232A
		BTFSC	_status,Z 	
		BTFSS	_FPbuff+14,7 	
		GOTO	SETIOV32	
		MOVF	_FPbuff+1,W	
		ANDLW	0x7F	
		IORWF	_FPbuff+2,W	
		IORWF	_FPbuff+3,W
		BTFSS	_status,Z	
		GOTO	SETIOV32
INT3232A:
		COMF	_FPbuff,F		
		INCF	_FPbuff,F		
					
		MOVLW	4
		MOVWF	_FPbuff+5
		BCF		_status,C
	
TSHIFT3232B:		
		RLF		_FPbuff+14,F		
		MOVLW	8
		SUBWF	_FPbuff,W		
		BTFSS	_status,C		
		GOTO	TSHIFT3232		
		MOVWF	_FPbuff		
		RLF		_FPbuff+4,F
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+4		
		MOVF	_FPbuff+2,W		
		MOVWF	_FPbuff+3		
		MOVF	_FPbuff+1,W		
		MOVWF	_FPbuff+2		
		CLRF	_FPbuff+1		
		DECFSZ	_FPbuff+5,F	
		GOTO	TSHIFT3232B		
					
		MOVF	_FPbuff,W		
		BTFSS	_status,Z		
		BCF		_status,C		
		GOTO	SHIFT3232OK		
					
TSHIFT3232:
		BTFSC	_FPbuff+14,0
		BSF		_status,C
		MOVF	_FPbuff,W
		BTFSC	_status,Z		
		GOTO	SHIFT3232OK		
					
SHIFT3232:
		BCF		_status,C		
		RRF		_FPbuff+1,F
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		RRF		_FPbuff+4,F		
		DECFSZ	_FPbuff,F		
		GOTO	SHIFT3232
					
SHIFT3232OK:	
		BTFSC	_FPbuff+15,6
		BTFSS	_status,C		
		GOTO	INT3232OK		
		INCF	_FPbuff+4,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
		BTFSC	_FPbuff+1,7
		GOTO	SETIOV32
					
INT3232OK:
		BTFSS	_FPbuff+14,7
		RETLW	0		
		COMF	_FPbuff+1,F		
		COMF	_FPbuff+2,F		
		COMF	_FPbuff+3,F		
		COMF	_FPbuff+4,F		
		INCF	_FPbuff+4,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
		RETLW	0		
	
SETIOV32:		
		BSF		_FPbuff+15,0
		BTFSS	_FPbuff+15,7
		RETLW	0xFF
					
		CLRW
		BTFSS	_FPbuff+14,7
		MOVLW	0xFF		
		MOVWF	_FPbuff+4
		GOTO	SETFOV32A
				
	
//----- Floating point multiply
FPM32:
		MOVF	_FPbuff,W
		BTFSS	_status,Z		
		MOVF	_FPbuff+7,W		
		BTFSC	_status,Z		
		GOTO	RES032		
					
M32BNE0:
		MOVF	_FPbuff+1,W		
		XORWF	_FPbuff+8,W		
		MOVWF	_FPbuff+14
					
		MOVF	_FPbuff+7,W		
		ADDWF	_FPbuff,F		
		MOVLW	0x7E		
		BTFSS	_status,C		
		GOTO	MTUN32		
					
		SUBWF	_FPbuff,F		
		BTFSC	_status,C		
		GOTO	SETFOV32
		GOTO	MOK32		
					
MTUN32:
		SUBWF	_FPbuff,F		
		BTFSS	_status,C		
		GOTO	SETFUN32		
					
MOK32:
		MOVF	_FPbuff+1,W		
		MOVWF	_FPbuff+4		
		MOVF	_FPbuff+2,W		
		MOVWF	_FPbuff+5		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+6		
		BSF		_FPbuff+4,7
		BSF		_FPbuff+8,7		
		BCF		_status,C		
		CLRF	_FPbuff+1
		CLRF	_FPbuff+2		
		CLRF	_FPbuff+3		
		MOVLW	0x18		
		MOVWF	_FPbuff+12
					
MLOOP32:
		BTFSS	_FPbuff+6,0
		GOTO	MNOADD32		
					
MADD32:
		MOVF	_FPbuff+10,W		
		ADDWF	_FPbuff+3,F		
		MOVF	_FPbuff+9,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+9,W		
		ADDWF	_FPbuff+2,F		
					
		MOVF	_FPbuff+8,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+8,W		
		ADDWF	_FPbuff+1,F		
					
MNOADD32:
		RRF		_FPbuff+1,F		
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		RRF		_FPbuff+4,F		
		RRF		_FPbuff+5,F		
		RRF		_FPbuff+6,F		
		BCF		_status,C		
		DECFSZ	_FPbuff+12,F		
		GOTO	MLOOP32		
					
		BTFSC	_FPbuff+1,7
		GOTO	MROUND32		
		RLF		_FPbuff+4,F		
		RLF		_FPbuff+3,F		
		RLF		_FPbuff+2,F		
		RLF		_FPbuff+1,F		
		DECF	_FPbuff,F		
					
MROUND32:
		BTFSC	_FPbuff+15,6		
		BTFSS	_FPbuff+3,0		
		GOTO	MUL32OK		
		BTFSS	_FPbuff+4,7		
		GOTO	MUL32OK		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
		BSF		_status,C
		BTFSS	_status,Z
		GOTO	MUL32OK		
		RRF		_FPbuff+1,F
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		INCF	_FPbuff,F		
		BTFSS	_status,Z
		GOTO	MUL32OK		
					
SETFOV32:		
		BSF		_FPbuff+15,1
		BTFSS	_FPbuff+15,7
		RETLW	0xFF
					
		MOVLW	0xFF		
		MOVWF	_FPbuff
SETFOV32A:
		MOVWF	_FPbuff+1
		MOVWF	_FPbuff+2
		MOVWF	_FPbuff+3		
		RLF		_FPbuff+14,F		
		RRF		_FPbuff+1,F		
		RETLW	0xFF

//-----  Floating Point Divide
FPD32:
		MOVF	_FPbuff+7,W
		BTFSC	_status,Z		
		GOTO	SETFDZ32		
					
		MOVF	_FPbuff,W		
		BTFSC	_status,Z		
		GOTO	RES032		
					
D32BNE0:
		MOVF	_FPbuff+1,W		
		XORWF	_FPbuff+8,W		
		MOVWF	_FPbuff+14
		BSF		_FPbuff+1,7
		BSF		_FPbuff+8,7		
					
TALIGN32:
		CLRF	_FPbuff+12
		MOVF	_FPbuff+1,W		
		MOVWF	_FPbuff+4
		MOVF	_FPbuff+2,W		
		MOVWF	_FPbuff+5		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+6		
					
		MOVF	_FPbuff+10,W		
		SUBWF	_FPbuff+6,F		
		MOVF	_FPbuff+9,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+9,W		
					
TS1ALIGN32:
		SUBWF	_FPbuff+5,F		
		MOVF	_FPbuff+8,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+8,W		
					
TS2ALIGN32:
		SUBWF	_FPbuff+4,F		
					
		CLRF	_FPbuff+4		
		CLRF	_FPbuff+5		
		CLRF	_FPbuff+6		
					
		BTFSS	_status,C		
		GOTO	DALIGN32OK		
					
		BCF		_status,C
		RRF		_FPbuff+1,F		
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		RRF		_FPbuff+4,F		
		MOVLW	0x01		
		MOVWF	_FPbuff+12
					
DALIGN32OK:	
		MOVF	_FPbuff+7,W
		SUBWF	_FPbuff,F		
		BTFSS	_status,C		
		GOTO	ALTB32		
					
AGEB32:
		MOVLW	0x7E		
		ADDWF	_FPbuff+12,W		
		ADDWF	_FPbuff,F		
		BTFSC	_status,C		
		GOTO	SETFOV32		
		GOTO	DARGOK32
					
ALTB32:
		MOVLW	0x7E		
		ADDWF	_FPbuff+12,W		
		ADDWF	_FPbuff,F		
		BTFSS	_status,C		
		GOTO	SETFUN32
					
DARGOK32:
		MOVLW	0x18
		MOVWF	_FPbuff+13		
					
DLOOP32:
		RLF		_FPbuff+6,F
		RLF		_FPbuff+5,F		
		RLF		_FPbuff+4,F		
		RLF		_FPbuff+3,F		
		RLF		_FPbuff+2,F		
		RLF		_FPbuff+1,F		
		RLF		_FPbuff+12,F		
					
		MOVF	_FPbuff+10,W
		SUBWF	_FPbuff+3,F		
		MOVF	_FPbuff+9,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+9,W		
DS132:
		SUBWF	_FPbuff+2,F		
					
		MOVF	_FPbuff+8,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+8,W		
DS232:
		SUBWF	_FPbuff+1,F		
					
		RLF		_FPbuff+8,W		
		IORWF	_FPbuff+12,F		
					
		BTFSS	_FPbuff+12,0
		GOTO	DREST32		
					
		BSF		_FPbuff+6,0		
		GOTO	DOK32		
					
DREST32:
		MOVF	_FPbuff+10,W
		ADDWF	_FPbuff+3,F		
		MOVF	_FPbuff+9,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+9,W		
DAREST32:
		ADDWF	_FPbuff+2,F		
					
		MOVF	_FPbuff+8,W		
		BTFSC	_status,C		
		INCF	_FPbuff+8,W		
		ADDWF	_FPbuff+1,F		
					
		BCF		_FPbuff+6,0		
					
DOK32:
		DECFSZ	_FPbuff+13,F		
		GOTO	DLOOP32		
					
DROUND32:
		BTFSC	_FPbuff+15,6		
		BTFSS	_FPbuff+6,0		
		GOTO	DIV32OK		
		BCF		_status,C		
		RLF		_FPbuff+3,F
		RLF		_FPbuff+2,F
		RLF		_FPbuff+1,F		
		RLF		_FPbuff+12,F		
					
		MOVF	_FPbuff+10,W
		SUBWF	_FPbuff+3,F		
		MOVF	_FPbuff+9,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+9,W		
		SUBWF	_FPbuff+2,F		
					
		MOVF	_FPbuff+8,W		
		BTFSS	_status,C		
		INCFSZ	_FPbuff+8,W		
		SUBWF	_FPbuff+1,F		
					
		RLF		_FPbuff+8,W		
		IORWF	_FPbuff+12,W		
		ANDLW	0x01		
					
		ADDWF	_FPbuff+6,F		
		BTFSC	_status,C		
		INCF	_FPbuff+5,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+4,F		
					
		BSF		_status,C
		BTFSS	_status,Z
		GOTO	DIV32OK		
		RRF		_FPbuff+4,F		
		RRF		_FPbuff+5,F		
		RRF		_FPbuff+6,F		
		INCF	_FPbuff,F		
		BTFSC	_status,Z
		GOTO	SETFOV32		
					
DIV32OK:
		BTFSS	_FPbuff+14,7		
		BCF		_FPbuff+4,7
					
		MOVF	_FPbuff+4,W		
		MOVWF	_FPbuff+1
		MOVF	_FPbuff+5,W		
		MOVWF	_FPbuff+2		
		MOVF	_FPbuff+6,W		
		MOVWF	_FPbuff+3		
					
		RETLW	0		
					
SETFUN32:
		BSF		_FPbuff+15,2
		BTFSS	_FPbuff+15,7
		RETLW	0xFF
					
		MOVLW	0x01
		MOVWF	_FPbuff
		CLRW
		GOTO	SETFOV32A		
					
SETFDZ32:
		BSF		_FPbuff+15,3
		GOTO	SETFOV32

//-----  Floating Point Subtract
FPS32:
		MOVLW	0x80		
		XORWF	_FPbuff+8,F		
					

//-----  Floating Point Add
FPA32:
		MOVF	_FPbuff+1,W
		XORWF	_FPbuff+8,W		
		MOVWF	_FPbuff+12		
					
		CLRF	_FPbuff+4
		CLRF	_FPbuff+11		
					
		MOVF	_FPbuff,W
		SUBWF	_FPbuff+7,W		
		BTFSS	_status,C		
		GOTO	USEA32		
					
		MOVF	_FPbuff+7,W
		MOVWF	_FPbuff+6
		MOVF	_FPbuff,W		
		MOVWF	_FPbuff+7		
		MOVF	_FPbuff+6,W		
		MOVWF	_FPbuff		
					
		MOVF	_FPbuff+8,W		
		MOVWF	_FPbuff+6		
		MOVF	_FPbuff+1,W		
		MOVWF	_FPbuff+8		
		MOVF	_FPbuff+6,W		
		MOVWF	_FPbuff+1		
					
		MOVF	_FPbuff+9,W		
		MOVWF	_FPbuff+6		
		MOVF	_FPbuff+2,W		
		MOVWF	_FPbuff+9		
		MOVF	_FPbuff+6,W		
		MOVWF	_FPbuff+2		
					
		MOVF	_FPbuff+10,W		
		MOVWF	_FPbuff+6		
		MOVF	_FPbuff+3,W		
		MOVWF	_FPbuff+10		
		MOVF	_FPbuff+6,W		
		MOVWF	_FPbuff+3		
					
USEA32:
		MOVF	_FPbuff+7,W
		BTFSC	_status,Z		
		RETLW	0x00		
					
		MOVF	_FPbuff+1,W		
		MOVWF	_FPbuff+14
		BSF		_FPbuff+1,7
		BSF		_FPbuff+8,7		
					
		MOVF	_FPbuff+7,W
		SUBWF	_FPbuff,W		
		MOVWF	_FPbuff+7		
		BTFSC	_status,Z		
		GOTO	ALIGNED32		
					
		MOVLW	8		
		SUBWF	_FPbuff+7,W		
		BTFSS	_status,C
		GOTO	ALIGNB32		
		MOVWF	_FPbuff+7		
		MOVF	_FPbuff+10,W
		MOVWF	_FPbuff+11		
		MOVF	_FPbuff+9,W		
		MOVWF	_FPbuff+10		
		MOVF	_FPbuff+8,W		
		MOVWF	_FPbuff+9		
		CLRF	_FPbuff+8		
					
		MOVLW	8		
		SUBWF	_FPbuff+7,W		
		BTFSS	_status,C
		GOTO	ALIGNB32		
		MOVWF	_FPbuff+7		
		MOVF	_FPbuff+10,W
		MOVWF	_FPbuff+11		
		MOVF	_FPbuff+9,W		
		MOVWF	_FPbuff+10		
		CLRF	_FPbuff+9		
					
		MOVLW	8		
		SUBWF	_FPbuff+7,W		
		BTFSS	_status,C
		GOTO	ALIGNB32		
		MOVF	_FPbuff+14,W		
		MOVWF	_FPbuff+1		
		RETLW	0x00		
					
ALIGNB32:
		MOVF	_FPbuff+7,W
		BTFSC	_status,Z		
		GOTO	ALIGNED32		
					
ALOOPB32:		
		BCF		_status,C
		RRF		_FPbuff+8,F		
		RRF		_FPbuff+9,F		
		RRF		_FPbuff+10,F		
		RRF		_FPbuff+11,F		
		DECFSZ	_FPbuff+7,F		
		GOTO	ALOOPB32		
					
ALIGNED32:
		BTFSS	_FPbuff+12,7
		GOTO	AOK32		
					
		COMF	_FPbuff+11,F		
		COMF	_FPbuff+10,F		
		COMF	_FPbuff+9,F		
		COMF	_FPbuff+8,F		
		INCF	_FPbuff+11,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+10,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+9,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+8,F		
					
AOK32:					
		MOVF	_FPbuff+11,W		
		ADDWF	_FPbuff+4,F		
		MOVF	_FPbuff+10,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+10,W		
		ADDWF	_FPbuff+3,F		
		MOVF	_FPbuff+9,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+9,W		
		ADDWF	_FPbuff+2,F		
		MOVF	_FPbuff+8,W		
		BTFSC	_status,C		
		INCFSZ	_FPbuff+8,W		
		ADDWF	_FPbuff+1,F		
					
		BTFSC	_FPbuff+12,7		
		GOTO	ACOMP32		
		BTFSS	_status,C		
		GOTO	NRMRND4032		
					
		RRF		_FPbuff+1,F
		RRF		_FPbuff+2,F		
		RRF		_FPbuff+3,F		
		RRF		_FPbuff+4,F		
		INCFSZ	_FPbuff,F		
		GOTO	NRMRND4032		
		GOTO	SETFOV32		
					
ACOMP32:
		BTFSC	_status,C		
		GOTO	NRM4032
					
		COMF	_FPbuff+4,F		
		COMF	_FPbuff+3,F
		COMF	_FPbuff+2,F
		COMF	_FPbuff+1,F		
		INCF	_FPbuff+4,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+3,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+2,F		
		BTFSC	_status,Z		
		INCF	_FPbuff+1,F		
					
		MOVLW	0x80		
		XORWF	_FPbuff+14,F		
		GOTO	NRM32		
	}
}

single int2float(long ii)
{
	asm
	{
		MOVF	_ii,W
		MOVWF	_FPbuff+4
		MOVF	_ii+1,W
		MOVWF	_FPbuff+3
		MOVF	_ii+2,W
		MOVWF	_FPbuff+2
		MOVF	_ii+3,W
		MOVWF	_FPbuff+1
	}
	FPmath(0);
	asm
	{
		MOVF	_FPbuff+3,W
		MOVWF	_ii
		MOVF	_FPbuff+2,W
		MOVWF	_ii+1
		MOVF	_FPbuff+1,W
		MOVWF	_ii+2
		MOVF	_FPbuff,W
		MOVWF	_ii+3
	}
	return ii;
}

long float2int(single xx, char round)
{
asm
	{
		MOVF	_xx,W
		MOVWF	_FPbuff+3
		MOVF	_xx+1,W
		MOVWF	_FPbuff+2
		MOVF	_xx+2,W
		MOVWF	_FPbuff+1
		MOVF	_xx+3,W
		MOVWF	_FPbuff
	}
	if (round) FPmath(1); else FPmath(2);
	asm
	{
		MOVF	_FPbuff+4,W
		MOVWF	_xx
		MOVF	_FPbuff+3,W
		MOVWF	_xx+1
		MOVF	_FPbuff+2,W
		MOVWF	_xx+2
		MOVF	_FPbuff+1,W
		MOVWF	_xx+3
	}
	return xx;
}

single addfloat(single xx,single yy)
{
asm
	{
		MOVF	_xx,W
		MOVWF	_FPbuff+3
		MOVF	_xx+1,W
		MOVWF	_FPbuff+2
		MOVF	_xx+2,W
		MOVWF	_FPbuff+1
		MOVF	_xx+3,W
		MOVWF	_FPbuff
		MOVF	_yy,W
		MOVWF	_FPbuff+10
		MOVF	_yy+1,W
		MOVWF	_FPbuff+9
		MOVF	_yy+2,W
		MOVWF	_FPbuff+8
		MOVF	_yy+3,W
		MOVWF	_FPbuff+7		
	}
	FPmath(3);
	asm
	{
		MOVF	_FPbuff+3,W
		MOVWF	_xx
		MOVF	_FPbuff+2,W
		MOVWF	_xx+1
		MOVF	_FPbuff+1,W
		MOVWF	_xx+2
		MOVF	_FPbuff,W
		MOVWF	_xx+3
	}
	return xx;
}

single subfloat(single xx,single yy)
{
asm
	{
		MOVF	_xx,W
		MOVWF	_FPbuff+3
		MOVF	_xx+1,W
		MOVWF	_FPbuff+2
		MOVF	_xx+2,W
		MOVWF	_FPbuff+1
		MOVF	_xx+3,W
		MOVWF	_FPbuff
		MOVF	_yy,W
		MOVWF	_FPbuff+10
		MOVF	_yy+1,W
		MOVWF	_FPbuff+9
		MOVF	_yy+2,W
		MOVWF	_FPbuff+8
		MOVF	_yy+3,W
		MOVWF	_FPbuff+7		
	}
	FPmath(4);
	asm
	{
		MOVF	_FPbuff+3,W
		MOVWF	_xx
		MOVF	_FPbuff+2,W
		MOVWF	_xx+1
		MOVF	_FPbuff+1,W
		MOVWF	_xx+2
		MOVF	_FPbuff,W
		MOVWF	_xx+3
	}
	return xx;
}

single mulfloat(single xx,single yy)
{
asm
	{
		MOVF	_xx,W
		MOVWF	_FPbuff+3
		MOVF	_xx+1,W
		MOVWF	_FPbuff+2
		MOVF	_xx+2,W
		MOVWF	_FPbuff+1
		MOVF	_xx+3,W
		MOVWF	_FPbuff
		MOVF	_yy,W
		MOVWF	_FPbuff+10
		MOVF	_yy+1,W
		MOVWF	_FPbuff+9
		MOVF	_yy+2,W
		MOVWF	_FPbuff+8
		MOVF	_yy+3,W
		MOVWF	_FPbuff+7		
	}
	FPmath(5);
	asm
	{
		MOVF	_FPbuff+3,W
		MOVWF	_xx
		MOVF	_FPbuff+2,W
		MOVWF	_xx+1
		MOVF	_FPbuff+1,W
		MOVWF	_xx+2
		MOVF	_FPbuff,W
		MOVWF	_xx+3
	}
	return xx;
}

single divfloat(single xx,single yy)
{
asm
	{
		MOVF	_xx,W
		MOVWF	_FPbuff+3
		MOVF	_xx+1,W
		MOVWF	_FPbuff+2
		MOVF	_xx+2,W
		MOVWF	_FPbuff+1
		MOVF	_xx+3,W
		MOVWF	_FPbuff
		MOVF	_yy,W
		MOVWF	_FPbuff+10
		MOVF	_yy+1,W
		MOVWF	_FPbuff+9
		MOVF	_yy+2,W
		MOVWF	_FPbuff+8
		MOVF	_yy+3,W
		MOVWF	_FPbuff+7		
	}
	FPmath(6);
	asm
	{
		MOVF	_FPbuff+3,W
		MOVWF	_xx
		MOVF	_FPbuff+2,W
		MOVWF	_xx+1
		MOVF	_FPbuff+1,W
		MOVWF	_xx+2
		MOVF	_FPbuff,W
		MOVWF	_xx+3
	}
	return xx;
}

single absfloat(single xx)
{
asm
	{
		BCF		_xx+2,7
	}
	return xx;
}

char xGTyfloat(single xx, single yy)
{
    char bb=0xFF;
	asm
	{
TAGTB32:	
		MOVF		_yy+2,W
		XORWF		_xx+2,W
		ANDLW		0x80
		BTFSS		_status,Z
		GOTO		TAGTB32O

		BTFSC		_yy+2,7
		GOTO		TAGTB32U

TAGTB32P:
		MOVF		_yy+3,W
		SUBWF		_xx+3,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_yy+2,W
		SUBWF		_xx+2,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_yy+1,W
		SUBWF		_xx+1,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_yy,W
		SUBWF		_xx,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y
		GOTO		TAGTB32N

TAGTB32U:	
		MOVF		_xx+3,W
		SUBWF		_yy+3,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_xx+2,W
		SUBWF		_yy+2,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_xx+1,W
		SUBWF		_yy+1,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y

		MOVF		_xx,W
		SUBWF		_yy,W
		BTFSS		_status,C
		GOTO		TAGTB32N
		BTFSS		_status,Z
		GOTO		TAGTB32Y
		GOTO		TAGTB32N

TAGTB32O:
		BTFSS		_xx+2,7
		GOTO		TAGTB32Y
TAGTB32N:
		CLRF		_bb
TAGTB32Y:		
	}
	return bb;
}

#ifdef TEST_FPMATH

void main()
{
	unsigned long floatmin=0x6E000000;  //7,62939453125E-6
	single x,y,z,r;
	signed long k,i,j;

	r=0;
	y=int2float(100);
	i=-100;
	k = -1;
	while (i<100)
	{
		z=int2float(i);  // z = i
		x=divfloat(z,y);  // x = i / 100
		x=mulfloat(x,y);  // x *= 100; should be nearly = i again.
		z=subfloat(x,z);  // z = i - i = 0
		z=absfloat(z);  // z represents the error.
		if (xGTyfloat(z,r)) {r=z; k=i;}  // z gets the maximum error, and k gets the iteration it happened on.
		i++;
	}
	
	z = mulfloat(z, y);  // z = z * 1000
	j = float2int(z, 1);  // j = z * 1000, rounded.
	
	// Break here.  j is the maximum error * 1000, and k is the iteration.
	x = 0;
	
	y=int2float(10);  // y = 10
	i=-100;
	while (i<100)
	{
		x=int2float(i);  // x = i, [-100 .. 99]
		j=float2int(x,1);  // j = x, rounded to an integer.
		if (i!=j) 
			break;  // check that conversion to and from returns the same value.
		x=divfloat(x,y);  // x = x / 10
		j=float2int(x,1);  // j = i / 10, rounded to an integer.
		k=float2int(x,0);  // k = 1 / 10, truncated to an integer.
		i++;
	}	
	
	x=addfloat(x,y);  // x = 99/10 + 10 = 9.9 + 10 = 19.9
	j = float2int(x, 1);  // j = 20
	k = float2int(x, 0);  // k = 19
	
	x = 0;
}

#endif