#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "RTS.h"

inline svalue_t *
box_value(svalue_variants_t value,
          stype_t type) {

  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  switch (type) {
    case INT:
      val->value = value;
      val->type_tag = type;
      break;
    case FLOAT:
      val->value = value;
      val->type_tag = type;
      break;
    case DOUBLE:
      val->value = value;
      val->type_tag = type;
    case STRING:
      val->value = value;
      val->type_tag = type;
    case CLOSURE:
      val->value = value;
      val->type_tag = type;
  }
  return val;
}

inline svalue_t *
box_int(int x) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.integer = x;
  val = box_value(value_val, INT);
  return val;
}

inline svalue_t *
box_float(float x) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.floating = x;
  val = box_value(value_val, FLOAT);
  return val;
}

inline svalue_t *
box_double(double x) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.doublev = x;
  val = box_value(value_val, DOUBLE);
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
  val = box_value(value_val, STRING);
  return val;
}

inline svalue_t *
box_closure(closure_t closure) {
  svalue_t *val = malloc(sizeof(svalue_t));
  CHECK(val);
  svalue_variants_t value_val;
  value_val.closure = (struct closure_t *)&closure;
  val = box_value(value_val, CLOSURE);
  return val;
}


inline closure_t
make_closure(svalue_t *(*func)(svalue_t, svalue_t*),
             svalue_t *fvars) {
  closure_t closure;
  closure.func = func;
  closure.fvars = fvars;
  return closure;
}

inline svalue_t*
invoke(closure_t closure, svalue_t val) {
  svalue_t *(*func)(svalue_t, svalue_t*) = closure.func;
  return func(val, closure.fvars);
}

#ifndef LIB
static svalue_t*
make_adder_inner(svalue_t x, svalue_t *env) {
  svalue_variants_t val;
  val.integer = env[0].value.integer + x.value.integer;
  return box_value(val, INT);
}

static closure_t
make_adder(svalue_t *inc) {
  closure_t closure = make_closure(make_adder_inner, inc);
  return closure;
}

int
main(void) {
  closure_t add2 = make_adder(box_int(2));
  printf("%d\n", invoke(add2, *box_int(5))->value.integer);
  (void)box_float;
  (void)box_double;
  (void)box_string;
  (void)box_closure;
  return 0;
}
#endif
