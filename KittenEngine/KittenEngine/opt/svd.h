#pragma once

#include "../includes/modules/Common.h"

namespace Kitten {
	void svd(const mat3& m, mat3& u, vec3& s, mat3& v);
}