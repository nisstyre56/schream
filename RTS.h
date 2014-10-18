typedef
  struct {
    size_t size;
    char *string;
  } sc_string_t;

typedef
  struct {
    struct svalue_t *left;
    struct svalue_t *right;
  }
  sc_pair_t;

/* This is not the most space efficient representation
 * However it is an easy to understand and debug one
 */
typedef
  union {
    int integer;
    float floating;
    double doublev;
    sc_string_t string;
    sc_pair_t pair;
    struct sc_closure_t *closure;
  } svalue_variants_t;

/* The tag values for each different type */
typedef
  enum {
    INT = 0,
    FLOAT = 1,
    DOUBLE = 2,
    STRING = 3,
    CLOSURE = 4,
    PAIR = 5
  } stype_t;

/* An actual boxed scheme value */
typedef
  struct svalue_t {
    stype_t type_tag;
    svalue_variants_t value;
  } svalue_t;

/* A closure is a function pointer taking a
 * single argument and an environment
 * What will happen is that applications of
 * procedures will be 'curried' but they will
 * always be required to be fully applied earlier
 * on in the compiler by checking applications
 * against the arity of a procedure (obtained by looking
 * at its definition). This simplifies compilation, but
 * does not change the semantics of procedures in any
 * way
 */
typedef
  struct sc_closure_t {
    svalue_t *(*func)(svalue_t**, svalue_t**);
    svalue_t **fvars;
  } sc_closure_t;

svalue_t
box_value(svalue_variants_t, stype_t);

svalue_t*
make_closure(svalue_t *(*func)(svalue_t**, svalue_t**),
                                     svalue_t**);

svalue_t *
invoke(svalue_t*, svalue_t**);


svalue_t *
box_int(int x);

svalue_t *
box_float(float x);

svalue_t *
box_double(double x);

svalue_t *
box_string(char *,
           size_t);

svalue_t *
box_closure(sc_closure_t*);

svalue_t *
box_pair(svalue_t*, svalue_t*);
