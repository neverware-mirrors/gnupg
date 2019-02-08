/* util.h - Utility functions for GnuPG
 * Copyright (C) 2001, 2002, 2003, 2004, 2009 Free Software Foundation, Inc.
 *
 * This file is part of GnuPG.
 *
 * GnuPG is free software; you can redistribute and/or modify this
 * part of GnuPG under the terms of either
 *
 *   - the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at
 *     your option) any later version.
 *
 * or
 *
 *   - the GNU General Public License as published by the Free
 *     Software Foundation; either version 2 of the License, or (at
 *     your option) any later version.
 *
 * or both in parallel, as here.
 *
 * GnuPG is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License
 * and the GNU Lesser General Public License along with this program;
 * if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GNUPG_COMMON_UTIL_H
#define GNUPG_COMMON_UTIL_H

#include <gcrypt.h> /* We need this for the memory function protos. */
#include <errno.h>  /* We need errno.  */
#include <gpg-error.h> /* We need gpg_error_t and estream. */

/* These error codes are used but not defined in the required
 * libgpg-error version.  Define them here.
 * Example: (#if GPG_ERROR_VERSION_NUMBER < 0x011500 // 1.21)
 */
#if GPG_ERROR_VERSION_NUMBER < 0x011a00 /* 1.26 */
# define GPG_ERR_UNKNOWN_FLAG     309
# define GPG_ERR_INV_ORDER	  310
# define GPG_ERR_ALREADY_FETCHED  311
# define GPG_ERR_TRY_LATER        312
# define GPG_ERR_SYSTEM_BUG	  666
# define GPG_ERR_DNS_UNKNOWN	  711
# define GPG_ERR_DNS_SECTION	  712
# define GPG_ERR_DNS_ADDRESS	  713
# define GPG_ERR_DNS_NO_QUERY	  714
# define GPG_ERR_DNS_NO_ANSWER	  715
# define GPG_ERR_DNS_CLOSED	  716
# define GPG_ERR_DNS_VERIFY	  717
# define GPG_ERR_DNS_TIMEOUT	  718
#endif


/* Hash function used with libksba. */
#define HASH_FNC ((void (*)(void *, const void*,size_t))gcry_md_write)

/* Get all the stuff from jnlib. */
#include "../common/logging.h"
#include "../common/argparse.h"
#include "../common/stringhelp.h"
#include "../common/mischelp.h"
#include "../common/strlist.h"
#include "../common/dotlock.h"
#include "../common/utf8conv.h"
#include "../common/dynload.h"
#include "../common/fwddecl.h"
#include "../common/utilproto.h"

#include "gettime.h"

/* Redefine asprintf by our estream version which uses our own memory
   allocator..  */
#define asprintf gpgrt_asprintf
#define vasprintf gpgrt_vasprintf

/* Due to a bug in mingw32's snprintf related to the 'l' modifier and
   for increased portability we use our snprintf on all systems. */
#undef snprintf
#define snprintf gpgrt_snprintf


/* Replacements for macros not available with libgpg-error < 1.20.  */

/* We need this type even if we are not using libreadline and or we
   did not include libreadline in the current file. */
#ifndef GNUPG_LIBREADLINE_H_INCLUDED
typedef char **rl_completion_func_t (const char *, int, int);
#endif /*!GNUPG_LIBREADLINE_H_INCLUDED*/


/* Handy malloc macros - please use only them. */
#define xtrymalloc(a)    gcry_malloc ((a))
#define xtrymalloc_secure(a)  gcry_malloc_secure ((a))
#define xtrycalloc(a,b)  gcry_calloc ((a),(b))
#define xtrycalloc_secure(a,b)  gcry_calloc_secure ((a),(b))
#define xtryrealloc(a,b) gcry_realloc ((a),(b))
#define xtrystrdup(a)    gcry_strdup ((a))
#define xfree(a)         gcry_free ((a))
#define xfree_fnc        gcry_free

#define xmalloc(a)       gcry_xmalloc ((a))
#define xmalloc_secure(a)  gcry_xmalloc_secure ((a))
#define xcalloc(a,b)     gcry_xcalloc ((a),(b))
#define xcalloc_secure(a,b) gcry_xcalloc_secure ((a),(b))
#define xrealloc(a,b)    gcry_xrealloc ((a),(b))
#define xstrdup(a)       gcry_xstrdup ((a))

/* For compatibility with gpg 1.4 we also define these: */
#define xmalloc_clear(a) gcry_xcalloc (1, (a))
#define xmalloc_secure_clear(a) gcry_xcalloc_secure (1, (a))

/* The default error source of the application.  This is different
   from GPG_ERR_SOURCE_DEFAULT in that it does not depend on the
   source file and thus is usable in code shared by applications.
   Defined by init.c.  */
extern gpg_err_source_t default_errsource;

/* Convenience function to return a gpg-error code for memory
   allocation failures.  This function makes sure that an error will
   be returned even if accidentally ERRNO is not set.  */
static inline gpg_error_t
out_of_core (void)
{
  return gpg_error_from_syserror ();
}


/*-- yesno.c --*/
int answer_is_yes (const char *s);
int answer_is_yes_no_default (const char *s, int def_answer);
int answer_is_yes_no_quit (const char *s);
int answer_is_okay_cancel (const char *s, int def_answer);

/*-- xreadline.c --*/
ssize_t read_line (FILE *fp,
                   char **addr_of_buffer, size_t *length_of_buffer,
                   size_t *max_length);


/*-- b64enc.c and b64dec.c --*/
struct b64state
{
  unsigned int flags;
  int idx;
  int quad_count;
  FILE *fp;
  estream_t stream;
  char *title;
  unsigned char radbuf[4];
  u32 crc;
  int stop_seen:1;
  int invalid_encoding:1;
  gpg_error_t lasterr;
};

gpg_error_t b64enc_start (struct b64state *state, FILE *fp, const char *title);
gpg_error_t b64enc_start_es (struct b64state *state, estream_t fp,
                             const char *title);
gpg_error_t b64enc_write (struct b64state *state,
                          const void *buffer, size_t nbytes);
gpg_error_t b64enc_finish (struct b64state *state);

gpg_error_t b64dec_start (struct b64state *state, const char *title);
gpg_error_t b64dec_proc (struct b64state *state, void *buffer, size_t length,
                         size_t *r_nbytes);
gpg_error_t b64dec_finish (struct b64state *state);

/*-- sexputil.c */
char *canon_sexp_to_string (const unsigned char *canon, size_t canonlen);
void log_printcanon (const char *text,
                     const unsigned char *sexp, size_t sexplen);
void log_printsexp (const char *text, gcry_sexp_t sexp);

gpg_error_t make_canon_sexp (gcry_sexp_t sexp,
                             unsigned char **r_buffer, size_t *r_buflen);
gpg_error_t make_canon_sexp_pad (gcry_sexp_t sexp, int secure,
                                 unsigned char **r_buffer, size_t *r_buflen);
gpg_error_t keygrip_from_canon_sexp (const unsigned char *key, size_t keylen,
                                     unsigned char *grip);
int cmp_simple_canon_sexp (const unsigned char *a, const unsigned char *b);
unsigned char *make_simple_sexp_from_hexstr (const char *line,
                                             size_t *nscanned);
int hash_algo_from_sigval (const unsigned char *sigval);
unsigned char *make_canon_sexp_from_rsa_pk (const void *m, size_t mlen,
                                            const void *e, size_t elen,
                                            size_t *r_len);
gpg_error_t get_rsa_pk_from_canon_sexp (const unsigned char *keydata,
                                        size_t keydatalen,
                                        unsigned char const **r_n,
                                        size_t *r_nlen,
                                        unsigned char const **r_e,
                                        size_t *r_elen);

int get_pk_algo_from_key (gcry_sexp_t key);
int get_pk_algo_from_canon_sexp (const unsigned char *keydata,
                                 size_t keydatalen);
char *pubkey_algo_string (gcry_sexp_t s_pkey);

/*-- convert.c --*/
int hex2bin (const char *string, void *buffer, size_t length);
int hexcolon2bin (const char *string, void *buffer, size_t length);
char *bin2hex (const void *buffer, size_t length, char *stringbuf);
char *bin2hexcolon (const void *buffer, size_t length, char *stringbuf);
const char *hex2str (const char *hexstring,
                     char *buffer, size_t bufsize, size_t *buflen);
char *hex2str_alloc (const char *hexstring, size_t *r_count);

/*-- percent.c --*/
char *percent_plus_escape (const char *string);
char *percent_plus_unescape (const char *string, int nulrepl);
char *percent_unescape (const char *string, int nulrepl);

size_t percent_plus_unescape_inplace (char *string, int nulrepl);
size_t percent_unescape_inplace (char *string, int nulrepl);

/*-- openpgp-oid.c --*/
gpg_error_t openpgp_oid_from_str (const char *string, gcry_mpi_t *r_mpi);
char *openpgp_oidbuf_to_str (const unsigned char *buf, size_t len);
char *openpgp_oid_to_str (gcry_mpi_t a);
int openpgp_oidbuf_is_ed25519 (const void *buf, size_t len);
int openpgp_oid_is_ed25519 (gcry_mpi_t a);
int openpgp_oidbuf_is_cv25519 (const void *buf, size_t len);
int openpgp_oid_is_cv25519 (gcry_mpi_t a);
const char *openpgp_curve_to_oid (const char *name, unsigned int *r_nbits);
const char *openpgp_oid_to_curve (const char *oid, int canon);
const char *openpgp_enum_curves (int *idxp);
const char *openpgp_is_curve_supported (const char *name,
                                        int *r_algo, unsigned int *r_nbits);


/*-- homedir.c --*/
const char *standard_homedir (void);
const char *default_homedir (void);
void gnupg_set_homedir (const char *newdir);
const char *gnupg_homedir (void);
int gnupg_default_homedir_p (void);
const char *gnupg_daemon_rootdir (void);
const char *gnupg_socketdir (void);
const char *gnupg_sysconfdir (void);
const char *gnupg_bindir (void);
const char *gnupg_libexecdir (void);
const char *gnupg_libdir (void);
const char *gnupg_datadir (void);
const char *gnupg_localedir (void);
const char *gnupg_cachedir (void);
const char *dirmngr_socket_name (void);

char *_gnupg_socketdir_internal (int skip_checks, unsigned *r_info);

/* All module names.  We also include gpg and gpgsm for the sake for
   gpgconf. */
#define GNUPG_MODULE_NAME_AGENT        1
#define GNUPG_MODULE_NAME_PINENTRY     2
#define GNUPG_MODULE_NAME_SCDAEMON     3
#define GNUPG_MODULE_NAME_DIRMNGR      4
#define GNUPG_MODULE_NAME_PROTECT_TOOL 5
#define GNUPG_MODULE_NAME_CHECK_PATTERN 6
#define GNUPG_MODULE_NAME_GPGSM         7
#define GNUPG_MODULE_NAME_GPG           8
#define GNUPG_MODULE_NAME_CONNECT_AGENT 9
#define GNUPG_MODULE_NAME_GPGCONF       10
#define GNUPG_MODULE_NAME_DIRMNGR_LDAP  11
#define GNUPG_MODULE_NAME_GPGV          12
const char *gnupg_module_name (int which);
void gnupg_module_name_flush_some (void);
void gnupg_set_builddir (const char *newdir);



/*-- gpgrlhelp.c --*/
void gnupg_rl_initialize (void);

/*-- helpfile.c --*/
char *gnupg_get_help_string (const char *key, int only_current_locale);

/*-- localename.c --*/
const char *gnupg_messages_locale_name (void);

/*-- miscellaneous.c --*/

/* This function is called at startup to tell libgcrypt to use our own
   logging subsystem. */
void setup_libgcrypt_logging (void);

/* Print an out of core message and die.  */
void xoutofcore (void);

/* Same as estream_asprintf but die on memory failure.  */
char *xasprintf (const char *fmt, ...) GPGRT_ATTR_PRINTF(1,2);
/* This is now an alias to estream_asprintf.  */
char *xtryasprintf (const char *fmt, ...) GPGRT_ATTR_PRINTF(1,2);

/* Replacement for gcry_cipher_algo_name.  */
const char *gnupg_cipher_algo_name (int algo);

void obsolete_option (const char *configname, unsigned int configlineno,
                      const char *name);

const char *print_fname_stdout (const char *s);
const char *print_fname_stdin (const char *s);
void print_utf8_buffer3 (estream_t fp, const void *p, size_t n,
                         const char *delim);
void print_utf8_buffer2 (estream_t fp, const void *p, size_t n, int delim);
void print_utf8_buffer (estream_t fp, const void *p, size_t n);
void print_utf8_string (estream_t stream, const char *p);
void print_hexstring (FILE *fp, const void *buffer, size_t length,
                      int reserved);
char *try_make_printable_string (const void *p, size_t n, int delim);
char *make_printable_string (const void *p, size_t n, int delim);

int is_file_compressed (const char *s, int *ret_rc);

int match_multistr (const char *multistr,const char *match);

int gnupg_compare_version (const char *a, const char *b);

struct debug_flags_s
{
  unsigned int flag;
  const char *name;
};
int parse_debug_flag (const char *string, unsigned int *debugvar,
                      const struct debug_flags_s *flags);


/*-- Simple replacement functions. */

/* We use the gnupg_ttyname macro to be safe not to run into conflicts
   which an extisting but broken ttyname.  */
#if !defined(HAVE_TTYNAME) || defined(HAVE_BROKEN_TTYNAME)
# define gnupg_ttyname(n) _gnupg_ttyname ((n))
/* Systems without ttyname (W32) will merely return NULL. */
static inline char *
_gnupg_ttyname (int fd)
{
  (void)fd;
  return NULL;
}
#else /*HAVE_TTYNAME*/
# define gnupg_ttyname(n) ttyname ((n))
#endif /*HAVE_TTYNAME */

#ifdef HAVE_W32CE_SYSTEM
#define getpid() GetCurrentProcessId ()
char *_gnupg_getenv (const char *name); /* See sysutils.c */
#define getenv(a)  _gnupg_getenv ((a))
char *_gnupg_setenv (const char *name); /* See sysutils.c */
#define setenv(a,b,c)  _gnupg_setenv ((a),(b),(c))
int _gnupg_isatty (int fd);
#define gnupg_isatty(a)  _gnupg_isatty ((a))
#else
#define gnupg_isatty(a)  isatty ((a))
#endif



/*-- Macros to replace ctype ones to avoid locale problems. --*/
#define spacep(p)   (*(p) == ' ' || *(p) == '\t')
#define digitp(p)   (*(p) >= '0' && *(p) <= '9')
#define alphap(p)   ((*(p) >= 'A' && *(p) <= 'Z')       \
                     || (*(p) >= 'a' && *(p) <= 'z'))
#define alnump(p)   (alphap (p) || digitp (p))
#define hexdigitp(a) (digitp (a)                     \
                      || (*(a) >= 'A' && *(a) <= 'F')  \
                      || (*(a) >= 'a' && *(a) <= 'f'))
  /* Note this isn't identical to a C locale isspace() without \f and
     \v, but works for the purposes used here. */
#define ascii_isspace(a) ((a)==' ' || (a)=='\n' || (a)=='\r' || (a)=='\t')

/* The atoi macros assume that the buffer has only valid digits. */
#define atoi_1(p)   (*(p) - '0' )
#define atoi_2(p)   ((atoi_1(p) * 10) + atoi_1((p)+1))
#define atoi_4(p)   ((atoi_2(p) * 100) + atoi_2((p)+2))
#define xtoi_1(p)   (*(p) <= '9'? (*(p)- '0'): \
                     *(p) <= 'F'? (*(p)-'A'+10):(*(p)-'a'+10))
#define xtoi_2(p)   ((xtoi_1(p) * 16) + xtoi_1((p)+1))
#define xtoi_4(p)   ((xtoi_2(p) * 256) + xtoi_2((p)+2))

#endif /*GNUPG_COMMON_UTIL_H*/
