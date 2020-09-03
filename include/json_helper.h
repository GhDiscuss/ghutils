typedef struct json_value_s Value;
typedef struct json_object_s Object;
typedef struct json_object_element_s OElement;
typedef struct json_array_element_s  AElement;

static inline const OElement* find(const Object *o, const char* name) {
  OElement *e = o->start;
  do if(!strcmp(e->name->string, name))
    return e;
  while((e = e->next));
  return NULL;
}

static inline const OElement* findv(Value *const v, const char* name) {
  const Object *o = json_value_as_object(v);
  return o ? find(json_value_as_object(v), name) : NULL;
}

static inline const OElement* findoe(const OElement *e, const char* name) {
  return findv(e->value, name);
}

static inline const OElement* findae(const AElement *e, const char* name) {
  return findv(e->value, name);
}

static inline const char* string(const OElement *e) {
  return json_value_as_string(e->value)->string;
}

static inline AElement* node(Value *const v, const char *str) {
  const Object *o = json_value_as_object(v);
  if(!o)
    return NULL;
  const OElement *elem = find(o, str);
  if(!elem)
    return NULL;
  const OElement *node = findoe(elem, "nodes");
  if(!node)
    return NULL;
  return json_value_as_array(node->value)->start;
}

static inline const char* number(Value *v) {
  const OElement *e = findv(v, "number");
  return json_value_as_number(e->value)->number;
}
