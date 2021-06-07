#pragma once

#include "mn/Exports.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mn_simd_support
{
	bool sse_supportted;
	bool sse2_supportted;
	bool sse3_supportted;
	bool ssse3_supportted;
	bool sse4_1_supportted;
	bool sse4_2_supportted;
	bool sse4a_supportted;
	bool sse5_supportted;
	bool avx_supportted;
} mn_simd_support;

// returns the support status of various SIMD extensions
MN_EXPORT mn_simd_support
mn_simd_support_check();

#ifdef __cplusplus
}
#endif