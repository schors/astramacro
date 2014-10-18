#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "lang.h"
#include "parselib.h"
#include "parsem.h"

Token	*files, *n;
char	*outfilename;
static	char	module_name[]={"macro.c"};
static	int	fl_out=0;

/* напечатать информацию о параметрах командной строки */
static	void  printhelp(void);
/* очистить всякие выделения памяти */
static	void  cleanheap(void);

/* очистить всякие выделения памяти */
void  cleanheap(void) {
	if (outfilename != NULL) free(outfilename);
	if (files != NULL) del_token(files);
	if (includes != NULL) del_token(includes);
}

/* напечатать информацию о параметрах командной строки */
void  printhelp(void) {
	sysmsg(MSG_HELP);
}

int main (int argc, char *argv[])
 {
	int i;
	int ret;

	for (i=1; i<argc; i++ ) {
		if (argv[i][0] == '-') {
			if ( (argv[i][1] == 'o') && (argv[i][2] == '\0') ) {
				if (fl_out) {
					sysmsg(MSG_DUPLICATE_SWITCH,"-o");
					cleanheap();
					return 1;
				} else fl_out++;
				i++;
				if (i>=argc) {
					sysmsg(MSG_ARGUMENT_MISSING,"-o");
					cleanheap();
					return 1;
				}
				if (argv[i][0] == '-') {
					sysmsg(MSG_INVALID_ARGUMENT,"-o");
					cleanheap();
					return 1;
				}
				if ((outfilename = (char *) malloc (strlen(argv[i])) ) == NULL ) {
					syserr(ERR_MEMORY,module_name);
					cleanheap();
					return 1;
				} else {
					strcpy(outfilename,argv[i]);
				}
			} else if ( (argv[i][1] == 'I') && (argv[i][2] == '\0') ) {
				i++;
				if (i>=argc) {
					sysmsg(MSG_ARGUMENT_MISSING,"-I");
					cleanheap();
					return 1;
				}
				if (argv[i][0] == '-') {
					sysmsg(MSG_INVALID_ARGUMENT,"-I");
					cleanheap();
					return 1;
				}
				if ( includes==NULL ) includes = new_token(includes,argv[i]);
				else n = new_token(includes,argv[i]);
			} else if ( ((argv[i][1] == '?') || (argv[i][1] == 'h')) && (argv[i][2] == '\0') ) {
				printhelp();
				return 1;
			} else if ( (argv[i][1] == 'L') && (argv[i][2] != '\0') && (argv[i][3] == '\0') ) {
				if ( argv[i][2] == 'r' ) lng_dialog = LANGUAGE_RU; else
					if ( argv[i][2] == 'e' ) lng_dialog = LANGUAGE_EN; else {
						sysmsg(MSG_INVALID_ARGUMENT,"-L");
					}
			} else if ( (argv[i][1] == 'l') && (argv[i][2] != '\0') && (argv[i][3] == '\0') ) {
				if ( argv[i][2] == 'r' ) lng_dist = LANGUAGE_RU; else
					if ( argv[i][2] == 'e' ) lng_dist = LANGUAGE_EN; else {
						sysmsg(MSG_INVALID_ARGUMENT,"-l");
					}
			} else if ( (argv[i][1] == 'P') && (argv[i][2] != '\0') && (argv[i][3] == '\0') ) {
				if ( argv[i][2] == '1' ) cp_dialog = RECODE_CP866_KOI8R; else
					if ( argv[i][2] == '2' ) cp_dialog = RECODE_CP866_CP1251; else
						if ( argv[i][2] == '3' ) cp_dialog = RECODE_CP866_CP866; else {
							sysmsg(MSG_INVALID_ARGUMENT,"-P");
					}
			} else if ( (argv[i][1] == 'p') && (argv[i][2] != '\0') && (argv[i][3] == '\0') ) {
				if ( argv[i][2] == '1' ) cp_dist = RECODE_CP866_KOI8R; else
					if ( argv[i][2] == '2' ) cp_dist = RECODE_CP866_CP1251; else
						if ( argv[i][2] == '3' ) cp_dist = RECODE_CP866_CP866; else {
							sysmsg(MSG_INVALID_ARGUMENT,"-p");
						}
			} else if ( (argv[i][1] == 'S') && (argv[i][2] != '\0') && (argv[i][3] == '\0') ) {
				if ( argv[i][2] == '1' ) cp_source = RECODE_CP866_KOI8R; else
					if ( argv[i][2] == '2' ) cp_source = RECODE_CP866_CP1251; else
						if ( argv[i][2] == '3' ) cp_source = RECODE_CP866_CP866; else {
							sysmsg(MSG_INVALID_ARGUMENT,"-S");
						}
			} else {
				sysmsg(MSG_INVALID_SWITCH,argv[i]);
				cleanheap();
				return 1;
			}
		} else {
			if ( files==NULL ) files = new_token(files,argv[i]);
			else n = new_token(files,argv[i]);
		}
	}

 if (outfilename == NULL) {
	outfile = stdout;
 } else {
	if ( (outfile = fopen (outfilename,"wb")) == NULL ) {
		sysmsg(MSG_CANT_OPEN,outfilename);
		cleanheap();
		return 1;
	}
 }

 if ( files == NULL ) {
		if ( (ret = parse_file(NULL)) == 1 )
			fprintf(stderr,"Errors detected\n");
		else if (ret == -1) {
			fprintf(stderr,"Crash!!!\n");
			if (amacro != NULL) del_macro(amacro);
			if (outfilename != NULL) fclose (outfile);
			cleanheap();
			return 1;
		} else {
			fprintf(stderr,"Ok\n");
		}
 } else {
	n = files;
	while (n != NULL) {
		fprintf(stderr,"%s \n",n->line);
		if ( (ret = parse_file(n->line)) == 1 )
			fprintf(stderr,"Errors detected\n");
		else if (ret == 2) {
			sysmsg(MSG_NOT_FOUND,n->line);
		} else if (ret == -1) {
			fprintf(stderr,"Crash!!!\n");
			if (amacro != NULL) del_macro(amacro);
			if (outfilename != NULL) fclose (outfile);
			cleanheap();
			return 1;
		} else {
			fprintf(stderr,"Ok\n");
		}
		n = n->next;
	}
 }

 print_msg(MSG_REPORT,progerrornum,line_count);

 if (amacro != NULL) del_macro(amacro);
 if (outfilename != NULL) fclose (outfile);
 cleanheap();

 return 0;
 }
