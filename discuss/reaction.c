#include <stdio.h>
#include <string.h>
#include "json.h"
#include "json_helper.h"
#include "discuss/reaction.h"

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

void reactions_pp(Value *const v) {
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

Value* react(Value *const base, const int idx) {
  const AElement *reactions = node(base, "reactions");
  while(reactions) {
    int is_viewer;
    const char *str = get_reaction(reactions, &is_viewer);
    if(!strcmp(str, reaction_names[idx])) {
      if(is_viewer)
        return reactions->value;
    }
    reactions = reactions->next;
  }
  return NULL;
}
