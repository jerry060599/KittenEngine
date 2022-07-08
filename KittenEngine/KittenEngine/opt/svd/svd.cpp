
#include "../svd.h"

#define USE_SCALAR_IMPLEMENTATION
#define COMPUTE_V_AS_MATRIX
#define COMPUTE_U_AS_MATRIX
#include "Singular_Value_Decomposition_Preamble.hpp"

namespace Kitten {
	void svd(const mat3& m, mat3& u, vec3& s, mat3& v) {
#include "Singular_Value_Decomposition_Kernel_Declarations.hpp"
		ENABLE_SCALAR_IMPLEMENTATION(Sa11.f = m[0][0];) ENABLE_SCALAR_IMPLEMENTATION(Sa21.f = m[1][0];) ENABLE_SCALAR_IMPLEMENTATION(Sa31.f = m[2][0];)
		ENABLE_SCALAR_IMPLEMENTATION(Sa12.f = m[0][1];) ENABLE_SCALAR_IMPLEMENTATION(Sa22.f = m[1][1];) ENABLE_SCALAR_IMPLEMENTATION(Sa32.f = m[2][1];)
		ENABLE_SCALAR_IMPLEMENTATION(Sa13.f = m[0][2];) ENABLE_SCALAR_IMPLEMENTATION(Sa23.f = m[1][2];) ENABLE_SCALAR_IMPLEMENTATION(Sa33.f = m[2][2];)
#include "Singular_Value_Decomposition_Main_Kernel_Body.hpp"

		v[0][0] = Su11.f;  v[1][0] = Su12.f;  v[2][0] = Su13.f;
		v[0][1] = Su21.f;  v[1][1] = Su22.f;  v[2][1] = Su23.f;
		v[0][2] = Su31.f;  v[1][2] = Su32.f;  v[2][2] = Su33.f;

		s[0] = Sa11.f;
		s[1] = Sa22.f;
		s[2] = Sa33.f;

		u[0][0] = Sv11.f;  u[1][0] = Sv12.f;  u[2][0] = Sv13.f;
		u[0][1] = Sv21.f;  u[1][1] = Sv22.f;  u[2][1] = Sv23.f;
		u[0][2] = Sv31.f;  u[1][2] = Sv32.f;  u[2][2] = Sv33.f;
	}
}