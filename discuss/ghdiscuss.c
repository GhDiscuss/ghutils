#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_helper.h"
#include "curl_helper.h"
#include "state.h"
#include "discuss/data.h"
#include "discuss/pp.h"
#include "discuss/action.h"

#define NUSED __attribute__((unused))

int init(struct CurlHelper* ch, struct State *state) {
  if(!init_state(state))
    return 0;
  char **argv = state->argv;
  int argc = state->argc;
  if(asprintf(&state->data, state->fmt, argv[0], argv[1%(argc+1)], argv[2%(argc+1)], argv[3%(argc+1)]) < 0)
    return 0;
  return ch_init(ch, state->data);
}

int run(struct CurlHelper *ch, struct State *state) {
  Value *const base = get_value(ch, state);
  if(!base || !json_value_as_object(base)) {
    printf("# can't find value");
    return 1;
  }
  const action_f action = get_action(state);
  return action(ch->curl, base);
}

int main(int argc, char **argv) {
  struct State state = { .argc=argc, argv=argv };
  struct CurlHelper ch = {};
  const int ret = init(&ch, &state) && run(&ch, &state);
  if(state.data)
    free(state.data);
  ch_cleanup(&ch);
  return ret;
}
