// ==============================================
// ���ľ�� ���� #03 mini-C Scanner
// 2019112062 ������
// ==============================================

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cstring>

#include "Scanner.h"

extern FILE *sourceFile;                       // miniC source program


int superLetter(char ch); // isalpha(ch) : ch�� ���ĺ��̰ų� _�̸� return 1
int superLetterOrDigit(char ch); // isalnum(ch) : ch�� ���ڰų� ���ĺ��̰ų� _�̸� return 1
int getNumber(char firstCharacter); // 10���� ���ڷ� ��ȯ�ϴ� �Լ�
int hexValue(char ch); // 16���� ����(char)�� 10���� ����(int)������ �������� �Լ�
void lexicalError(int n); // ���� ��� �Լ�

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
	":",
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
	"char", "double", "for", "do", "goto", "switch", "case", "break", "default", "comment"
};

enum tsymbol tnum[NO_KEYWORD] = {
	tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tchar, tdouble, tfor, tdo, tgoto, tswitch, tcase, tbreak, tdefault, tcomment
};

struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fgetc(sourceFile))) {	// state 1: skip blanks
		// isspace(int n); -> n�� �������� �ƴ��� �Ǵ�, ������ �ƴϸ� 0 return
		// ��, while���� sourceFile���� �޾ƿ� ���ڰ� *������ �ƴ� ���� �ݺ�*
		// " ", "\n", "\t", "\v", "\f", "\r"�� �������� �Ǵ�
		// -> �ܾ��, ���帶�� ���Ӱ� �ν�

			if (ch == '\n') {
				// printf("���ͳ�����? >< \n");
				_line++;  // ���͸� _line + 1
				_col = 1;
			}
			else _col++; // ���Ͱ� �ƴϸ� ��ĭ++
		} // ���� �������� ��� ����

		if (superLetter(ch)) { // identifier or keyword
		// �ܾ� ���� ch�� ���ĺ��̰ų� _�� ��
			token.value.line = _line;
			token.value.col = _col;
			

			i = 0;


			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
				_col++;
			} while (superLetterOrDigit(ch));

			// �̾��� ���ڰ� ����, ���ĺ�, _�̰�, �ܾ� ���̰� 12 �̸�
			// ���� �޾ƿͼ� id[]�� ����

			if (i >= ID_LENGTH) lexicalError(1); // ���̰� �Ѿ��� �� ����
			id[i] = '\0'; // ������ ���� �ʱ�ȭ

			ungetc(ch, sourceFile);  //  retract : �ǵ���
									 // find the identifier in the keyword table
			
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;

			// �޾ƿ� id�� keyword �� ��ū�� ��
			// ���� ��ū ã���� for �� Ż��

			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];

			// Ű���� ã�Ƽ� �ش� ��ū ��ȣ�� struct�� number�� �־���
			
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy_s(token.value.id, id);
			} 

			// ��ū ��Ͽ� ������ number�� tident(== 4), value.id�� �ܾ� ����
		}  // end of identifier or keyword
		// identifier�� �ܾ� �ν� �Ϸ�


		// ������ ���
		else if (isdigit(ch)) {  // number
			token.number = tnumber;
			token.value.num = getNumber(ch);
			token.value.line = _line;
			token.value.col = _col;			
		}
		// �ܾ� ������ ������ ���
		// 10���� �������� �ٲ㼭
		// number�� tnumber(== 5), value.number�� ��ȯ�� ���� ����


		else switch (ch) {  // special character
			// �Ϲ����� �ܾ ���ڰ� �ƴ� ���

		case '/':
			ch = fgetc(sourceFile);
			_col++;

			// == text comment: ���� �ּ� ==
			if (ch == '*') {

				char txt[MAX];
				i = 0;
				ch = fgetc(sourceFile);
				_col++;

					token.value.comment = 1; // ���� �ּ��� �ڵ� 1

					while (ch != '/') {
						ch = fgetc(sourceFile);
						txt[i] = ch;
						i++;
						_col++;
						if (ch == '\n') {
							_col = 1;
							_line++;
						}
					}

					txt[i] = '\0';
					strcpy_s(token.value.text, txt);
					ungetc(ch, sourceFile);
					token.number = tcomment;
			}

			// == line comment: �� �ּ� ==
			else if (ch == '/') {

				char txt[MAX];
				i = 0;
				token.value.comment = 2;
				ch = fgetc(sourceFile);
				while (ch != '\n') {
					ch = fgetc(sourceFile);
					txt[i] = ch;
					i++;
					_col++;
				}
				txt[i] = '\0';
				strcpy_s(token.value.text, txt);
				ungetc(ch, sourceFile);
				token.number = tcomment;
			}

			// == '/=' ������ ==
			else if (ch == '=') {
				token.number = tdivAssign;
				_col++;
			}

			// == '/' �� �� ==
			else {
				token.number = tdiv;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tnotequ;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tnot;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tremainder;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);
			}
			break;

		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&') {
				token.number = tand;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				lexicalError(2);
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tmulAssign;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tmul;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+') {
				token.number = tinc;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else if (ch == '=') {
				token.number = taddAssign;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tplus;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-') {
				token.number = tdec;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else if (ch == '=') {
				token.number = tsubAssign;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tminus;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '<':
			ch = fgetc(sourceFile);
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			if (ch == '=') {
				token.number = tlesse; 
			}
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tequal;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tassign;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tgreate;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				token.number = tgreat;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|') {
				token.number = tor;
				token.value.line = _line;
				token.value.col = _col;
				_col++;
			}
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;


		case '(': 
			token.number = tlparen;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case ')': 
			token.number = trparen;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case ',': 
			token.number = tcomma;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case ';': 
			token.number = tsemicolon;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case '[': 
			token.number = tlbracket;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case ']': 
			token.number = trbracket;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case '{': 
			token.number = tlbrace;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case '}': 
			token.number = trbrace;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case ':':
			token.number = tcol;
			token.value.line = _line;
			token.value.col = _col;
			_col++;
			break;

		case EOF: 
			token.number = teof;
			break;

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
	if (isalpha(ch) || ch == '_') return 1; // ch�� ���ĺ��̰ų� _�̸� return 1
	else return 0; // ch�� ���ĺ��� �ƴϸ� return 0
}
// isalpha(int n); -> �ƽ�Ű �ڵ�� ��ȯ�� n�� ���ĺ��� �ƴϸ� 0 return
// A~Z / �ƽ�Ű �ڵ� ���� 65~90 : return 1;
// a~z�� / �ƽ�Ű �ڵ� ���� 97~122 : return 2;

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1; // ch�� ���ĺ�, ����, _�̸� return 1
	else return 0; // ch�� ���ĺ�, ����, _�� �ƹ��͵� �ƴϸ� return 0 (ex. =, ==, %, ;..)
}
// isalnum(int n);
// -> �ƽ�Ű �ڵ�� ��ȯ�� n�� ���ĺ� �Ǵ� ���ڰ� �ƴϸ� 0 return

int getNumber(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter == '0') {
		// ���� ǥ���� ù ���ڷ� 0�� ������ ��: 16����, 8����, �׳� 0�� ���
		ch = fgetc(sourceFile); // ���� ���� ������
		_col++;

		if ((ch == 'X') || (ch == 'x')) { // hexa decimal : 0X__����
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
				_col++;
				num = 16 * num + value;
		}
			// ���� ���ڸ� �����ͼ� hexValue�� ���� �� value
			// 16���� ���ڰ� ���̻� ������ ���� ������ �ݺ�
			// 10���� �� num���� ��ȯ

		else if ((ch >= '0') && (ch <= '7'))	// octal : 0___ ����
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
				_col++;
			} while ((ch >= '0') && (ch <= '7'));
			// 8������ ���� ������ �ݺ�
			// 10���� �� num���� ��ȯ

		else num = 0;						// zero : �׳� 0 �� ��
	}
	else {									// decimal : �Ϲ� 10����
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(sourceFile);
			_col++;
		} while (isdigit(ch));
	}
	// isdigit(int n) -> �ƽ�Ű �ڵ�� ��ȯ�� n�� 0~9������ �����̸� return 1

	ungetc(ch, sourceFile);  /*  retract  */
	// �о�� ���ڸ� �ٽ� �������´�.
	return num;
	// ��ȯ�� 10���� �������� return
}

int hexValue(char ch)
{	// 16���� ����(char)�� 10���� ����(int)������ �������� �Լ�

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
		printf("%d   \t%s \t%d  \t%d\n", token.number, token.value.id, token.value.line, token.value.col);
	else if (token.number == tnumber)
		printf("%d   \t%d \t%d  \t%d\n", token.number, token.value.num, token.value.line, token.value.col);
	else
		printf("%d   \t%s \t%d  \t%d\n", token.number, tokenName[token.number], token.value.line, token.value.col);

}

void printComment(struct tokenType token)
{
	printf("%s \n", token.value.text);
}

/*
void printToken(struct tokenType token)
{
	if (token.number == tident) // ����
		printf("number: %d, value: %s, line number: %d, col number: %d\n", token.number, token.value.id, token.value.line, token.value.col);
	else if (token.number == tnumber) // ����
		printf("number: %d, value: %d, line number: %d, col number: %d\n", token.number, token.value.num, token.value.line, token.value.col);
	// else if (token.number == tdocuCom || token.number == tsingleCom)
	// printf("number: %d, value: %s, line number: %d, col number: %d\n", token.number, token.value.comment, token.value.line, token.value.col);
	else // �� ��
		printf("number: %d(%s), line number: %d, col number: %d\n", token.number, tokenName[token.number], token.value.line, token.value.col);

}
*/