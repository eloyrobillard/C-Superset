#include "9cc.h"

// Reports an error and exit.
void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int off = loc - usr_in;
  fprintf(stderr, "%s\n", usr_in);
  fprintf(stderr, "%*s", off, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}