typedef
  struct {
    size_t size;
    char* string;
  } sc_string_t;

typedef
  union {
    sc_string_t string;
    double doublev;
    int integer;
    float floating;
    struct closure_t *closure;
  } svalue_variants_t;

typedef
  enum {
    INT = 0,
    FLOAT = 1,
    DOUBLE = 2,
    STRING = 3
  } stype_t;

typedef
  struct {
    stype_t type_tag;
    svalue_variants_t value;
  } svalue_t;

typedef
  struct {
    svalue_t *(*func)(svalue_t, svalue_t*);
    svalue_t *fvars;
  } closure_t;

typedef
  struct {
    svalue_t head;
    struct cell *tail;
  }
  cell;

svalue_t *
box_value(svalue_variants_t, stype_t);

closure_t
make_closure(svalue_t* (*func)(svalue_t, svalue_t*),
                                     svalue_t*);

svalue_t*
invoke(closure_t, svalue_t);

#ifndef LIB
static svalue_t*
make_adder_inner(svalue_t, svalue_t *);

static closure_t
make_adder(svalue_t *);
#endif
