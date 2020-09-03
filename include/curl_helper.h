struct String {
  char *memory;
  size_t size;
};

struct CurlHelper {
  CURL *curl;
  struct curl_slist *slist;
  struct json_value_s *value;
};

int ch_init(struct CurlHelper* ch, const char*);
Value * parse(CURL *const curl, const char *sdata);
static inline void simple_parse(CURL *const curl, const char *sdata) {
  Value *const v = parse(curl, sdata);
  if(v)
    free(v);
}

void ch_cleanup(struct CurlHelper *ch);
