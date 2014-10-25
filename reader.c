#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "error.h"
#include "maa.h"
#include "tokenize.h"
#include "RTS.h"

svalue_t*
read_scm(source_t);

svalue_t*
read_scm(source_t source_code) {
  size_t nbytes = read(STDIN_FILENO, source_code, 111000);
  if (nbytes == 0) {
    exit(EXIT_FAILURE);
  }

  token_stream toks = tokenize(source_code, 0, nbytes);
  token_t current_tok;
  while (toks.length > 0) {
    current_tok = peek_token(&toks);
    switch (current_tok.token_type) {
      case SYMBOL:
        printf("symbol: %s\n", current_tok.token.symbol);
        break;
      case IDENTIFIER:
        printf("identifer: %s\n", current_tok.token.identifier);
        break;
      case INTEGER:
        printf("integer: %s\n", current_tok.token.integer);
        break;
      case FLOATING:
        printf("floating: %s\n", current_tok.token.floating);
        break;
      case QUOTE:
        printf("quote: '\n");
        break;
      case WSPACE:
        printf("whitespace\n");
        break;
      case PAREN:
        printf("paren: %s\n", current_tok.token.parenthesis);
        break;
      case EMPTY:
        printf("this should not be empty\n");
        break;
      case STRING:
        printf("string: %s\n", current_tok.token.string);
        break;
      default:
        printf("oops, there was an unknown token, check valgrind or gdb\n");
    }
    pop_token(&toks);
  }
  return box_int(12);
}

int main(void) {
  return 0;
}
