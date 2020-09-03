#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"
#include "ql.h"
#include "state.h"
#include "discuss/pp.h"
#include "discuss/reaction.h"

#define NUSED __attribute__((unused))

#define ITERATE(name, action)      \
  AElement *e = node(base, #name); \
  while(e) {                       \
    action                         \
    e = e->next;                   \
  }

static int list_orgs(CURL *curl NUSED, Value *const base) {
  ITERATE(organizations, pp_meta(e->value);)
  return 0;
}

static int list_teams(CURL *curl NUSED, Value *const base) {
  ITERATE(teams, pp_meta(e->value);)
  return 0;
}

static int list_discussions(CURL *curl NUSED, Value *const base) {
  ITERATE(discussions, printf("\n"); pp(e->value, "##");)
  return 0;
}

static int pp_discussion(CURL *curl NUSED, Value *const base) {
  pp(base, "#");
  ITERATE(comments, printf("  \n\n"); pp(e->value, "###");)
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

static int create_comment(CURL *curl, Value *const base) {
  const char *fmt = MUTATION "createTeamDiscussionComment(input:{discussionId:\\\"%s\\\",body:\\\"%s\\\"}){teamDiscussionComment{id}}}\"}";
  create(curl, base, fmt, 1, NULL);
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
  pp(base, "###");
  return 0;
}

static int addReaction(CURL *curl, Value *const base) {
  char* fmt = MUTATION "addReaction(input:{subjectId:\\\"%s\\\",content:%s}){clientMutationId}}\"}";
  mutate(curl, base, fmt, 0, NULL);
  return 0;
}

static int removeReaction(CURL *curl, Value *const base) {
  char* fmt = MUTATION "removeReaction(input:{subjectId:\\\"%s\\\",content:%s}){clientMutationId}}\"}";
  mutate(curl, base, fmt, 0, NULL);
  return 0;
}

static int _react_comment(CURL *curl, Value *const base, const char *emoj, const unsigned int idx) {
  return (react(base, idx) ? addReaction : removeReaction)(curl, base);
}

#define react_comment(name,idx,emoj) \
static int name##_comment(CURL *curl, Value *const base) { \
  return _react_comment(curl, base, #emoj, idx);           \
}

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
