/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      scanf_ska.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   My scanf implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
  int modifier;
  int format;
} fmt_t;

typedef struct
{
  const char *fmt;
  const char *str;
  size_t slen;
  int cur_arg;
  int n_args;
  void *args[];
} scanf_struct_t;

#define chk_supported(x, y) do { if ( (x) ) { fprintf(stderr, "%s is not supported\n", y); return 0; } } while(0)

static int is_modifier(char c)
{
  return ( c == 'l' || c == 'L' );
}

static int is_format(char c)
{
#define OK(x) (c == (x))
  return OK('%') || /* Literal '%' */
    OK('d') || OK('i') || OK('u') || /* int */
    OK('x') || OK('X') || OK('o') || OK('p') || /* int, hex/oct pointer */
    OK('s') || OK('c') ||  /* String, char */
    OK('f') || OK('e') || OK('g') || OK('E') || OK('a'); /* Floats */
#undef OK
}

static const char *convert_number(const char *s, int is_signed, size_t size, int base, void *arg)
{
  char *endp;

  assert(size == sizeof(int) ||
         size == sizeof(long long));

  if (is_signed)
    {
      if (size == sizeof(int))
        {
          int *p = (int*)arg;

          *p = strtol(s, &endp, base);
        }
      else
        {
          long long *p = (long long*)arg;

          *p = strtoll(s, &endp, base);
        }
    }
  else
    {
      if (size == sizeof(unsigned int))
        {
          unsigned int *p = (unsigned int*)arg;

          *p = strtoul(s, &endp, base);
        }
      else
        {
          unsigned long long *p = (unsigned long long*)arg;

          *p = strtoull(s, &endp, base);
        }
    }

  if (endp == s) /* Something was wrong! */
    return NULL;

  return endp; /* First character after */
}

static const char *convert_float(const char *s, size_t size, void *arg)
{
#if defined(FLOAT_SUPPORT)
#else
  return NULL;
#endif
}

static const char *convert_string(const char *s, void *arg)
{
  char *p = (char*)arg;

  while( !isspace(*s) && *s != '\0')
    {
      *p = *s;
      s++;
      p++;
    }
  *p = '\0';

  return s;
}

static const char *convert_char(const char *s, void *arg)
{
  char *p = (char*)arg;

  *p = *s;

  return s + 1;
}

static int parse_fmt(scanf_struct_t *sp)
{
  char modifier = 0;
  char fmt = 0;
  const char *p = sp->fmt + 1; /* Points to first character after % */
  const char *s = sp->str;

  assert(sp->slen > 2); /* % + at least one fmt */
  assert(*sp->fmt == '%');

  /* Some unsupported modifiers */
  chk_supported(isdigit(*p), "%n$ format");
  chk_supported(*p == 'h', "h modifier");
  chk_supported(*p == 'q', "q modifier");
  chk_supported(*p == 't', "t modifier");
  chk_supported(*p == 'z', "z modifier");

  /* Maybe modifier */
  if (is_modifier(*p))
    {
      modifier = *p;
      p++;
    }

  /* Unsupported formats */
  chk_supported(*p == '[', "[ format");
  chk_supported(*p == 'n', "n format");

  if (!is_format(*p))
    {
      /* Unknown format! Cannot be converted */
      fprintf(stderr, "Uknown scanf format %c!\n", *p);
      return 0;
    }
  fmt = *p;
  p++;

  switch(fmt)
    {
    case '%':
      /* Not an actual argument, just return without handling */
      sp->fmt = p;
      sp->str = s + 1;
      return 0;
    case 'i':
    case 'd':
      s = convert_number(s, 1, sizeof(int), 10, sp->args[sp->cur_arg]);
      break;
    case 'o':
      s = convert_number(s, 1, sizeof(int), 8, sp->args[sp->cur_arg]);
      break;
    case 'x':
    case 'X':
    case 'p':
      s = convert_number(s, 1, sizeof(int), 16, sp->args[sp->cur_arg]);
      break;
    case 'f':
    case 'e':
    case 'g':
    case 'E':
    case 'a':
      s = convert_float(s, sizeof(float), sp->args[sp->cur_arg]);
      break;
    case 's':
      s = convert_string(s, sp->args[sp->cur_arg]);
      break;
    case 'c':
      s = convert_char(s, sp->args[sp->cur_arg]);
      break;
    default:
      /* Should never happen */
      assert(0);
      break;
    }

  /* The string pointer has not moved */
  if (s == sp->str)
    return 0;

  /* Commit the things back to the structure */
  sp->fmt = p;
  sp->str = s;

  sp->cur_arg++;

  return 1; /* Handled */
}

static int run_scanf(scanf_struct_t *sp)
{
  const char *end = sp->fmt + sp->slen;
  int n = 0; /* Number of handled args */
  int is_backspaced = 0;

  while (sp->fmt < end)
    {
      if (*sp->fmt == '\\' && *(sp->fmt + 1) == '%')
        {
          is_backspaced = 1;
          sp->fmt++;
          continue;
        }

      if (*sp->fmt == '%' && !is_backspaced)
        n += parse_fmt(sp);
      else
        {
          char cf = *sp->fmt;
          char cs = *sp->str;

          is_backspaced = 0;
          sp->fmt++;
          sp->str++;

          if (cf != cs)
            return n;
        }
    }

  return n;
}

static scanf_struct_t *alloc_scanf_struct(const char *fmt, const char *str,
                                          size_t slen, int n_args)
{
  scanf_struct_t *out = (scanf_struct_t*)malloc(sizeof(scanf_struct_t) + n_args * sizeof(void*));

  if (!out)
    return NULL;

  memset(out, 0, sizeof(scanf_struct_t) + n_args * sizeof(void*));

  out->fmt = fmt;
  out->str = str;
  out->slen = slen;
  out->n_args = n_args;

  return out;
}

static scanf_struct_t *create_scanf_struct_from_vargs(const char *fmt, const char *str,
                                                      size_t slen, va_list arg_ptr)
{
  int n = 16;
  scanf_struct_t *out = alloc_scanf_struct(fmt, str, slen, n);
  void *arg;
  int i = 0;

  assert(out);

  for (arg = va_arg(arg_ptr, void*); arg != NULL; arg = va_arg(arg_ptr, void*))
    {
      /* Realloc on very long arg strings, should be quite uncommon */
      if (i >= n)
        {
          scanf_struct_t *p;
          int j;

          n *= 2;
          p = alloc_scanf_struct(fmt, str, slen, n);
          assert(p);

          for (j = 0; j < i; j++)
            p->args[j] = out->args[j];

          free(out);
          out = p;
        }
      out->args[i] = arg;
      i++;
    }

  return out;
}

static void free_scanf_struct(scanf_struct_t *st)
{
  free(st);
}


int vsscanf(const char* str, const char* format, va_list arg_ptr)
{
  scanf_struct_t *sp = create_scanf_struct_from_vargs(format, str, strlen(str), arg_ptr);
  int out;

  out = run_scanf(sp);
  free_scanf_struct(sp);

  return out;
}

int sscanf(const char *str, const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n = vsscanf(str,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}

#if 0
/* No ungetc */
int vfscanf(FILE *stream, const char *format, va_list arg_ptr)
{
  struct arg_scanf farg = { (void*)stream, (int(*)(void*))fgetc, (int(*)(int,void*))ungetc };
  return __v_scanf(&farg,format,arg_ptr);
}

int fscanf(FILE *stream, const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n = vfscanf(stream,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}

int vscanf(const char *format, va_list arg_ptr)
{
  return vfscanf(stdin,format,arg_ptr);
}

int scanf(const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n = vfscanf(stdin,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}
#endif

#if defined(SCANF_TEST)

static void test_1(void)
{
  const char *s = "Mister klister %d 75 hej % hopp f: maboo 0xabcd\n";
  scanf_struct_t *sp = alloc_scanf_struct("Mister klister \\%d %d hej %% hopp %c: %s 0x%x\n",
                                          s, strlen(s), 4);
  int i, v, v2;
  char str[80];
  char c;

  sp->args[0] = &i;
  sp->args[1] = &c;
  sp->args[2] = str;
  sp->args[3] = &v2;
  v = run_scanf(sp);
  printf("t1: %d:%d:%c:%s:0x%x\n", v, i, c, str, v2);

  free_scanf_struct(sp);
}

int main(int argc, char *argv[])
{
  test_1();

  return 0;
}

#endif /* SCANF_TEST */
