#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "err.h"
#include "lang.h"
#include "parselib.h"

int	is_symbol(int);			/* символ ли? */
int	is_digit(int);			/* цифра ли? */
int	is_special(int);		/* спецсимвол ли? */

Token	*new_token(Token *, char *);	/* новая строка в список */
void	del_token(Token *);		/* удалить список */
Macro	*new_macro(Macro *);		/* новый макрос в список */
void	del_macro(Macro *);		/* удалить список */
int	upcasec(int);	   		/* перевести символ в верхний регистр */
int	lowcasec(int);			/* перевести символ в нижний регистр */
int	str_cmp(char *, char *);	/* сравнить строки без учета регистра */

	/* разобрать строку по словам, возвращает указатели на вхождение
	   слов или NULL */
void	parsestring(char *, char **, char **, char **, char **, char **);

char	*getline(FILE *, char *);	/* считать строку из файла fb */

static	char	module_name[]={"parselib.c"};

FILE	*outfile;			/* результирующий файл */
int	*curstrn;			/* номер строки текущем разбираемом файле */

/*
Функция добавляет новый элемент в список строк, заданный параметром 'p'
строка s становиться полем ->line вновьсозданной структуры Token
*/
Token	*new_token(Token *p, char *s)
 {
	Token	*n;

	n = p;
	while ( n != NULL ) { p = n; n = p->next; }
	if ((n = (Token *) malloc (sizeof(Token)) ) == NULL ) {
	   syserr(ERR_MEMORY,module_name);
	   return NULL;
	}
	if (s != NULL) {
		if ((n->line = (char *) malloc (strlen(s)+1) ) == NULL ) {
			syserr(ERR_MEMORY,module_name);
			return NULL;
		}
		n->line = strcpy(n->line,s);
	} else {
		n->line = NULL;
	}
	if (p != NULL) p->next = n;
	n->next = NULL;
	return n;
 }

/*
Функция удаляет список строк
*/
void	del_token(Token *p)
 {
	Token *n;
	n = p;
	while ( n != NULL ) {
		p = n->next;
		if ( n->line != NULL ) free (n->line);
		free (n);
		n = p;
	}
 }

/*
Функция добавляет новый элемент в список макросов, заданный параметром 'p'
*/
Macro	*new_macro(Macro *p)
 {
	Macro	*n;
	n = p;
	while ( n != NULL ) { p = n; n = p->next; }
	if ((n = (Macro *) malloc (sizeof(Macro)) ) == NULL ) {
	   syserr(ERR_MEMORY,module_name);
	   return NULL;
	}
	if (p != NULL) p->next = n;
	n->param = n->body = NULL;
	n->name = NULL;
	n->next = NULL;
	return n;
 }

/*
Функция удаляет список макросов
*/
void	del_macro(Macro *p)
 {
	Macro *n;
	n = p;
	while ( n != NULL ) {
		p = n->next;
		del_token(n->body);
		del_token(n->param);
		if ( n->name != NULL ) free(n->name);
		free (n);
		n = p;
	}
 }

/*
Перевести символ в нижний регистр
*/
int	lowcasec(int c)
 {
	if ( c>=65 && c<=90 ) {
		c += 32;
		return c;
	}
	if ( c>=128 && c<=143 ) {
		c += 32;
	} else if ( c>=144 && c<=159 ) {
		c += 80;
	}
	return c;
 };

/*
Перевести символ в верхний регистр
*/
int	upcasec(int c)
 {
	if ( c>=97 && c<=122 ) {
		c -= 32;
		return c;
	}
	if ( c>=160 && c<=175 ) {
		c -= 32;
	} else if ( c>=224 && c<=239 ) {
		c -= 80;
	}
	return c;
 };

/*
Предполагается, что строка заканчивается '\0',
*/
void	parsestring(char *s, char **label, char **oper, char **flags, char **term, char **rem)
 {
	int i = 0, j;
	char	*st;

	/* Если NULL строка, то все возвращаемые значения NULL и выходим */
	if ( s == NULL ) { *label = *oper = *flags = *term = *rem = NULL; return; }

	/* Если нулевая строка, то все возвращаемые значения NULL и выходим */
	if ( s[i] == '\0' ) { *label = *oper = *flags = *term = *rem = NULL; return; }

	/* Если первый символ - ПРОБЕЛ или ТАБ, то метка равна NULL
	   Еcли метка есть, то запоминаем указатель на 1-ый символ,
	   доходим до первого пробела и ставим '\0'.
	   Или, если встречен конец строки, присваеваем NULL куда надо
	   и выходим */
	if ( (s[i] == ' ') || (s[i] == '\t') ) *label = NULL; /* Если нет метки, то NULL */
	 else  {
		*label = s;             				/* Если метка есть, то присваеваем */
		while ( (s[i] != '\t') && (s[i] != ' ') && (s[i] != '\0' ) ) { i++; }	/* указатель и идем в конец */
		if ( s[i] == '\0' ) { *oper = *flags = *term = *rem = NULL; return; }
		s[i++] = '\0';
	}

	/* Ищем первый непробельный символ,  указатель на него возвращаем
	   в *oper
	   Или, если встречен конец строки, присваеваем NULL куда надо
	   и выходим */
	while ( ((s[i] == '\t') || (s[i] == ' ')) && (s[i] != '\0' ) ) { i++; };	/* идем до первого непробела */
	if ( s[i] == '\0' ) { *oper = *flags = *term = *rem = NULL; return; }
		*oper = s+i;                                            /* присвоили указатель оператора */

	/* Доходим до первого пробела и ставим '\0'.
	   Или, если встречен конец строки, присваеваем NULL куда надо
	   и выходим */
	while ( (s[i] != '\t') && (s[i] != ' ') && (s[i] != '\0' ) ) { i++; };	/* идем в конец */
	if ( s[i] == '\0' ) { *flags = *term = *rem = NULL; return; }
		s[i++] = '\0';

	/* Ищем первый непробельный символ,  указатель на него запоминаем в j
	   Или, если встречен конец строки, присваеваем NULL куда надо
	   и выходим */
	while ( ((s[i] == '\t') || (s[i] == ' ')) && (s[i] != '\0' ) ) { i++; };	/* идем до первого непробела */
	if ( s[i] == '\0' ) { *flags = *term = *rem = NULL; return; }
		j = i;			/* Запоминаем начало операндов */

	/* Доходим до первого пробела и ставим '\0', в *rem запоминаем
	   адрес следующей ячейки.
	   Или, если встречен конец строки, присваеваем NULL куда надо
	   и НЕ выходим */
	while ( (s[i] != '\t') && (s[i] != ' ') && (s[i] != '\0' ) ) { i++; };	/* идем в конец */
	if (s[i] == '\0') *rem = NULL;  else { *rem = s+i+1; s[i] = '\0'; }

	/* Если есть запятая ( присутствует тип операции)
	   -- указатель j запоминаем в *flags, запятую+1 запоминаем в *oper
	*/
	if ( (st = (char *)strchr((char *)(s+j),',')) != NULL ) {
		st[0]  = '\0';
		*flags = s+j;
		*term  = st+1;
	/* В противном случае *flags = NULL, а *oper = j  */
	}  else {
		*flags = NULL;
		*term  = s+j;
	};
 };


int	is_symbol(int c) {		/* символ ли? */
	if ( (c>=65) && (c<=90) )	return 1;
	if ( (c>=97) && (c<=122) )	return 1;
	if ( (c>=128) && (c<=159) )	return 1;
	if ( (c>=160) && (c<=175) )	return 1;
	if ( (c>=224) && (c<=239) )	return 1;
	return 0;
}

int	is_digit(int c) {		/* цифра ли? */
	if ( (c>47) && (c<58) ) {
		return 1;
	} else {
		return 0;
	};
}

int	is_special(int c) {		/* спецсимвол ли? */
	if ( ((c>21) && (c<47)) || ((c>57) && (c<64)) || ((c>90) && (c<97)) || (c=='\t') ) {
		return 1;
	} else {
		return 0;
	};
}

/* сравнить строки без учета регистра,
   возвращает 0, если строки идентичны */
int	str_cmp(char *s1, char *s2) {
	int i;

	if ( (s1 == NULL) || (s2 == NULL) )  {
		return -1;
	}
	for (i=0;( (s1[i] != '\0') && (s2[i] != '\0') );i++) {
		if ( upcasec( (int)s1[i] ) != upcasec( (int)s2[i] ) ) {
			return -1;
		}
	}
	if ( upcasec( (int)s1[i] ) != upcasec( (int)s2[i] ) ) {
		return -1;
	} else return 0;
}

/*
Считать строку s из файла fb
*/
char	*getline(FILE *fb, char *s) {
	int c,i=0;

	if(s == NULL) return s;
	while ( ((c = fgetc(fb) ) != '\n') && !feof(fb) ) {
		if (c != 26) s[i++] = cp_tab[cp_source][c]; else s[i++] = ' ';
		if (i>=PROGSTRLEN) {
			s[i-1] = '\0';
			syserr(ERR_LINE_TO_LONG,s);
			s[0] = '\0';
			return s;
		}
	}
	if ( (i>0) && (s[i-1] == '\r') ) s[i-1] = ' ';
	s[i] = '\0';
	return s;
}