#include <stdio.h>
#include "parselib.h"

#ifndef MACRO_H
#define MACRO_H

extern	Macro	*amacro;			/* указатель на список макросов */
extern	Token	*includes;                      /* Список каталогов, где могут находиться включения */

extern	int	line_count;			/* номер строки (общий) */

/* подставить фактические параметры */
extern	char	*releasestring(Token *, Token *, char *, char *);

extern	int	parse_macro(char *, FILE *, Token *, Token *, Token *, int *);
extern	int	parse_file(char *fn);

extern	FILE	*file_exists(char *);
extern	char	*read_line(FILE *, Token *, Token *, Token **, int *, int *, char *);

#endif
