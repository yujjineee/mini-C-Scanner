/***************************************************************
*      scanner routine for Mini C language                    *
*                                   2003. 3. 10               *
***************************************************************/

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define NO_KEYWORD 17
#define ID_LENGTH 12
#define MAX 100

static int _line = 1;
static int _col = 1;


struct tokenType {
	int number;
	union {
		char id[ID_LENGTH]; // 문자형 identifier 담음
		char symbol[ID_LENGTH]; // 기호형 symbol 담음
		int num;
		int col, line;
		int comment;
		char text[MAX];
	} value;
};


enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1            2         3            4          5     */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7            8         9           10         11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14        15           16         17     */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18         19          20        21           22         23     */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24         25          26        27           28         29     */
	tcol,
	/* 30                                                              */
	//   ...........    word symbols ................................. //
	/* 31          32        33           34         35         36     */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 37          38        39           40         41         42     */
	twhile, tlbrace, tor, trbrace, tchar, tdouble,
	/* 43          44        45           46         47         48     */
	tfor, tdo, tgoto, tswitch, tcase, tbreak,
	/* 49        50         51          52         53          54      */
	tdefault, tcharlit, tstringlit, tdoublelit, tdocuCom, tsingleCom,
	/* 55 */
	tcomment
};

struct tokenType scanner();
void printToken(struct tokenType token);
void printComment(struct tokenType token);