/* Generated by ./xlat/gen.sh from ./xlat/usagewho.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(RUSAGE_SELF) || (defined(HAVE_DECL_RUSAGE_SELF) && HAVE_DECL_RUSAGE_SELF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RUSAGE_SELF) == (0), "RUSAGE_SELF != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RUSAGE_SELF 0
#endif
#if defined(RUSAGE_CHILDREN) || (defined(HAVE_DECL_RUSAGE_CHILDREN) && HAVE_DECL_RUSAGE_CHILDREN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RUSAGE_CHILDREN) == ((-1)), "RUSAGE_CHILDREN != (-1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RUSAGE_CHILDREN (-1)
#endif
#if defined(RUSAGE_BOTH) || (defined(HAVE_DECL_RUSAGE_BOTH) && HAVE_DECL_RUSAGE_BOTH)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RUSAGE_BOTH) == ((-2)), "RUSAGE_BOTH != (-2)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RUSAGE_BOTH (-2)
#endif
#if defined(RUSAGE_THREAD) || (defined(HAVE_DECL_RUSAGE_THREAD) && HAVE_DECL_RUSAGE_THREAD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RUSAGE_THREAD) == (1), "RUSAGE_THREAD != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RUSAGE_THREAD 1
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat usagewho in mpers mode

# else

static const struct xlat_data usagewho_xdata[] = {
 XLAT(RUSAGE_SELF),
 XLAT(RUSAGE_CHILDREN),
 XLAT(RUSAGE_BOTH),
 XLAT(RUSAGE_THREAD),
};
static
const struct xlat usagewho[1] = { {
 .data = usagewho_xdata,
 .size = ARRAY_SIZE(usagewho_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
