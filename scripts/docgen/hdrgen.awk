#!/usr/bin/awk -f

# we are leaving a struct
in_struct && $0 == "};" {
  in_struct = 0
}

# if we are in a struct
in_struct {
  # skip comments
  if ($1 ~ /\*|\/|\\/)
    next

  # read in struct field
  space = ""
  def = ""
  for (i = 1; i <= NF; i++) {
    def = def space $i
    space = " "
    if ($i ~ /;$/)
      break
  }
  fields = fields def "\n"
}

# we have an include
/^#include/ {
  gsub(/<|>/, "", $2)
  includes = includes $2 "\n"
}

# we have a function
/^[a-zA-Z_][a-zA-Z_0-9]* ( |([a-z_A-Z][a-zA-Z0-9_]*|\*))*[a-z_A-Z][a-z_A-Z0-9]*\(/ {
  funcs = funcs $0 "\n"
}

# we have a macro
/^#define/ {
  # do not include header names
  if ($NF !~ /H$/) {
    $1 = ""
    if ($NF ~ /\\/)
      $NF = ""
    macros = macros $0 "\n"
  }
}

# we have a struct
/^struct/ {
  in_struct = 1
}

END {
  # print header includes
  if (length(includes) > 0) {
    printf("header includes\n")
    printf("---------------\n")
    printf("%s\n", includes)
  }

  # print public functions
  if (length(funcs) > 0) {
    printf("public functions\n")
    printf("----------------\n")
    printf("%s\n", funcs)
  }

  # print macros
  if (length(macros) > 0) {
    printf("public macros\n")
    printf("-------------\n")
    printf("%s\n", macros)
  }

  if (length(fields) > 0) {
    # print fields
    printf("fields\n")
    printf("------\n")
    printf("%s", fields)
  }
}
