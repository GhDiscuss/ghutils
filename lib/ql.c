#define _GNU_SOURCE
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "curl/curl.h"
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"

void mutate(CURL *const curl, Value *const v, const char *fmt, const char *s0, const char *s1) {
  const OElement *id = findv(v, "id");
  char *str;
  asprintf(&str, fmt, string(id), s0, s1);
  simple_parse(curl, str);
  free(str);
}

static char* escape(char *ptr, const long len) {
  char *buf = malloc(len * 2 + 1);
  long i, j;
  for(i = 0, j = 0; i < len-1; ++i) {
    if(ptr[i] == '\n') {
      buf[j++] = '\\';
      buf[j++] = 'n';
    } else {
      if(ptr[i] == '"') {
        buf[j++] = '\\';
        buf[j++] = '\\';
        buf[j++] = '\\';
      }
      buf[j++] = ptr[i];
    }
  }
  buf[j] = '\0';
  return buf;
}

// reads file in a string
static int file2str(FILE *f, struct String *const str, const long start) {
  fseek(f, 0L, SEEK_END);
  const long len = ftell(f);
  char *ptr = malloc(len + 1);
  if (ptr == NULL) // Failed to allocate memory
  { perror("calloc"); return 0; };
  fseek(f, start, SEEK_SET); // Or use rewind
  size_t read = fread(ptr, sizeof(char), len , f);
  str->memory = escape(ptr, read); // check memory ?
  free(ptr);
  return 1;
}


static int readtitle(FILE *f, struct String *const restrict title) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  if((read = getline(&line, &len, f)) == -1)
    return 0;
  title->memory = escape(line, read);
  title->size = len;
  free(line);
  return 1;
}

static int readfile(const char *name, struct String *const restrict title, struct String *const restrict body) {
  FILE *f = fopen(name, "r");
  if(!f)
    return 0;
  if(title && !readtitle(f, title)) {
    perror("can't read tmp file");
    return 0;
  }
  long pos = ftell(f);
  const int ret = file2str(f, body, pos);
  fclose(f);
  return ret;
}

static void tempfile(char *const fname, const OElement *restrict title, const OElement *restrict body) {
  const char *_fname = ".ghdiscussXXXXXX";
  strcpy(fname, _fname);
  const int fd = mkostemp(fname, O_CLOEXEC); // check for errors
  if(title) {
    write(fd, string(title), strlen(string(title)));
    if(body)
      write(fd, "\n", 1);
  }
  if(body)
    write(fd, string(body), strlen(string(body)));
  close(fd);
}

static inline void editor(const char *fname) {
  char cmd[64]; // could be smaller
  sprintf(cmd, "vim %.16s", fname); // could use $EDITOR
  system(cmd);
}

static inline void edit(Value *const v, char *const fname) {
  const OElement *title = v ? findv(v, "title") : NULL;
  const OElement *body = v ? findv(v, "body") : NULL;
  tempfile(fname, title, body);
  editor(fname);
}

void create(CURL *const curl, Value *const v, const char *fmt, const uint is_new, struct String *s0) {
  char fname[64];
  edit(!is_new ? v : NULL, fname);
  struct String s1 = {};
  if(readfile(fname, s0, &s1)) {
    mutate(curl, v, fmt, s0 ? s0->memory : s1.memory, s1.memory);
    free(s1.memory);
  }
  remove(fname);
}
