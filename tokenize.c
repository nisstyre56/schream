#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "maa.h"
#include "tokenize.h"

/*
 * This is a basic s-expression tokenizer
 * it also tokenizes things like number, string, and symbol literals
 */

static const token_t nulltok = {.token_type=EMPTY, {.null_token=false}};

static const token_t whitespace_tok = {.token_type=WSPACE, .token={.whitespace=true } };

static const token_t quote_tok = {.token_type=QUOTE, .token={.quote=true} };

static const token_t left_paren = {.token_type=PAREN, .token={.parenthesis="("} };

static const token_t right_paren = {.token_type=PAREN, .token={.parenthesis=")"} };

static inline char *
string_head(uint32_t n, char *in, char *out) {
  /* out must be large enough to store the number of characters
   * you want to select from in, plus a byte for the null terminator
   */
#ifndef NDEBUG
  size_t in_len = strlen(in);
#endif
  assert(n > 0 && n <= in_len);
  int iserror = snprintf(out, (size_t)n+1 , "%s", in);

  assert((iserror != -1) && (iserror == in_len));

  if (iserror == -1) {
    printf("Out of memory");
    exit(EXIT_FAILURE);
  }
  return out;
}

static inline token_t
make_token(token_val_t val, tok_t toktype) {
  token_t result;
  result.token_type = toktype;
  result.token = val;
  return result;
}

bool
push_token(token_stream *tokens, token_t token) {
  /*
   * Check if tokens points to NULL
   */

  size_t len;
  size_t max;

  assert(tokens != NULL);

  len = tokens->length;
  max = tokens->max_length;

  assert(len <= max);
  assert(max > 0);

  if (len == max) {
    /* We've reached the maximum stack size
     * So we must try to increase that by GROWTH_SIZE
     */
    token_t *new_tokens = realloc(tokens->tokens, sizeof(token_t) * (max + GROWTH_SIZE));
    if (!new_tokens) {
      printf("Could not allocate enough memory for the token stack\n");
      exit(EXIT_FAILURE);
    }
    tokens->tokens = new_tokens;
    tokens->max_length = max + GROWTH_SIZE;
    tokens->tokens[len] = token;
    tokens->length++;
    return true;
  }
  tokens->tokens[len] = token;
  tokens->length++;
  return true;
}

bool
pop_token(token_stream *tokens) {
  size_t len;
  assert(tokens != NULL);

  len = tokens->length;

  assert(len != 0);
  len--;
  assert(tokens->tokens != NULL);

  tokens->length--;
  return true;
}

inline token_t
peek_token(token_stream *tokens) {
  /*
   * Check if tokens points to NULL
   */
  size_t len = tokens->length;
  size_t max = tokens->max_length;
  assert(tokens != NULL);
  assert(len != 0);

  if (len == 0 || len > max) {
    return nulltok;
  }
  return tokens->tokens[len-1];
}

static inline uint32_t
match_int(source_t source, uint32_t begin, const uint32_t length) {
  /* Return false if there is no match
   * otherwise return the position of the end of the match + 1
   */
  uint32_t i = begin;
  uint32_t test;
  assert(source != NULL);
  assert(length > 0);

  if (source[i] == '+' ||
      source[i] == '-') {
    i++;
  }
  test = i;
  while (i < length &&
         isdigit(source[i])) {
    i++;
  }
  if (i == test)
    return false;
  return i;
}

static inline uint32_t
match_float(source_t source, uint32_t begin, const uint32_t length) {
  /* Return false if there is no match
   * otherwise:
   *  if there is a leading decimal point and then a valid int match:
   *    return the position of the end of the match
   *  if there is a leading valid int match:
   *    but no decimal point match after that:
   *      return false
   *    if there is a decimal point match and then a valid int match:
   *        return the position of the match
   *    if there is no valid int match:
   *      return false
   * ALWAYS returns the position + 1 to avoid confusion with false (which is a valid index)
   */
  uint32_t i, leading_int_match, trailing_int_match;
  assert(source != NULL);
  assert(length > 0);

  i = begin;
  leading_int_match = match_int(source, i, length);

  if (leading_int_match) {
    i = leading_int_match;
  }

  assert(i <= length);

  if (source[i] != '.' ||
      source[i] == '+' ||
      source[i] == '-') {
    if (((i+1) <= length) && /* Make sure there is at least two characters to look at */
        ((source[i] == '+') ||
         (source[i] == '-'))
        && (source[i+1] == '.')) {
      i++;
    }
    else {
      return false;
    }
  }
  i++;

  trailing_int_match = match_int(source, i, length);
  if (trailing_int_match) {
    return trailing_int_match;
  }
  return false;
}

static inline uint32_t
match_identifier(source_t source, uint32_t begin, const uint32_t length) {

  /* Return false if there is no match
   *    if there is a match for any characters that are not:
   *      whitespace
   *      a parenthesis ( )
   *      a brace { }
   *      a square bracket [ ]
   *        then return the position of the match + 1
   *    if there is nothing else to match:
   *      return false
   */
  uint32_t i = begin;
  assert(source != NULL);
  assert(length > 0);

  while (i < length &&
         !(source[i] == '(' ||
           source[i] == ')' ||
           isspace(source[i]))) {
    i++;
  }

  if (i == begin) {
    return false;
  }
  assert(i <= length);
  return i;
}

static inline uint32_t
match_symbol(source_t source, uint32_t begin, const uint32_t length) {
  uint32_t i, identifier_match;
  assert(source != NULL);
  assert(length > 0);

  i = begin;
  if (source[i] != '\'') {
    return false;
  }
  i++;

  identifier_match = match_identifier(source, i, length);
  if (identifier_match) {
    return identifier_match;
  }
  assert(identifier_match <= length);
  return false;
}

static inline void
extract_token(uint32_t position,
                   uint32_t begin,
                   source_t source,
                   char *token_val) {
    assert(position > begin);
    string_head(position - begin,
                &source[begin],
                token_val);
}

token_stream
tokenize(source_t source, uint32_t begin, const uint32_t length) {
  /*
   * Remember to free everything from this struct
   * for example, token_stack.tokens will not necessarily be
   * equal to tokens after this function has run
   *
   */
  uint32_t position = begin;
  char *current_token_val;
  token_stream token_stack;
  token_val_t current_token;
  token_t *tokens = calloc(STACK_SIZE, sizeof(token_t));

  hsh_HashTable token_memo = hsh_create(NULL, NULL);

  assert(begin == 0);
  assert(length > 0);
  assert(source != NULL);

  token_stack.length = 0;
  token_stack.max_length = STACK_SIZE;
  token_stack.tokens = tokens;
  token_stack.memo = token_memo;
  char lookahead = '\0';
  assert(STACK_SIZE > 0);

  while (begin <= length && source[begin]) {
   if (source[begin] == '(') {
      /*Matched a left paren */
      position = begin + 1;
      push_token(&token_stack, left_paren);
    }
    else if (source[begin] == ')') {
      /*Matched a left paren */
      position = begin + 1;
      push_token(&token_stack, right_paren);
    }
    else if (source[begin] == '\'') {
      /* Matched a quote (apostrophe) */
      position = begin + 1;
      push_token(&token_stack, quote_tok);
    }
    else if (isspace(source[begin])) {
      position = begin + 1;
      push_token(&token_stack, whitespace_tok);
      /* Matched a whitespace character */
    }
    else if ((position = match_float(source, begin, length))) {
      /* Matched a float */
      lookahead = source[position];
      source[position] = '\0';
      if ((current_token_val = (char *)hsh_retrieve(token_stack.memo, source+begin))) {
        current_token.floating = current_token_val;
        source[position] = lookahead;
      }
      else {
        source[position] = lookahead;
        assert(position > begin);
        current_token_val = calloc(((position - begin) + 1), sizeof(char));
        assert(current_token_val != NULL);
        extract_token(position, begin, source, current_token_val);
        hsh_insert(token_stack.memo, current_token_val, current_token_val);
        current_token.floating = current_token_val;
      }
      push_token(&token_stack, make_token(current_token, FLOATING));
    }
    else if ((position = match_int(source, begin, length))) {
      /* Matched an int */
      lookahead = source[position];
      source[position] = '\0';
      if ((current_token_val = hsh_retrieve(token_stack.memo, source+begin))) {
        current_token.integer = (char *)current_token_val;
        source[position] = lookahead;
      }
      else {
        assert(position > begin);
        assert(position <= length);

        source[position] = lookahead;
        current_token_val = calloc(((position - begin) + 1), sizeof(char));
        assert(current_token_val != NULL);
        extract_token(position, begin, source, current_token_val);
        hsh_insert(token_stack.memo, current_token_val, current_token_val);
        current_token.integer = current_token_val;
      }

      push_token(&token_stack, make_token(current_token, INTEGER));
    }
    else if ((position = match_symbol(source, begin, length))) {
      /* Matched a symbol */
      lookahead = source[position];
      source[position] = '\0';
      if ((current_token_val = hsh_retrieve(token_stack.memo, source+begin))) {
        current_token.symbol = (char *)current_token_val;
        source[position] = lookahead;
      }
      else {
        assert(position > begin);
        assert(position <= length);

        source[position] = lookahead;
        current_token_val = calloc(((position - begin) + 1), sizeof(char));
        assert(current_token_val != NULL);
        extract_token(position, begin, source, current_token_val);
        hsh_insert(token_stack.memo, current_token_val, current_token_val);
        current_token.symbol = current_token_val;
      }
      push_token(&token_stack, make_token(current_token, SYMBOL));
    }
    else if ((position = match_identifier(source, begin, length))) {
      /* Matched an identifier */
      lookahead = source[position];
      source[position] = '\0';
      if ((current_token_val = hsh_retrieve(token_stack.memo, source+begin))) {
        current_token.identifier = (char *)current_token_val;
        source[position] = lookahead;
      }
      else {

        assert(position > begin);
        assert(position <= length);

        source[position] = lookahead;
        current_token_val = calloc(((position - begin) + 1), sizeof(char));
        assert(current_token_val != NULL);
        extract_token(position, begin, source, current_token_val);
        hsh_insert(token_stack.memo, current_token_val, current_token_val);
        current_token.identifier = current_token_val;
      }

      push_token(&token_stack, make_token(current_token, IDENTIFIER));
      /* Matched an identifier */
    }
    else {
      printf("Unmatched token\n");
      exit(EXIT_FAILURE);
    }
    begin = position;
  }

  return token_stack;
}

int free_token(const void *key, const void *val) {
  /* silence warnings about unused parameters, key and val point to the same data*/
  (void)key;
  free((char *)val);
  return true;
}

bool
release_tokens(token_stream *tokens) {
  /* Iterate through the stack, release each token
   * Then release the entire stack
   */
  assert(tokens != NULL);
  assert(tokens->tokens != NULL);
  assert(tokens->max_length > 0);
  free(tokens->tokens);
  hsh_iterate(tokens->memo, free_token);

  hsh_destroy(tokens->memo);
  return true;
}
