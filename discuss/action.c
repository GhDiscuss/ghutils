#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"
#include "ql.h"
#include "state.h"
#include "discuss/pp.h"

#define NUSED __attribute__((unused))

static Value* react(Value *const base, const int idx) {
  const AElement *reactions = node(base, "reactions");
  while(reactions) {
    const OElement *content = findae(reactions, "content");
    const OElement *user = findae(reactions, "user");
    const OElement *viewer = findoe(user, "isViewer");
    const int is_viewer = json_value_is_true(viewer->value);
    const char *str = string(content);
    if(!strcmp(str, reaction_names[idx])) {
      if(is_viewer)
        return reactions->value;
    }
    reactions = reactions->next;
  }
  return NULL;
}

static int create_comment(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "createTeamDiscussionComment(input:{discussionId:\\\"%s\\\",body:\\\"%s\\\"}){teamDiscussionComment{id}}}\"}";
  create(curl, base, fmt, 1, NULL);
  return 0;
}

static int list_orgs(CURL *curl NUSED, Value *const base) {
  AElement *e = node(base, "organizations");
  while(e) {
    pp2(e->value);
    e = e->next;
  }
  return 0;
}

static int list_teams(CURL *curl NUSED, Value *const base) {
  AElement *e = node(base, "teams");
  while(e) {
    pp2(e->value);
    e = e->next;
  }
  return 0;
}

static int list_discussions(CURL *curl NUSED, Value *const base) {
  AElement *e = node(base, "discussions");
  while(e) {
    printf("\n");
    pp1(e->value, "##");
    e = e->next;
  }
  return 0;
}

static int pp_discussion(CURL *curl NUSED, Value *const base) {
  pp1(base, "#");
  AElement *e = node(base, "comments");
  while(e) {
    printf("  \n\n");
    pp1(e->value, "###");
    e = e->next;
  }
  return 0;
}
static int create_discussion(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "createTeamDiscussion(input:{teamId:\\\"%s\\\",title:\\\"%s\\\",body:\\\"%s\\\"}){teamDiscussion{body}}}\"}";
  struct String title = {};
  create(curl, base, fmt, 1, &title);
  free(title.memory);
  return 0;
}

static int delete_discussion(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "deleteTeamDiscussion(input:{id:\\\"%s\\\"}){clientMutationId}}\"}";
  mutate(curl, base, fmt, 0, NULL);
  return 0;
}

static int edit_discussion(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "updateTeamDiscussion(input:{id:\\\"%s\\\",title:\\\"%s\\\",body:\\\"%s\\\"}){teamDiscussion{id}}}\"}";
  struct String title = {};
  create(curl, base, fmt, 0, &title);
  free(title.memory);
  return 0;
}

static int delete_comment(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "deleteTeamDiscussionComment(input:{id:\\\"%s\\\"}){clientMutationId}}\"}";
  mutate(curl, base, fmt, 0, NULL);
  return 0;
}

static int edit_comment(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "updateTeamDiscussionComment(input:{id:\\\"%s\\\",body:\\\"%s\\\"}){teamDiscussionComment{id}}}\"}";
  /*return */ create(curl, base, fmt, 0, NULL);
  return 0;
}

static int pp_comment(CURL *curl NUSED, Value *const base) {
  pp1(base, "###");
  return 0;
}

#define react_comment(name,idx,emoj) \
static int name##_comment(CURL *curl, Value *const base) { \
  Value *const v = react(base, idx);\
  if(!v) {\
  char* fmt = MUTATION "addReaction(input:{subjectId:\\\"%s\\\",content:"#emoj"}){clientMutationId}}\"}";\
  mutate(curl, base, fmt, 0, NULL);\
  } else {\
  char* fmt = MUTATION "removeReaction(input:{subjectId:\\\"%s\\\",content:"#emoj"}){clientMutationId}}\"}";\
  mutate(curl, base, fmt, 0, NULL);\
  }\
  return 0;\
}
// renaming
react_comment(confused, 0, CONFUSED)
react_comment(eyes, 1, EYES)
react_comment(heart, 2, HEART)
react_comment(hooray, 3, HOORAY)
react_comment(rocket, 3, ROCKET)
react_comment(laugh, 4, LAUGH)
react_comment(minus, 6, THUMBS_DOWN)
react_comment(plus, 7, THUMBS_UP)

static const action_f actions0[] = { list_orgs };
static const action_f actions1[] = { list_teams };
static const action_f actions2[] = { list_discussions, create_discussion };
static const action_f actions3[] = { pp_discussion, create_comment, delete_discussion, edit_discussion,
  confused_comment, eyes_comment, heart_comment, hooray_comment, laugh_comment, rocket_comment, minus_comment, plus_comment };
static const action_f actions4[] = { pp_comment, NULL, delete_comment, edit_comment,
  confused_comment, eyes_comment, heart_comment, hooray_comment, laugh_comment, rocket_comment, minus_comment, plus_comment };
static const action_f *actions[] = { actions0, actions1, actions2, actions3, actions4 };

action_f get_action(struct State *state) {
  return actions[state->argc][state->action];
}
