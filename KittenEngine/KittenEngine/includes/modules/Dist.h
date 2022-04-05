#pragma once
#include <math.h>
// Jerry Hsu 2022

namespace Kitten {
	/// <summary>
	/// Represents a distribution tracked through its mean and variance.
	/// </summary>
	struct Dist {
	public:
		double X = 0;
		double XX = 0;
		int num = 0;

		Dist(int n = 0) {
			X = XX = 0;
			num = n;
		}

		void accu(double x) {
			++num;
			X += x;
			XX += x * x;
		}

		void accu(Dist other) {
			num += other.num;
			X += other.X;
			XX += other.XX;
		}

		double mean() {
			return X / num;
		}

		double var() {
			if (num < 2)
				return 0;
			return (XX - X * X / num) / (num - 1);
		}

		double sd() {
			return sqrt(var());
		}

		static Dist zero() {
			return Dist(0);
		}
	};
}