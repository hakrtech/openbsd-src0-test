/*	$OpenBSD: misccmds.pro,v 1.1.1.1 1996/09/07 21:40:29 downsj Exp $	*/
/* misccmds.c */
int get_indent __PARMS((void));
int get_indent_lnum __PARMS((linenr_t lnum));
void set_indent __PARMS((register int size, int del_first));
int Opencmd __PARMS((int dir, int redraw, int del_spaces));
int get_leader_len __PARMS((char_u *line, char_u **flags));
int plines __PARMS((linenr_t p));
int plines_win __PARMS((WIN *wp, linenr_t p));
int plines_m __PARMS((linenr_t first, linenr_t last));
int plines_m_win __PARMS((WIN *wp, linenr_t first, linenr_t last));
void ins_char __PARMS((int c));
void ins_str __PARMS((char_u *s));
int delchar __PARMS((int fixpos));
int truncate_line __PARMS((int fixpos));
void dellines __PARMS((long nlines, int dowindow, int undo));
int gchar __PARMS((FPOS *pos));
int gchar_cursor __PARMS((void));
void pchar_cursor __PARMS((int c));
void goto_endofbuf __PARMS((FPOS *pos));
int inindent __PARMS((int extra));
char_u *skipwhite __PARMS((register char_u *p));
char_u *skipdigits __PARMS((register char_u *p));
char_u *skiptowhite __PARMS((register char_u *p));
char_u *skiptowhite_esc __PARMS((register char_u *p));
long getdigits __PARMS((char_u **pp));
char_u *skip_to_option_part __PARMS((char_u *p));
char *plural __PARMS((long n));
void set_Changed __PARMS((void));
void unset_Changed __PARMS((BUF *buf));
void change_warning __PARMS((void));
int ask_yesno __PARMS((char_u *str, int direct));
int get_number __PARMS((void));
void msgmore __PARMS((long n));
void beep_flush __PARMS((void));
void vim_beep __PARMS((void));
void init_homedir __PARMS((void));
void expand_env __PARMS((char_u *src, char_u *dst, int dstlen));
void home_replace __PARMS((BUF *buf, char_u *src, char_u *dst, int dstlen));
char_u *home_replace_save __PARMS((BUF *buf, char_u *src));
int fullpathcmp __PARMS((char_u *s1, char_u *s2));
char_u *gettail __PARMS((char_u *fname));
int ispathsep __PARMS((int c));
char_u *concat_fnames __PARMS((char_u *fname1, char_u *fname2, int sep));
char_u *FullName_save __PARMS((char_u *fname));
int islabel __PARMS((int ind_maxcomment));
int iscase __PARMS((char_u *s));
int get_c_indent __PARMS((void));
int get_lisp_indent __PARMS((void));
void preserve_exit __PARMS((void));
int vim_fexists __PARMS((char_u *fname));
void line_breakcheck __PARMS((void));
void FreeWild __PARMS((int num, char_u **file));
