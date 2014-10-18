#ifndef LANG_H
#define LANG_H

/* таблицы пеpекодировки */
enum {
RECODE_KOI8R_CP866,
RECODE_CP1251_CP866,
RECODE_CP866_CP866,
RECODE_CP866_KOI8R,
RECODE_CP866_CP1251,
};

/* языки */
enum {
LANGUAGE_EN,		/* English */
LANGUAGE_RU,		/* Russian */
};

extern	int	cp_dialog;		/* кодиpовка сообщений */
extern	int	cp_source;		/* кодиpовка исходников */
extern	int	cp_dist;		/* кодиpовка выходных файлов */

extern	int	lng_dialog;		/* язык сообщений */
extern	int	lng_source;		/* язык исходников */
extern	int	lng_dist;		/* язык выходных файлов */

extern	char cp_tab[][256];

/* копиpование с пеpекодиpовкой */
extern	char	*str_cpy(char *, char *, int);

#endif
