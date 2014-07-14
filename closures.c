#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "RTS.h"

static svalue_t*
make_doubleadder_inner_inner(svalue_t *, svalue_t **);

static svalue_t*
make_doubleadder_inner(svalue_t *, svalue_t **);

static svalue_t*
make_doubleadder(svalue_t *, svalue_t **);

inline svalue_t
box_value(svalue_variants_t value,
          stype_t type) {

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
    case CLOSURE:
      val.value.closure = value.closure;
      /*printf("%p\n", val->value.closure->func);*/
      val.type_tag = type;
  }
  return val;
}

inline svalue_t *
box_int(int x) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.integer = x;
  *val = box_value(value_val, INT);
  return val;
}

inline svalue_t *
box_float(float x) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.floating = x;
  *val = box_value(value_val, FLOAT);
  return val;
}

inline svalue_t *
box_double(double x) {
  svalue_t *val = malloc(sizeof(svalue_t));
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

  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);

  svalue_variants_t value_val;
  value_val.string = strval;
  *val = box_value(value_val, STRING);
  return val;
}

inline svalue_t *
box_closure(closure_t *closure) {
  svalue_t *val = calloc(sizeof(svalue_t), 1);
  CHECK(val);
  svalue_variants_t value_val;
  value_val.closure = closure;
  *val = box_value(value_val, CLOSURE);
  return val;
}


closure_t*
make_closure(svalue_t *(*func)(svalue_t*, svalue_t**),
             svalue_t **fvars) {
  closure_t *closure = calloc(sizeof(closure_t), 1);
  closure->func = func;
  closure->fvars = fvars;
  return closure;
}

inline svalue_t*
invoke(svalue_t *closure, svalue_t *val) {
  printf("In invoke: %p\n", closure->value.closure->func);
  return closure->value.closure->func(val, closure->value.closure->fvars);
}


static svalue_t*
make_doubleadder_inner_inner(svalue_t *z, svalue_t **env) {
  int x,y;
  x = env[0]->value.integer;
  y = env[1]->value.integer;
  z->value.integer = x + y + (z->value.integer);
  return box_int(z->value.integer);
}

static svalue_t*
make_doubleadder_inner(svalue_t *y, svalue_t **env) {
  env[1] = y;
  closure_t *inner = make_closure(make_doubleadder_inner_inner, env);
  return box_closure(inner);
}

static svalue_t*
make_doubleadder(svalue_t *x, svalue_t **env) {
  env[0] = x;
  closure_t *closure = make_closure(make_doubleadder_inner, env);
  return box_closure(closure);
}

void printpointer(closure_t);
void printpointer(closure_t closure) {
  printf("%p\n", closure.func);
}

int
main(void) {
  (void)box_float;
  (void)box_double;
  (void)box_string;
  svalue_t **env = calloc(sizeof(svalue_t *), 2);
  closure_t *closure1_closure = make_closure(make_doubleadder, env);
  printf("First %p\n", closure1_closure->func);
  svalue_t *closure1 = box_closure(closure1_closure);
  printf("Second %p\n", closure1->value.closure->func);
  svalue_t *c1 = invoke(closure1, box_int(23));
  printf("Last %p\n", c1->value.closure->func);
  svalue_t *c2 = invoke(c1, box_int(5));
  printf("%d\n", c2->value.closure->fvars[1]->value.integer);
  svalue_t *result = invoke(c2, box_int(334));
  printf("%d\n", result->value.integer);
  return 0;
}
