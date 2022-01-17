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

  char *lf = loc;
  while (*lf != '\n' && *lf != EOF)
    lf++;
  int off = lf - usr_in;
  fprintf(stderr, "%.*s\n", off, usr_in);

  char *line_start = loc;
  while (*line_start != '\n' && line_start >= usr_in)
    line_start--;
  int spaces = loc - line_start - 1;
  if (spaces)
  fprintf(stderr, "%*s", spaces, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}