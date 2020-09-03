#define QUERY "{\"query\":\"query{"
#define MUTATION "{\"query\":\"mutation{"
#define REACTIONS "reactions(first:100){nodes{content id user{isViewer}}}"
#define ORG "organization(login:\\\"%s\\\"){"
#define TEAM "team(slug:\\\"%s\\\"){"
#define CONTENT "author{login} id viewerDidAuthor body number createdAt updatedAt " REACTIONS
#define DISCUSSION "title " CONTENT REACTIONS

void mutate(CURL *const curl, Value *const v, const char *fmt, const char *s0, const char *s1);
void create(CURL *const curl, Value *const v, const char *fmt, const uint is_new, struct String *s0);
