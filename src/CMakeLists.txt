include_directories(.)
include_directories(include)

SET(BIDI_SRC
    brinput.c
    brrule.c
    brtable.c
    brtest.c
    brutils.c
)

# add_executable( bidirefdemo
#     demo/bidirefdemo.c

#     ${BIDI_SRC}
# )

add_executable( bidiref
  bidiref.c

  ${BIDI_SRC}
)

# add_executable( bidiref1
#   bidiref1.c

#   ${BIDI_SRC}
# )
