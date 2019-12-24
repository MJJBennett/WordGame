#ifndef WG_ASSERT_HPP
#define WG_ASSERT_HPP

#if defined(WIN32) || defined(_WIN32)
#include <assert.h>
#endif

namespace wg
{
static void abort_if(bool assertion)
{
	assert(assertion);
#if defined(WIN32) || defined(_WIN32)
	if (!assertion)
	abort();
#endif
}
}

#if defined(WIN32) || defined(_WIN32)
#define WG_ABORT() abort();
#else
#define WG_ABORT() assert(false);
#endif

#endif // WG_ASSERT_HPP