#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "parsem.h"
#include "parselib.h"

static char	*declarations [] = {"INCLUDE","MACRO","MEND"};

Macro	*amacro;					/* указатель на список макросов */
Token	*includes;                      		/* Список каталогов, где могут находиться включения */
int	line_count=0;					/* номер строки (общий) */
int	macro_mode=0;					/* 0 - просто текст, 1 - режим подстановки */

static	char	module_name[]={"parsem.c"};		/* имя локального модуля */
char	spacestr[]={" "};				/* пробельная строка */

Token	*parseparam(char *, char *, char*);		/* разобрать строку параметров */
int	chkparam(Token *);				/* проверить строку формальных параметров на '&' */
int	cntparam(Token *);				/* сосчитать количество параметров */
Macro	*findmacro(char *);				/* есть ли такое макроопределение? */
char	*read_line(FILE *, Token *, Token *, Token **, int *, int *, char *);
int	parse_macro(char *, FILE *, Token *, Token *, Token *, int *);
int	parse_file(char *);
FILE	*file_exists(char *);

/* подставить фактические параметры
1-ый аргумент -- формальные параметры
2-ой аргумент -- фактические параметры
3-ий аргумент -- строка, которую нужно разобрать
*/
char	*releasestring(Token *, Token *, char *, char *);

/* сосчитать количество параметров */
int	cntparam(Token *p) {
	int i;
	for (i=0;p != NULL;i++) {
		p = p->next;
	}
	return i;
 }

/* проверить строку формальных параметров на '&' */
int	chkparam(Token *p) {
	if (p != NULL) p = p->next; else return 1;
	while (p != NULL) {
		if ( p->line[0] != '&') {
			return 1;
		}
		p = p->next;
	}
	return 0;
 }

/* подставить фактические параметры */
char	*releasestring(Token *formp, Token *factp, char *s, char *so) {
	int	i,ii,io;
	char	c,*n;
	Token	*formp1,*factp1;

	io = 0;
	while ( (n = (char *)strchr(s,'&')) !=NULL ) {
		for (i=0; (s+i) != n; i++) {
			so[io++] = s[i];
		}
		s = n;
		for (i = 0; ( s[i]=='&' || !is_special((int)s[i]) && (s[i] != '\0')); i++) {};
		c = s[i];
		s[i] = '\0';
		ii = i;
		formp1 = formp;
		factp1 = factp;
		while (formp1 != NULL) {
		      if (!str_cmp(formp1->line,s)) {
		      for (i=0; factp1->line[i] != '\0'; i++) {
			so[io++] = factp1->line[i];
		      }
		      break;
		      }
		      formp1 = formp1 -> next;
		      factp1 = factp1 -> next;
		}
		s[ii] = c;
		s += ii;
	}
	for (i=0; s[i] != '\0'; i++ ) {
		so[io++] = s[i];
	};
	so[io] = '\0';
	return so;
 }

Macro	*findmacro(char *s) {			/* есть ли такое макроопределение? */
	Macro *p;

	p = amacro;
	while ( p != NULL) {
		if ( !str_cmp(p->name,s) ) {
			return p;
		}
		p = p -> next;
	}
	return p;
 }

Token	*parseparam(char *s1, char *s2, char*s3)		/* разобрать строку параметров */
 /*
 s1 - слово метки
 s2 - слово типа операции
 s3 - слово операнда
 */
 {
	Token	*n;
	char	*s;

	n = NULL;
	if (s1 != NULL) {
		if ((n = new_token(n,s1)) == NULL) return NULL;
	} else {
		if ((n = new_token(n,spacestr)) == NULL) return NULL;
	}
	if (s2 != NULL) {
		if (new_token(n,s2) == NULL) return NULL;
	}
	if (s3 != NULL) {
		while ((s = (char *)strchr(s3,',')) != NULL) {
			s[0] = '\0';
			if (new_token(n,s3) == NULL) return NULL;
			s[0] = ',';
			s3 = s+1;
			if (s3[0] == '\0') break;
		}
		if (s3[0] != '\0')
			if (new_token(n,s3) == NULL) return NULL;
	}
	return n;
 }

char *read_line(FILE *fb, Token *formp, Token *factp, Token **m, int *end_flag, int *fstrno, char *s) {
	if (macro_mode) {
	  s = releasestring(formp, factp, (*m)->line, s);
	  (*m) = (*m) -> next;
	  if ((*m) == NULL) (*end_flag)=1;
	  return s;
	} else {
	  s = getline(fb, s);
	  (*fstrno)++;
	  if (feof(fb)) (*end_flag)=1;
	  return s;
	}
}

/*
функция разбора файла
fn - имя файла
возвращаем:
1  -- файл разобран неуспешно
0  -- файл разобран успешно
-1 -- возникли ошибки системы
*/
int	parse_file(char *fn) {
	FILE	*fb;			/* исходный файл */
	int	fstrno=0;		/* номер строки в файле */
	int	err=0;			/* возникшие ошибки */
	/* Открыть входной файл */
	if (fn == NULL) {
		fb = stdin;
	} else {
		if ( (fb = file_exists(fn)) == NULL ) {
			err = 2;
			return err;
		}
	}
	print_msg(MSG_BEGIN_FILE,fn);
	err = parse_macro(fn,fb,NULL,NULL,NULL,&fstrno);
	print_msg(MSG_END_FILE,fn);
	fclose(fb);
	return err;
}

/*
функция разбора макроса
fn - имя файла
fb - указатель на файл
s  - строка
fstrno - указатель на счетчик строк в файле
возвращаем:
1  -- файл разобран неуспешно
0  -- файл разобран успешно
-1 -- возникли ошибки системы
*/
int	parse_macro(char *fn, FILE *fb, Token *formp, Token *factp, Token *body, int *fstrno) {
	Token	*p,*n;
	Macro	*m;
	char *s,*so;
	char *label,*oper,*flags,*term,*rem;
	int	end_flag=0,err=0,macro_level;

	if ((so = (char *) malloc (PROGSTRLEN) ) == NULL ) {
		syserr(ERR_MEMORY,module_name);
		return -1;
	}
	if ((s = (char *) malloc (PROGSTRLEN) ) == NULL ) {
		syserr(ERR_MEMORY,module_name);
		free(so);
		return -1;
	}

	if (macro_mode) p = body;

	while ( !end_flag ) {
		so = read_line(fb,formp,factp,&p,&end_flag,fstrno,so); line_count++;
		s = strcpy (s,so);
		if (s[0] == '*') {
			print_line(so);
			continue;
		} else {
			parsestring(s,&label,&oper,&flags,&term,&rem);
		}
		if (oper == NULL) if (label == NULL) {
			print_line(so);
			continue;
		} else {
			progerr(SINTAX_NOOPERATOR,so,fn,*fstrno);
			err=1;
			continue;
		}
		if (!str_cmp(oper,declarations[0])) {
			/* Разбор включенного файла */
			if (term != NULL) {
				if ( (err = parse_file(term)) == -1) {
					goto exitl;
				} else if (err == 2) {
					progerr(COMMON_NOFILE,so,fn,*fstrno,term);
				}
			} else	{
				progerr(SINTAX_NOFILENAME,so,fn,*fstrno);
				err=1;
			}
		} else
		if (!str_cmp(oper,declarations[1])) {
			/* Разбор определения макроса */
			print_rem_line(so);
			so = read_line(fb,formp,factp,&p,&end_flag,fstrno,so); line_count++;
			s = strcpy (s,so);
			parsestring(s,&label,&oper,&flags,&term,&rem);
			n = parseparam(label,flags,term);
			if ( (!str_cmp(oper,declarations[0]))
			     || (!str_cmp(oper,declarations[1]))
			     || (!str_cmp(oper,declarations[2])) ) {
				progerr(SINTAX_MACRONAME,so,fn,*fstrno,oper);
				err=1;
			} else if ( findmacro(oper) !=NULL ) {
				progerr(SINTAX_DUPLICATENAME,so,fn,*fstrno,oper);
				err=1;
			} else 	if ( chkparam(n) ) {
				progerr(SINTAX_ARGUMENTS,so,fn,*fstrno);
				err=1;
			} else if (oper != NULL) {
				print_rem_line(so);
				if ((m = new_macro(amacro)) == NULL) {
						err= -1;
						goto exitl;
					} else {
						if (amacro == NULL) amacro = m;
					}
				if ((m->name = (char *) malloc (strlen(oper)) ) == NULL ) {
					syserr(ERR_MEMORY,module_name);
					err= -1;
					goto exitl;
				}
				m->name=(char *)strcpy((char *)m->name,(char *)oper);
				m->param = n;
				macro_level = 0;
				while ( !end_flag ) {
					so = read_line(fb,formp,factp,&p,&end_flag,fstrno,so); line_count++;
					s = (char *)strcpy (s,so);
					parsestring(s,&label,&oper,&flags,&term,&rem);
					if (!str_cmp(oper,declarations[1])) {
						macro_level++;
					}
					print_rem_line(so);
					if (!str_cmp(oper,declarations[2])) {
						if (!macro_level) break; else {
							macro_level--;
						}
					}
					if ((n = new_token(m->body,so)) == NULL) {
						err= -1;
						goto exitl;
					}  else if (m->body == NULL) m->body = n;
				}
			}  else {
				progerr(SINTAX_NOMACRONAME,so,fn,*fstrno);
				err=1;
		       }
		} else
		if (!str_cmp(oper,declarations[2])) {
			progerr(SINTAX_MENDLESS,so,fn,*fstrno);
			err=1;
		} else
		if ( (m = findmacro(oper)) !=NULL ) {
			print_rem_line(so);
			n = parseparam(label,flags,term);
			if ( cntparam(n) != cntparam(m->param) ) {
				progerr(SINTAX_ARGUMENTS_COUNT,so,fn,*fstrno);
				err=1;
				free(n);
				continue;
			}
			macro_mode++;
			if ((err = parse_macro(fn,fb,m->param,n,m->body,fstrno))== -1) {
				free(n);
				goto exitl;
				}
			macro_mode--;
			free(n);
		} else
		print_line(so);
	}
exitl:  free(s);
	free(so);
	return err;
}

FILE	*file_exists(char *fn) {
	Token *n;
	FILE *fb;
	char *s;

	if ( (fb = fopen(fn,"rb")) != NULL) {
		return fb;
	}

	n = includes;

	while (n != NULL) {
		if ((s = (char *) malloc (strlen((n->line))+strlen(fn)+1) ) == NULL ) {
			syserr(ERR_MEMORY,module_name);
			return NULL;
		}
		s = (char *)strcpy (s,(n->line));
		s = (char *)strcat(s,fn);
		if ( (fb = fopen(s,"rb")) != NULL) {
			free(s);
			return fb;
		}
		free(s);
		n = n -> next;
	}
	return NULL;
}