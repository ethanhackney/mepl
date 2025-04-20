#!/usr/bin/awk -f

# we have an include
/^#include/ {
  gsub(/<|>/, "", $2)
  includes = includes $2 "\n"
}

# we have a function
/^static( |([a-z_A-Z][a-zA-Z0-9_]*|\*))*[a-z_A-Z][a-z_A-Z0-9]*\(/ {
  funcs = funcs $0 "\n"
}

# we have a macro
/^#define/ {
  macros = macros $2 "\n"
}

END {
  # print source includes
  if (length(includes) > 0) {
    printf("source includes\n")
    printf("---------------\n")
    printf("%s\n", includes)
  }

  # print private functions
  if (length(funcs) > 0) {
    printf("private functions\n")
    printf("-----------------\n")
    printf("%s\n", funcs)
  }

  if (length(macros) > 0) {
    # print macros
    printf("private macros\n")
    printf("--------------\n")
    printf("%s", macros)
  }
}
