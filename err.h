#include <stdio.h>
#ifndef ERR_H
#define ERR_H

/* Системные ошибки, могущие возникать при работе программы */

/* Ошибки программы */
enum {
MSG_HELP,		
MSG_CANT_OPEN,		
MSG_NOT_FOUND,		
MSG_DUPLICATE_SWITCH,
MSG_ARGUMENT_MISSING,
MSG_INVALID_ARGUMENT,
MSG_INVALID_SWITCH,
};

/* Различные сообщения */
enum {
MSG_ERROR,
MSG_BEGIN_FILE,
MSG_END_FILE,
MSG_REPORT,
};

/* Ошибки системы */
enum {
ERR_MEMORY,		/* Ошибка памяти */
ERR_LINE_TO_LONG,       /* Ошибка длинная строка */
};

/* Ошмбки при разборе */
enum {
SINTAX_NOOPERATOR,	/* нет поля операции */
SINTAX_NOMACRONAME,	/* нет имени макроопределения */
SINTAX_NOFILENAME,	/* нет имени файла */
SINTAX_MENDLESS,	/* mend без macro */
SINTAX_MACRONAME,	/* неправильное имя макроопределения */
SINTAX_DUPLICATENAME,	/* повторное определение имени */
SINTAX_ARGUMENTS,	/* неверные аргументы (отсутствует &) */
SINTAX_ARGUMENTS_COUNT,	/* неверные аргументы (количество) */
COMMON_NOFILE,          /* Нет файла или дериктории в include */
};

/* системные сообщения */
extern	void	sysmsg(int,...);
/* сообщения о системных ошибок */
extern	void	syserr(int,...);
/* сообщения об ошибках в программе */
extern	void    progerr(int,...);
extern	void	print_msg(int,...);

extern	void	print_line(char *);
extern	void	print_rem_line(char *);

extern	char	*sysmsgstr[][2];
extern	char	*msgstr[][2];

extern	char	*progerrstr[][2];
extern	char	*syserrstr[][2];

/* количество ошибок */
extern	int	progerrornum;

#endif
