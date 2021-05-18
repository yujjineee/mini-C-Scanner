// ==============================================
// 형식언어 과제 #03 mini-C Scanner
// 2019112062 이유진
// ==============================================

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cstring>

#include "Scanner.h"

extern FILE *sourceFile;                       // miniC source program


int superLetter(char ch); // isalpha(ch) : ch가 알파벳이거나 _이면 return 1
int superLetterOrDigit(char ch); // isalnum(ch) : ch가 숫자거나 알파벳이거나 _이면 return 1
int getNumber(char firstCharacter); // 10진수 숫자로 변환하는 함수
int hexValue(char ch); // 16진수 문자(char)를 10진수 숫자(int)값으로 가져오는 함수
void lexicalError(int n); // 오류 출력 함수


char *tokenName[] = {
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11        */
	"++",       "+=",      ",",       "-",      "--",	    "-=",
	/* 12         13         14        15         16         17        */
	"/",        "/=",      ";",       "<",      "<=",       "=",
	/* 18         19         20        21         22         23        */
	"==",       ">",       ">=",      "[",      "]",        "eof",
	/* 24         25         26        27         28         29        */
	":"
	/* 30                                                              */
	//   ...........    word symbols ................................. //
	/* 31         32         33          34          35        36      */
	"const",    "else",     "if",      "int",     "return",  "void",
	/* 37         38         39         40         41          42      */
	"while",    "{",        "||",       "}",	 "char",	"double",
	/* 43        44         45           46         47         48      */
	"for",		"do",		"goto",	   "switch", "case",	"break", 
	/* 49                                                              */
	"default"
};

char *keyword[NO_KEYWORD] = {
	"const",  "else",    "if",    "int",    "return",  "void",    "while",
	"char", "double", "for", "do", "goto", "switch", "case", "break", "default"
};

enum tsymbol tnum[NO_KEYWORD] = {
	tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tchar, tdouble, tfor, tdo, tgoto, tswitch, tcase, tbreak, tdefault
};

struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];


	token.number = tnull;

	do {
		while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
		
		// isspace(int n); -> n이 공백인지 아닌지 판단, 공백이 아니면 0 return
		// 즉, while문은 sourceFile에서 받아온 문자가 공백이 아닌 동안 반복
		// " ", "\n", "\t", "\v", "\f", "\r"을 공백으로 판단
		// -> 단어마다, 문장마다 새롭게 인식
		
		if (superLetter(ch)) { // identifier or keyword

			// 단어 시작 ch가 알파벳이거나 _일 때

			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
			} while (superLetterOrDigit(ch));
			// 이어진 문자가 숫자, 알파벳, _이고, 단어 길이가 12 미만
			// 문자 받아와서 id[]에 저장

			if (i >= ID_LENGTH) lexicalError(1); // 길이가 넘었을 때 에러
			id[i] = '\0'; // 마지막 문자 초기화

			ungetc(ch, sourceFile);  //  retract : 되돌림
									 // find the identifier in the keyword table
			
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;
			// 받아온 id를 keyword 속 토큰과 비교
			// 같은 토큰 찾으면 for 문 탈출

			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];
			// 키워드 찾아서 해당 토큰 번호를 struct의 number에 넣어줌
			
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy_s(token.value.id, id);
			} 
			// 토큰 목록에 없으면 number에 tident(== 4), value.id에 단어 복사
		}  // end of identifier or keyword
		// identifier나 단어 인식 완료

		else if (isdigit(ch)) {  // number
			token.number = tnumber;
			token.value.num = getNumber(ch);
		}
		// 단어 시작이 숫자인 경우
		// 10진수 형식으로 바꿔서
		// number에 tnumber(== 5), value.number에 변환한 숫자 저장




		else switch (ch) {  // special character

			// 일반적인 단어나 숫자가 아닌 경우

		case '/':
			ch = fgetc(sourceFile);
			if (ch == '*')			// text comment
				do {
					while (ch != '*') ch = fgetc(sourceFile);
					ch = fgetc(sourceFile);
				} while (ch != '/');
			else if (ch == '/')		// line comment
				while (fgetc(sourceFile) != '\n');
			else if (ch == '=')  token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
			}
			else {
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;

		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&')  token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+')  token.number = tinc;
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-')  token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '<':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tlesse;
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|')  token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '(': token.number = tlparen;         break;
		case ')': token.number = trparen;         break;
		case ',': token.number = tcomma;          break;
		case ';': token.number = tsemicolon;      break;
		case '[': token.number = tlbracket;       break;
		case ']': token.number = trbracket;       break;
		case '{': token.number = tlbrace;         break;
		case '}': token.number = trbrace;         break;
		case EOF: token.number = teof;            break;
		case ':': token.number = tcol;			  break;
		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}
		} // switch end

	} while (token.number == tnull);

	return token;

} // end of scanner

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &\n");
		break;
	case 3: printf("next character must be |\n");
		break;
	case 4: printf("invalid character\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1; // ch가 알파벳이거나 _이면 return 1
	else return 0; // ch가 알파벳이 아니면 return 0
}
// isalpha(int n); -> 아스키 코드로 변환된 n이 알파벳이 아니면 0 return
// A~Z / 아스키 코드 숫자 65~90 : return 1;
// a~z는 / 아스키 코드 숫자 97~122 : return 2;

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1; // ch가 알파벳, 숫자, _이면 return 1
	else return 0; // ch가 알파벡, 숫자, _중 아무것도 아니면 return 0 (ex. =, ==, %, ;..)
}
// isalnum(int n);
// -> 아스키 코드로 변환한 n이 알파벳 또는 숫자가 아니면 0 return

int getNumber(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter == '0') {
		// 숫자 표기의 첫 문자로 0이 들어왔을 때: 16진수, 8진수, 그냥 0의 경우
		ch = fgetc(sourceFile); // 다음 문자 가져옴
		if ((ch == 'X') || (ch == 'x')) { // hexa decimal : 0X__형식
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
				num = 16 * num + value;
		}
			// 다음 문자를 가져와서 hexValue에 넣은 값 value
			// 16진수 문자가 더이상 나오지 않을 때까지 반복
			// 10진수 값 num으로 변환

		else if ((ch >= '0') && (ch <= '7'))	// octal : 0___ 형식
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
			} while ((ch >= '0') && (ch <= '7'));
			// 8진수가 끝날 때까지 반복
			// 10진수 값 num으로 변환

		else num = 0;						// zero : 그냥 0 일 때
	}
	else {									// decimal : 일반 10진수
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(sourceFile);
		} while (isdigit(ch));
	}
	// isdigit(int n) -> 아스키 코드로 변환된 n이 0~9까지의 숫자이면 return 1

	ungetc(ch, sourceFile);  /*  retract  */
	// 읽어온 문자를 다시 돌려놓는다.
	return num;
	// 변환한 10진수 형식으로 return
}

int hexValue(char ch)
{	// 16진수 문자(char)를 10진수 숫자(int)값으로 가져오는 함수

	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

void printToken(struct tokenType token)
{
	if (token.number == tident)
		printf("number: %d, value: %s\n", token.number, token.value.id);
	else if (token.number == tnumber)
		printf("number: %d, value: %d\n", token.number, token.value.num);
	// else if (token.number == tdocuCom || token.number == tsingleCom)
	// printf("number: %d, value: %s\n", token.number, token.value.comment);
	else
		printf("number: %d(%s)\n", token.number, tokenName[token.number]);

}