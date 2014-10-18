#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "RTS.h"

/* Test case stuff */
#ifndef LIB
static svalue_t*
make_doubleadder_inner_inner(svalue_t **, svalue_t **);

static svalue_t*
make_doubleadder_inner(svalue_t **, svalue_t **);

static svalue_t*
make_doubleadder(svalue_t **, svalue_t **);
#endif

inline svalue_t
box_value(svalue_variants_t value,
          stype_t type) {
  /*
   * Creates a boxed value which is just
   * a tagged union where the value is the unboxed
   * value and the tag is an enum value describing
   * what the unboxed value represents
   * We do this so that all values are of the same "type"
   * and this makes it a lot simpler to pass around parameters,
   * environments, closures, etc...
   */

  svalue_t val;
  switch (type) {
    case INT:
      val.value.integer = value.integer;
      val.type_tag = type;
      break;
    case FLOAT:
      val.value.floating = value.floating;
      val.type_tag = type;
      break;
    case DOUBLE:
      val.value.doublev = value.doublev;
      val.type_tag = type;
    case STRING:
      val.value.string = value.string;
      val.type_tag = type;
    case PAIR:
      val.value.pair = value.pair;
      val.type_tag = type;
    case CLOSURE:
      val.value.closure = value.closure;
      val.type_tag = type;
  }
  return val;
}

inline svalue_t *
box_int(int x) {
  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.integer = x;
  *val = box_value(value_val, INT);
  return val;
}

inline svalue_t *
box_float(float x) {
  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.floating = x;
  *val = box_value(value_val, FLOAT);
  return val;
}

inline svalue_t *
box_double(double x) {
  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.doublev = x;
  *val = box_value(value_val, DOUBLE);
  return val;
}

inline svalue_t *
box_string(char *chars, size_t n) {
  sc_string_t strval;
  strval.string = chars;
  strval.size = n;

  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);

  svalue_variants_t value_val;
  value_val.string = strval;
  *val = box_value(value_val, STRING);
  return val;
}

inline svalue_t *
box_closure(sc_closure_t *closure) {
  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.closure = closure;
  *val = box_value(value_val, CLOSURE);
  return val;
}

inline svalue_t *
box_pair(svalue_t *left, svalue_t *right) {
  sc_pair_t pair;
  pair.left = left;
  pair.right = right;

  svalue_t *val = malloc(sizeof (svalue_t));
  CHECK(val);

  svalue_variants_t value_val;
  value_val.pair = pair;
  *val = box_value(value_val, PAIR);
  return val;
}

inline svalue_t*
make_closure(svalue_t *(*func)(svalue_t**, svalue_t**),
             svalue_t **fvars) {
  /* The reason we dynamically allocate here is because
   * svalue_variants_t will hold a pointer to the closure
   * and we cannot store a pointer to a stack allocated
   * closure or else it is undefined behavior when it is invoked
   * since it would get deallocated when this function returns
   */
  sc_closure_t *closure = malloc(sizeof (sc_closure_t));
  closure->func = func;
  closure->fvars = fvars;
  return box_closure(closure);
}

inline svalue_t*
invoke(svalue_t *closure, svalue_t **arguments) {
  return closure->value.closure->func(arguments, closure->value.closure->fvars);
}

/* Special case where there is only on argument
 * this might end up only being used for testing
 * since the code generator will construct singleton
 * arrays most likely, anyway
 */
inline svalue_t*
invoke1(svalue_t *closure, svalue_t *arg) {
  svalue_t **args = malloc(sizeof (svalue_t *));
  CHECK(args);
  args[0] = arg;
  svalue_t* result = invoke(closure, args);
  free(args);
  return result;
}

/*
 * The process for closure conversion basically involves finding all of the free variables
 * This will give the number of variables the environment must hold in total
 * Hence we can figure out how much memory to allocate for them!
 * Then the process of creating a closure simply involves assigning the bound variables to the environment
 * before returning the closure (created with make_closure)
 * Problem: how do we handle escaping functions? C can't do this afaik.
 */

/* More testing stuff */
#ifndef LIB
static inline svalue_t*
make_doubleadder_inner_inner(svalue_t **z, svalue_t **env) {
  int x,y;
  x = env[0]->value.integer;
  y = env[1]->value.integer;
  z[0]->value.integer = x + y + (z[0]->value.integer);
  return box_int(z[0]->value.integer);
}

static inline svalue_t*
make_doubleadder_inner(svalue_t **y, svalue_t **env) {
  env[1] = *y;
  return make_closure(make_doubleadder_inner_inner, env);
}

static svalue_t*
make_doubleadder(svalue_t **x, svalue_t **env) {
  env[0] = *x;
  return make_closure(make_doubleadder_inner, env);
}

int
main(void) {
  (void)box_float;
  (void)box_double;
  (void)box_string;
  /*Allocate an environment
   * The environment size depends on how many nested functions there are ?
   */
  svalue_t **env = calloc(sizeof (svalue_t *), 2);
  /* Get the final closure */
  svalue_t *closure1 = make_closure(make_doubleadder, env);
  /* Invoke the closure that the closure returns */
  svalue_t *c1 = invoke1(closure1, box_int(23));
  svalue_t *c2 = invoke1(c1, box_int(5));
  svalue_t *result = invoke1(c2, box_int(334));
  /* The final result */
  printf("print 23 + 5 + 334 == %d\n", result->value.integer);
  svalue_t *a = box_int(123);
  svalue_t *b = box_int(455);
  svalue_t *improper = box_pair(a, b);
  improper->value.pair.right = improper;
  /* woo cyclic pair */
  printf("(%d, %d)\n", improper->value.pair.left->value.integer, improper->value.pair.right->value.pair.left->value.integer);
  return 0;
}
#endif
