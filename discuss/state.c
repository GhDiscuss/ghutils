#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"
#include "ql.h"
#include "state.h"

static char *const get0 = QUERY "viewer{organizations(first:100){nodes{name description membersWithRole(first:100){nodes{login}}}}}}\"}";
static char *const get1 = QUERY ORG "teams(first:100){nodes{name description members(first:100){nodes{login}}}}}}\"}";
static char *const get2 = QUERY ORG TEAM "id discussions(first:100){nodes{" DISCUSSION "}}}}}\"}";
static char *const get3 = QUERY ORG TEAM "discussion(number:%s){" DISCUSSION "comments(first:100){nodes{" CONTENT "}}}}}}\"}";
static char *const get4 = QUERY ORG TEAM "discussion(number:%s){ "DISCUSSION "comments(first:1,fromComment:%s){nodes{" CONTENT "}}}}}}\"}";
static char *const get_fmt[] = { get0, get1, get2, get3, get4 };

static char *const mut2 = QUERY ORG TEAM "id}}}\"}";
static char *const mut3 = QUERY ORG TEAM "discussion(number:%s){id number}}}}\"}";
static char *const mut4 = QUERY ORG TEAM "discussion(number:%s){comments(first:1,fromComment:%s){nodes{id number }}}}}}\"}";
static char *const mut_fmt[] = { NULL, NULL, mut2, mut3, mut4 };

static char *const edit3 = QUERY ORG TEAM "discussion(number:%s){ id title body}}}}\"}";
static char *const edit4 = QUERY ORG TEAM "discussion(number:%s){ comments(first:1,fromComment:%s){nodes{id number body}}}}}}\"}";
static char *const edit_fmt[]= { NULL, NULL, NULL, edit3, edit4 };

static char *const react3 = QUERY ORG TEAM "discussion(number:%s){id " REACTIONS "}}}}\"}";
static char *const react4 = QUERY ORG TEAM "discussion(number:%s){comments(first:1,fromComment:%s){nodes{id " REACTIONS " number}}}}}}\"}";
static char *const react_fmt[]= { NULL, NULL, NULL, react3, react4 };

static char *const* fmts[] = { get_fmt, mut_fmt, mut_fmt, edit_fmt,
  react_fmt, react_fmt, react_fmt, react_fmt, react_fmt, react_fmt, react_fmt, react_fmt };

int init_state(struct State *s) {
  ++s->argv;
  if(--s->argc) {
    if((!strcmp(s->argv[0], "create")   && (s->action = CREATE))      ||
       (!strcmp(s->argv[0], "delete")   && (s->action = DELETE))      ||
       (!strcmp(s->argv[0], "edit")     && (s->action = EDIT))        ||
       (!strcmp(s->argv[0], "confused") && (s->action = CONFUSED))    ||
       (!strcmp(s->argv[0], "eyes")     && (s->action = EYES))        ||
       (!strcmp(s->argv[0], "heart")    && (s->action = HEART))       ||
       (!strcmp(s->argv[0], "hooray")   && (s->action = HOORAY))      ||
       (!strcmp(s->argv[0], "laugh")    && (s->action = LAUGH))       ||
       (!strcmp(s->argv[0], "-1")       && (s->action = THUMBS_DOWN)) ||
       (!strcmp(s->argv[0], "+1")       && (s->action = THUMBS_UP))   ||
        !strcmp(s->argv[0], "get")) {
        ++s->argv;
        --s->argc;
    } else
    s->action = GET;
  }
  if((s->fmt = fmts[s->action][s->argc]))
    return 1;
  perror("# undefined action");
  return 0;
}
