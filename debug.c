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
  char *line_start = loc;
  while (*lf != '\n' && *lf != EOF)
    lf++;
  while (*line_start != '\n' && line_start >= usr_in)
    line_start--;
  int off = lf - line_start;

  char *line_counter = usr_in;
  int lc = 1;
  while (line_counter++ < loc)
    if (*(line_counter) == '\n')
      lc++;

  fprintf(stderr, "\n%d:%d\n%.*s\n", lc, off-1, off-1, line_start+1);
  int spaces = loc - line_start - 1;
  if (spaces)
    fprintf(stderr, "%*s", spaces, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n\n");
  exit(1);
}

void warning(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  return va_end(ap);
}

void warning_at(char *loc, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  char *lf = loc;
  char *line_start = loc;
  while (*lf != '\n' && *lf != EOF)
    lf++;
  while (*line_start != '\n' && line_start >= usr_in)
    line_start--;
  int off = lf - line_start;

  char *line_counter = usr_in;
  int lc = 1;
  while (line_counter++ < loc)
    if (*(line_counter) == '\n')
      lc++;

  fprintf(stderr, "\n%d:%d\n%.*s\n", lc, off-1, off-1, line_start+1);
  int spaces = loc - line_start - 1;
  if (spaces)
    fprintf(stderr, "%*s", spaces, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n\n");
  return va_end(ap);
}