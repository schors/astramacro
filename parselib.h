#ifndef PARSELIB_H
#define PARSELIB_H

#define PROGSTRLEN 1024		/* длина строки */

struct sToken {
 struct sToken *next;
 char *line;
 };
typedef struct sToken Token;

struct sMacro {
 struct sMacro *next;
 char *name;
 struct sToken *body, *param;
 };
typedef struct sMacro Macro;

extern	int	is_symbol(int);			/* символ ли? */
extern	int	is_digit(int);			/* цифра ли? */
extern	int	is_special(int);		/* спецсимвол ли? */
extern	int	upcasec(int);	   		/* перевести символ в верхний регистр */
extern	int	lowcasec(int);			/* перевести символ в нижний регистр */

extern	Token	*new_token(Token *, char *);		/* новая строка в список */
extern	void	del_token(Token *);		/* удалить список */
extern	Macro	*new_macro(Macro *);		/* новый макрос в список */
extern	void	del_macro(Macro *);		/* удалить список */
extern	int	str_cmp(char *, char *);	/* сравнить строки без учета регистра */
extern	char	*getline(FILE *, char *);	/* считать строку из файла fb */

	/* разобрать строку по словам, возвращает указатели на вхождение
	   слов или NULL */
extern	void	parsestring(char *, char **, char **, char **, char **, char **);

extern	FILE	*outfile;			/* результирующий файл */
extern	int	*curstrn;			/* номер строки текущем разбираемом файле */

#endif
