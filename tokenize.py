#! /usr/bin/python2

from ctypes import *

tokenizer = cdll.LoadLibrary("./tokenize.so")

toktypes = { 0 : "symbol",
             1 : "identifier",
             2 : "integer",
             3 : "floating",
             4 : "quote",
             5 : "whitespace",
             6 : "parenthesis"}

class TokenValT(Union):
    _fields_ = [("symbol", c_char_p),
                ("identifier", c_char_p),
                ("integer", c_char_p),
                ("floating", c_char_p),
                ("parenthesis", c_char_p),
                ("quote", c_bool),
                ("whitespace", c_bool),
                ("null_token", c_bool)]

class TokenT(Structure):
    _fields_ = [("token_type", c_int),
                ("token", TokenValT)]

class TokStream(Structure):
    _fields_ = [("length", c_size_t),
                ("max_length", c_size_t),
                ("tokens", POINTER(TokenT)),
                ("memo", c_void_p)]

tokenizer.tokenize.restype = TokStream
tokenizer.peek_token.restype = TokenT
tokenizer.pop_token.restype = c_bool
tokenizer.release_tokens.restype = c_bool

def tokenize(source):
    tokens = tokenizer.tokenize(source, 0, len(source))
    tp = pointer(tokens)
    while tokens.length > 0:
        tok = tokenizer.peek_token(tp)
        ttype = toktypes[tok.token_type]
        yield (ttype, getattr(tok.token, ttype))
        tokenizer.pop_token(tp)
    tokenizer.release_tokens(tp)

line = " '34 34"
xs = list(tokenize(line))
print xs
