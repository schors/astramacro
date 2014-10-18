#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "err.h"
#include "lang.h"
#include "parselib.h"

/* сообщения о системных ошибках */
void	syserr(int,...);
/* системные сообщения */
void	sysmsg(int,...);
/* сообщения об ошибках пpи разборе */
void    progerr(int,...);
/* вывод строки программы */
void	print_line(char *);
/* вывод закомментированной строки программы */
void	print_rem_line(char *);
/* вывод строки сообщения транслятора */
void	print_msg(int,...);
/* общее количество ошибок */
int	progerrornum=0;

char	*sysmsgstr[][2]={
{"Usage:  mastra  files... [-<switches>]\n\
files...       - source files (default - stdin)\n\
-L<r | e>      - dialog language,\n\
-l<r | e>      - destination language, where\n\
		 r - russian\n\
 		 e - english\n\
-S<1 | 2 | 3>  - source charset,\n\
-P<1 | 2 | 3>  - dialog charset,\n\
-p<1 | 2 | 3>  - destination charset, where\n\
		 1 - koi8-r;\n\
		 2 - cp-1251;\n\
		 3 - cp-866 (default).\n\
-o <filename>  - destination file name (default - stdout)\n\
-I <pathname>  - path for includes (may be more than one switch,\n\
		 must be terminated by \"\\\")\n",
"Применять: mastra  files... [-<switches>]\n\
files...       - входные файлы (по умолчанию stdin)\n\
-L<r | e>      - язык программы,\n\
-l<r | e>      - язык результата, где\n\
		 r - русский;\n\
		 e - английский.\n\
-S<1 | 2 | 3>  - кодировка исходных файлов,\n\
-P<1 | 2 | 3>  - кодировка программы,\n\
-p<1 | 2 | 3>  - кодировка результата, где\n\
		 1 - koi8-r;\n\
		 2 - cp-1251;\n\
		 3 - cp-866 (по умолчанию).\n\
-o <filename>  - имя выходного файла (по умолчанию stdout)\n\
-I <pathname>  - путь к вставляемым файлам (может быть несколько таких ключей,\n\
		 должны содержать завершающий \"\\\")\n"},
{"Can't open file: %s\n","Не могу открыть файл: %s\n"},
{"File not found: %s\n","Файл не найден: %s\n"},
{"Duplicate switch %s\n","Повторное использование ключа %s\n"},
{"Argument missing, switch %s\n","Нет аргумента ключа %s\n"},
{"Invalid switch argument %s\n","Неверный параметр ключа %s\n"},
{"Invalid switch %s\n","Неверный ключ %s\n"}
};

char	*msgstr[][2]={
{"**>*   %s\n**[!]* Error in file %s at line %d: ","**>*   %s\n**[!]* Ошибка в файле %s в строке %d: "},
{"**[+]* Macro: begin of file: %s\n","**[+]* Macro: начало файла: %s\n"},
{"**[-]* Macro: end of file: %s\n","**[-]* Macro: конец файла: %s\n"},
{"**[#]* Errors: %d\n**[#]* Lines:  %d\n","**[#]* Ошибок: %d\n**[#]* Строк:  %d\n"},
};

char	*syserrstr[][2]={
{"System error in module: %s, insuccifient memory.\n","Системная ошибка в модуле: %s, ошибка памяти.\n"},
{"**>*%s\n Line to long.\n","**>*%s\n Строка слишком длинная.\n"},
};

char	*progerrstr[][2]={
{"operator missing.\n","нет поля операции.\n"},
{"macro name missing.\n","нет имени макроопределения.\n"},
{"file name missing.\n","нет имени файла.\n"},
{"MACRO missing.\n","пропущена операция MACRO.\n"},
{"invalid macro name %s.\n","неправильное имя макроопределения %s.\n"},
{"duplicate identificator %s.\n","повторное определение имени %s\n"},
{"Invalid arguments.\n","Неверные аргументы.\n"},
{"Invalid arguments count.\n","Неверное количество аргументов.\n"},
{"no file or directory: %s.\n","нет файла или директории: %s.\n"},
};

static	char	module_name[]={"err.c"};	/* имя локального модуля */

/*
системные сообщения
code -- код ошибки, по нему будет выбираться значение из массива sysmsg
...  -- параметры, определяемые спецификаторами формата из выбранной строки
Вывод сообщения производится в stderr
*/
void	sysmsg(int code,...) {
	char *s,*so,*p,*sval;
	int ival;
	FILE *fb;
	va_list ap;
	fb = stderr;
	if (fb != NULL) {
		if ((so = (char *) malloc(strlen(sysmsgstr[code][lng_dialog])+1) ) == NULL ) {
			syserr(ERR_MEMORY,module_name);
			return;
		}
		s=str_cpy(sysmsgstr[code][lng_dialog],so,cp_dialog);
		if (s != NULL) {
			va_start(ap,code);
			for (p=s; *p != '\0'; p++) {
				if (*p != '%') {
					fputc(*p,fb);
					continue;
				}
				switch (*++p) {
				case 'd':
					ival = va_arg(ap,int);
					fprintf(fb,"%d",ival);
					break;
				case 's':
                                        sval=va_arg(ap,char *);
					if (sval==NULL) break;
						else fputc(*sval,fb);
					for (++sval; *sval != '\0'; sval++)
						fputc(*sval,fb);
					break;
				default:
					fputc(*p,fb);
					break;
				}
			}
			va_end(ap);
		}
	}
	free(so);
	return;
}

/*
Печать сообщения системной ошибке
code -- код ошибки, по нему будет выбираться значение из массива syserrstr
...  -- параметры, определяемые спецификаторами формата из выбранной строки
Вывод сообщения производится в выходной файл и stderr
*/
void	syserr(int code,...) {
	char *s,*so,*p,*sval;
	int ival;
	FILE *fb;
	int i,lng,cp;
	va_list ap;
	fb = stderr;
	lng = lng_dialog;
	cp = cp_dialog;
	for (i=0; i<2; i++) {
		if (fb != NULL) {
			if ((so = (char *) malloc(strlen(syserrstr[code][lng])+1) ) == NULL ) {
				syserr(ERR_MEMORY,module_name);
				return;
			}
			s=str_cpy(syserrstr[code][lng],so,cp);
			if (s != NULL) {
				va_start(ap,code);
				for (p=s; *p != '\0'; p++) {
					if (*p != '%') {
						fputc(*p,fb);
						continue;
					}
					switch (*++p) {
					case 'd':
						ival = va_arg(ap,int);
						fprintf(fb,"%d",ival);
						break;
					case 's':
                                                sval=va_arg(ap,char *);
						if (sval==NULL) break;
							else fputc(*sval,fb);
						for (++sval; *sval != '\0'; sval++)
							fputc(*sval,fb);
						break;
					default:
						fputc(*p,fb);
						break;
					}
				}
				va_end(ap);
			}
		}
		free(so);
		fb = outfile;
		lng = lng_dist;
		cp = cp_dist;
	}
	return;
}

/*
Печать сообщения о ошибке в тексте программы
code -- код ошибки, по нему будет выбираться значение из массива progerrstr
...  -- параметры, определяемые спецификаторами формата из выбранной строки
Вывод сообщения производится в выходной файл и stderr
*/
void	progerr(int code,...) {
	char *s,*so,*p,*sval;
	int ival;
	FILE *fb;
	int i,lng,cp;
	va_list ap;
	fb = stderr;
	lng = lng_dialog;
	cp = cp_dialog;
	for (i=0; i<2; i++) {
		if (fb != NULL) {
			if ((so = (char *) malloc(strlen(progerrstr[code][lng])+strlen(msgstr[0][lng])+1) ) == NULL ) {
				syserr(ERR_MEMORY,module_name);
				return;
			}
			so=strcpy(so,msgstr[MSG_ERROR][lng]);
			so=strcat(so,progerrstr[code][lng]);
			s=str_cpy(so,so,cp);
			if (s != NULL) {
				va_start(ap,code);
				for (p=s; *p != '\0'; p++) {
					if (*p != '%') {
						fputc(*p,fb);
						continue;
					}
					switch (*++p) {
					case 'd':
						ival = va_arg(ap,int);
						fprintf(fb,"%d",ival);
						break;
					case 's':
                                                sval=va_arg(ap,char *);
						if (sval==NULL) break;
							else fputc(*sval,fb);
						for (++sval; *sval != '\0'; sval++)
							fputc(*sval,fb);
						break;
					default:
						fputc(*p,fb);
						break;
					}
				}
				va_end(ap);
			}
		}
		fb = outfile;
		lng = lng_dist;
		cp = cp_dist;
		free(so);
	}
	progerrornum++;
	return;
}

void	print_line(char *s) {
	char *s1,*so;
	if ((so = (char *) malloc (PROGSTRLEN) ) == NULL ) {
		syserr(ERR_MEMORY,module_name);
		return;
	}
	if (outfile != NULL) {
		s1 = str_cpy(s,so,cp_dist);
		if (s1 != NULL) {fprintf(outfile,"%s",s1);}
		fprintf(outfile,"\n");
	}
	free(so);
}

void	print_rem_line(char *s) {
	char *s1,*so;
	if ((so = (char *) malloc (PROGSTRLEN) ) == NULL ) {
		syserr(ERR_MEMORY,module_name);
		return;
	}
	if (outfile != NULL) {
		s1 = str_cpy(s,so,cp_dist);
		if (s1 != NULL) {fprintf(outfile,"* %s",s1);}
		fprintf(outfile,"\n");
	}
	free(so);
}

/*
Печать сообщения в тексте программы
code -- код ошибки, по нему будет выбираться значение из массива msgstr
...  -- параметры, определяемые спецификаторами формата из выбранной строки
Вывод сообщения производится в выходной файл и stderr
*/
void	print_msg(int code,...) {
	char *s,*so,*p,*sval;
	int ival;
	FILE *fb;
	int i,lng,cp;
	va_list ap;
	fb = stderr;
	lng = lng_dialog;
	cp = cp_dialog;
	for (i=0; i<2; i++) {
		if (fb != NULL) {
			if ((so = (char *) malloc(strlen(msgstr[code][lng])+1) ) == NULL ) {
				syserr(ERR_MEMORY,module_name);
				return;
			}
			s=str_cpy(msgstr[code][lng],so,cp);
			if (s != NULL) {
				va_start(ap,code);
				for (p=s; *p != '\0'; p++) {
					if (*p != '%') {
						fputc(*p,fb);
						continue;
					}
					switch (*++p) {
					case 'd':
						ival = va_arg(ap,int);
						fprintf(fb,"%d",ival);
						break;
					case 's':
                                                sval=va_arg(ap,char *);
						if (sval==NULL) break;
							else fputc(*sval,fb);
						for (++sval; *sval != '\0'; sval++)
							fputc(*sval,fb);
						break;
					default:
						fputc(*p,fb);
						break;
					}
				}
				va_end(ap);
			}
		}
		fb = outfile;
		lng = lng_dist;
		cp = cp_dist;
		free(so);
	}
	return;
}

