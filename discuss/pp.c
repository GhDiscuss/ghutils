#include <stdio.h>
#include <string.h>
#include "json.h"
#include "json_helper.h"
#include "discuss/pp.h"
#include "discuss/reaction.h"

static inline void name_pp(Value *v) {
  const OElement *login = findv(v, "name");
  printf("  * \033[1m%s\033[0m", string(login));
}

static inline void description_pp(Value *v) {
  const OElement *description = findv(v, "description");
  if(json_value_as_string(description->value))
    printf("\n\t%s", string(description));
  printf("\n");
}

static inline void title_pp(Value *v) {
  const OElement *title = findv(v, "title");
  if(title)
    printf("# %s\n", string(title));
}

static inline void body_pp(Value *v) {
  const OElement *body = findv(v, "body");
  printf("%s\n", string(body));
}

enum reaction_content                 { rc_confused, rc_eyes, rc_heart, rc_hooray, rc_laugh, rc_rocket, rc_down,       rc_up, rc_last };
static const char *reaction_emoji[] = { ":confused:",  ":eyes:",    ":heart:",     ":hooray:", ":smile:", ":rocket:", ":thumbsdown:", ":thumbsup:" };

static inline uint reaction_pp(const uint rc, const uint author, const char *emoj) {
  if(rc)
    printf("%s%i%s\t", emoj, rc, author ? "." : "");
  return rc;
}

static inline void _reactions_pp(uint *rc, uint *author) {
  int reactions = 0;
  for(enum reaction_content i = 0; i < rc_last; ++i)
    reactions += reaction_pp(rc[i], author[i], reaction_emoji[i]);
  if(reactions)
    printf("\n");
}

static const char* get_reaction(const AElement *reactions, int *is_viewer) {
  const OElement *user = findae(reactions, "user");
  const OElement *viewer = findoe(user, "isViewer");
  *is_viewer = json_value_is_true(viewer->value);
  const OElement *content = findae(reactions, "content");
  return string(content);
}

static void reactions_pp(Value *const v) {
  const AElement *reactions = node(v, "reactions");
  uint rc[rc_last] = {};
  uint author[rc_last] = {};
  while(reactions) {
    int is_viewer;
    const char *str = get_reaction(reactions, &is_viewer);
    for(enum reaction_content i = 0; i < rc_last; ++i) {
      if(!strcmp(str, reaction_names[i])) {
        ++rc[i];
        if(is_viewer)
          ++author[i];
        break;
      }
    }
    reactions = reactions->next;
  }
  _reactions_pp(rc, author);
}

static void member_pp(Value *const v, const char *str) {
  const AElement *member = node(v, str);
  if(!member)
    return;
  printf("> members:");
  while(member) {
    const OElement *login = findae(member, "login");
    printf(" @%s ", string(login));
    member = member->next;
  }
  printf("\n\n");
}

static void header_pp(Value *v, const char *prefix) {
  printf("%s **%s**: ", prefix, number(v));
  const Object *o = json_value_as_object(v);
  const OElement *author = find(o, "author");
  const OElement *author_login = findoe(author, "login");
  const OElement *createdAt = find(o, "createdAt");
  const OElement *updatedAt = find(o, "updatedAt");
  const OElement *authored = find(o, "viewerDidAuthor");
  const char *mark = json_value_is_true(authored->value) ? "*" : "";
  printf("%s%s%s %s", mark, string(author_login), mark, string(createdAt));
  if(strcmp(string(createdAt), string(updatedAt)))
    printf(" (updated %s)", string(updatedAt));
  printf("\n");
}

void pp(Value *v, const char *prefix) {
  header_pp(v, prefix);
  title_pp(v);
  body_pp(v);
  reactions_pp(v);
}

void pp_meta(Value *const v) {
  name_pp(v);
  description_pp(v);
  member_pp(v, "members");
}
