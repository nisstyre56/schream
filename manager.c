#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "RTS.h"
#include "manager.h"

#define sc_size sizeof(svalue_t)

svalue_t * acquire(void *val,
                   svalue_t *(*box_val)(void *)) {
  svalue_t *new_val = box_val(val);
  CHECK(val);
  return new_val;
}

int main(void) {
  return 0;
}

/*
 * Outline of garbage collection algorithm
 * we mark things accessible from a certain environment
 * when things get removed from an environment we decrement their reference count
 * periodically we go through and unmark things?
 */

