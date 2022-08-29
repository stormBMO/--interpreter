#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#define BUFSIZE 80

char TW[8][6] = {"begin","end","for","to","or","next","and","not"};
char TD[14][3] = { "(", ")", "/", ";", "<", ">", "=", "*", "+", "-", "!=", ":=", "<=", ">="};

char buf[BUFSIZE];
char c;
char d;

typedef struct TID{
	char word[256];
	struct TID* next;
} TID;

typedef struct TNUM{
	int number;
	struct TNUM* next;
} TNUM;


void printTDorTW(char type) { 
	if (type == 'W') {
		printf("1)TW:\n");
		for (int i = 0; i < 8; ++i)
			printf("%d - %s\n", i + 1, TW[i]);
		printf("\n");
	}
	if (type == 'D') {
		printf("2)TD:\n");
		for (int i = 0; i < 14; ++i)
			printf("%d - %s\n", i + 1, TD[i]);
		printf("\n");
	}
}

void printTIDorTNUM(char type, TID* ti, TNUM* tn) {
	int len = 1;
	if (type == 'I') {
		printf("\n3)TID:\n");
		for (ti; ti->next != NULL; ti = ti->next, ++len)
			printf("%d. %s\n", len, ti->word);
	}
	else if (type == 'N') {
		printf("\n4)TNUM:\n");
		for (tn; tn->next != NULL; tn = tn->next, ++len)
			printf("%d. %d\n", len, tn->number);
	}
}

int findID(TID* head) {
	TID* el = head;
	for (int i = 1; el != NULL; ++i) {
		if (!strcmp(el->word, buf))
			return i;
		el = el->next;
	}
	return -1;
}

int findNUM(TNUM* head, int d) {
	TNUM* el = head;
	for (int i = 1; el != NULL; ++i) {
		if (el->number == d)
			return i;
		el = el->next;
	}
	return -1;
}

void clear() {
	for (int i = 0; i < BUFSIZE; ++i) {
		buf[i] = '\0';
	}
}


int lookTW() {
	for (int i = 0; i < 8; ++i)
		if (!strcmp(TW[i], buf))
			return i + 1;
	return -1;
}

int lookTD(int n) {
	for (int i = 0; i < 14; i++)
		if (!strcmp(TD[i], buf))
			return i+1;
	return -1;
}


void main(int argc, char* argv[]){
	int ercode = 0, str = 1, str_, q = 1;
	char ersymb;
	
	enum state {
		H, ID, NUM, COM, ASS, DLM, ER, AS, NOT, IN
	} TC;
	
	
	FILE* program;

	TID* tID, * headID;
	TNUM* tNUM, * headNUM;

	if ((program = fopen(argv[1], "rt")) == NULL){
		printf("���� �� ������");
		return;
	}
	printf("TW - 1\nTD - 2\nTID - 3\nTNUM - 4\n\n");
	TC = H;
	tID = (struct TID*)malloc(sizeof(TID));
	tNUM = (struct TNUM*)malloc(sizeof(TNUM));
	tID->next = NULL;
	tNUM->next = NULL;
	headID = tID;
	headNUM = tNUM;
	c = fgetc(program);
	do {
		if (feof(program)) {
			printf("\n\n");
			printTDorTW('W');
			printTDorTW('D');
			printTIDorTNUM('I', headID, NULL);
			printTIDorTNUM('N', NULL, headNUM);
			return;
		}
		if (c == ' ')
			c = fgetc(program);
		while (c == '\n') {
			printf("\n");
			str++;
			c = fgetc(program);
		}
		if (isalpha(c)) {
			buf[0] = c;
			c = fgetc(program);
			TC = ID;
		}
		else if (isdigit(c)) {
			d = c - '0';
			c = fgetc(program);
			TC = NUM;
		}
		else if (c == '/') {
			buf[0] = c;
			c = fgetc(program);
			TC = ASS;
			if (c == '*') {
				c = fgetc(program);
				TC = COM;
			}
		}
		else if (c == ':') {
			buf[0] = c;
			c = fgetc(program);
			TC = AS;
		}
		else if (c == '!') {
			buf[0] = c;
			c = fgetc(program);
			TC = NOT;
		}
		else if (c == '<' || c == '>') {
			buf[0] = c;
			c = fgetc(program);
			TC = IN;
		}
		else
			TC = DLM;
		switch (TC)	{
		case ID:
			while (isalpha(c) || isdigit(c)) {
				buf[q] = c;
				q++;
				c = fgetc(program);
			}
			if (lookTW() >= 0) {
				printf("<1, %d> ", lookTW());
				q = 1;
				clear();
			}
			else if (findID(headID) < 0) {
				strcpy(tID->word, buf);
				printf("<3, %d> ", findID(headID));
				tID->next = malloc(sizeof(TID));
				tID = tID->next;
				tID->next = NULL;
				q = 1;
				clear();
			}
			else {
				printf("<3, %d> ", findID(headID));
				q = 1;
				clear();
			}
			TC = H;
			break;
		case NUM:
			while (isdigit(c)) {
				d = d * 10 + (c - '0');
				c = fgetc(program);
			}
			if (findNUM(headNUM, d) >= 0)
				printf("<4, %d> ", findNUM(headNUM, d));
			else {
				tNUM->number = d;
				tNUM->next = malloc(sizeof(TNUM));
				printf("<4, %d> ", findNUM(headNUM, d));
				tNUM = tNUM->next;
				tNUM->next = NULL;
			}
			TC = H;
			break;
		case COM:
			str_ = str;
			while (!feof(program)) {
				if (c == '\n') {
					str++;
					c = fgetc(program);
				}
				if (c == '*') {
					c = fgetc(program);
					if (c == '/') {
						c = fgetc(program);
						TC = H;
						break;
					}
					if (c == '\n') {
						str++;
						c = fgetc(program);
					}
				}
				else
					c = fgetc(program);
			}
			if (feof(program) && TC == COM) {
				ercode = 1;
				TC = ER;
				break;
			}
			break;
		case ASS:
			if (lookTD(1) >= 0)
				printf("<2, %d> ", lookTD(1));
			TC = H;
			break;
		case DLM:
			if (c == '+' || c == '-' || c == ';' || c == '*' || c == '(' || c == ')' || c == '=') {
				buf[0] = c;
				c = fgetc(program);
				if (lookTD(1) >= 0)
					printf("<2, %d> ", lookTD(1));
				TC = H;
			}
			else {
				ersymb = c;
				ercode = 2;
				TC = ER;
				break;
			}
			clear();
			break;
		case AS:
			if (c == '=') {
				buf[1] = c;
				c = fgetc(program);
				if (lookTD(2) >= 0)
					printf("<2, %d> ", lookTD(2));
				TC = H;
				break;
			}
			else {
				ercode = 4;
				TC = ER;
				break;
			}
			break;
		case NOT:
			if (c == '=') {
				buf[1] = c;
				c = fgetc(program);
				if (lookTD(2) >= 0)
					printf("<2, %d> ", lookTD(2));
				TC = H;
				break;
			}
			else {
				ercode = 3;
				TC = ER;
				break;
			}
			break;
		case IN:
			if (c == '=') {
				buf[1] = c;
				c = fgetc(program);
				if (lookTD(2) >= 0)
					printf("<2, %d> ", lookTD(2));
				TC = H;
				break;
			}
			else {
				buf[1] = c;
				c = fgetc(program);
				if (lookTD(1) >= 0)
					printf("<2, %d> ", lookTD(1));
				TC = H;
				break;
			}
			break;
		default:
			break;
		}
		clear();
	} while (TC != ER);

	if (TC == ER) {
		printf("Program cannot be parsed!, %d\n", ercode);
		switch (ercode){
		case 1:
			printf("\nError in %d line: comment isnt over", str);
			break;
		case 2:
			printf("\nError in %d line: There no such symbol in TD - '%c'", str, ersymb);
			break;
		case 3:
			printf("\nError in %d line: ! without =", str);
			break;
		case 4:
			printf("Error in %d line: : without =", str);
			break;
		}
	}
	fclose(program);
	system("pause");
}