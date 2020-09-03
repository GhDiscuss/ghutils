#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"

static int auth_token(char *c) {
  strcpy(c, "Authorization: bearer ");
  char *token = getenv("GHBOARD_TOKEN");
  if(!token) {
    perror("please set GHBOARD_TOKEN environment variable.");
    return 0;
  }
  strcat(c, token);
  return 1;
}

static struct curl_slist* headers(void) {
  char c[64];
  if(!auth_token(c))
    return NULL;
  struct curl_slist *slist = curl_slist_append(NULL, c);
  if(!slist || !(slist = curl_slist_append(slist, "Content-Type: application/json")))
    return NULL;
  return slist;
}

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
  const size_t realsize = size * nmemb;
  struct String *s = (struct String*)userp;
  char *ptr = realloc(s->memory, s->size + realsize + 1);
  if(!ptr) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  s->memory = ptr;
  memcpy(&(s->memory[s->size]), buffer, realsize);
  s->size += realsize;
  s->memory[s->size] = 0;
  return realsize;
}

int ch_init(struct CurlHelper* ch, const char *data) {
  curl_global_init(CURL_GLOBAL_ALL);
  if(!(ch->curl = curl_easy_init()))
    return 0;
  if(!(ch->slist = headers()))
    return 0;
  curl_easy_setopt(ch->curl, CURLOPT_USERAGENT, "ghboard");
  curl_easy_setopt(ch->curl, CURLOPT_URL, "https://api.github.com/graphql");
  curl_easy_setopt(ch->curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(ch->curl, CURLOPT_HTTPHEADER, ch->slist);
  return !!(ch->value = parse(ch->curl, data));
}

static CURLcode perform(CURL *const curl, const char *sdata) {
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sdata);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(sdata));
  return curl_easy_perform(curl);
}

Value * parse(CURL *const curl, const char *sdata) {
  char error[CURL_ERROR_SIZE];
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
  struct String s = { };
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
  if(perform(curl, sdata) != CURLE_OK) {
    printf("# %s\n", error);
    return NULL;
  }
  Value *const v = json_parse(s.memory, s.size);
  free(s.memory);
  if(v) {
    if(!findv(v, "errors"))
      return v;
    free(v);
  }
  perror("# no data");
  return NULL;
}

void ch_cleanup(struct CurlHelper *ch) {
  if(ch->curl)
    curl_easy_cleanup(ch->curl);
  if(ch->slist)
    curl_slist_free_all(ch->slist);
  if(ch->value)
    free(ch->value);
  curl_global_cleanup();
}
