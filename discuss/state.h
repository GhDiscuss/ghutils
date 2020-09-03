typedef int (*action_f)(CURL*, Value *const);

enum action {
  GET,
  CREATE,
  DELETE,
  EDIT,
  CONFUSED,
  EYES,
  HEART,
  HOORAY,
  LAUGH,
  ROCKET,
  THUMBS_DOWN,
  THUMBS_UP,
};

struct State {
  int argc;
  char **argv;
  char *fmt;
  enum action action;
};

int init_state(struct State *s);
