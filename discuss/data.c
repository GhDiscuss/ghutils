#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"
#include "state.h"

static const AElement* comment(Value *const v, const char *idx) {
  const AElement *e = node(v, "comments");
  if(e)
    return atoi(number(e->value)) == atoi(idx) ? e : NULL;
  puts("# no such comment");
  return NULL;
}

static Value* get_data(Value *const v, char **argv) {
  const OElement *data = findv(v, "data");
  return data ? data->value : NULL;
}

#define GET(name, parent, func) \
static Value* get_##name(Value *const v, char **argv) { \
  Value *const parent = get_##parent(v, argv); \
  if(!parent) \
    return NULL; \
  const OElement *name = findv(parent, #name); \
  if(name && json_value_as_object(name->value)) \
    return name->value; \
  printf("# no such %s\n", #name); \
  return NULL; \
}

GET(viewer, data, findv)
GET(organization, data, findv)
GET(team, organization, findv)
GET(discussion, team, findv)

static Value* get_comment(Value *const v, char **argv) {
  Value *const discussion = get_discussion(v, argv);
  if(!discussion)
    return NULL;
  const AElement *a = comment(discussion, argv[4]);
  return a ? a->value : NULL;
}

typedef Value* (*get_f)(Value *const, char**);
static get_f _get_value[] = { get_viewer, get_organization, get_team, get_discussion, get_comment };

struct json_value_s* get_value(struct CurlHelper *ch, struct State *state) {
  return _get_value[state->argc](ch->value, state->argv);
}
