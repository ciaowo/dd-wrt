/* Generated by ./xlat/gen.sh from ./xlat/name_to_handle_at_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(AT_SYMLINK_FOLLOW) || (defined(HAVE_DECL_AT_SYMLINK_FOLLOW) && HAVE_DECL_AT_SYMLINK_FOLLOW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((AT_SYMLINK_FOLLOW) == (0x400), "AT_SYMLINK_FOLLOW != 0x400");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define AT_SYMLINK_FOLLOW 0x400
#endif
#if defined(AT_EMPTY_PATH) || (defined(HAVE_DECL_AT_EMPTY_PATH) && HAVE_DECL_AT_EMPTY_PATH)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((AT_EMPTY_PATH) == (0x1000), "AT_EMPTY_PATH != 0x1000");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define AT_EMPTY_PATH 0x1000
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat name_to_handle_at_flags in mpers mode

# else

static const struct xlat_data name_to_handle_at_flags_xdata[] = {
 XLAT(AT_SYMLINK_FOLLOW),
 XLAT(AT_EMPTY_PATH),
};
static
const struct xlat name_to_handle_at_flags[1] = { {
 .data = name_to_handle_at_flags_xdata,
 .size = ARRAY_SIZE(name_to_handle_at_flags_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
