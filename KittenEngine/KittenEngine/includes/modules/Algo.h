#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Eigen/Eigen>
#include <Eigen/Sparse>

#include "Common.h"

namespace Kitten {
	/// <summary>
	/// A constant-memory trapezoidal adaptive romberg integrator. 
	/// Jerry Hsu 2021
	/// </summary>
	/// <param name="f">The integrand</param>
	/// <param name="a">The left hand bound</param>
	/// <param name="b">The right hand bound</param>
	/// <param name="tol">The error tolerance</param>
	/// <returns>The value of the integral down to the given tolerance</returns>
	template<typename T, int minLevel, int maxLevel>
	T adpInt(std::function<T(double)> f, const double a, const double b, const double tol = 0.001) {
		// 0 - real, 1 - Eigen, 2 - glm.
		constexpr int type = (std::is_same<T, float>::value || std::is_same<T, double>::value) ? 0 :
			((std::is_same <T, Eigen::VectorXf>::value || std::is_same <T, Eigen::VectorXd>::value) ? 1 : 2);

		const double sqrTolPerLen = pow2(tol / (b - a));
		const double minIntSize = 1 / double(1 << minLevel);

		T samples[maxLevel];
		T sampleL = f(a);
		samples[minLevel] = f(mix(a, b, minIntSize));

		double ends[maxLevel];
		ends[minLevel - 1] = ends[minLevel] = minIntSize;

		T v[maxLevel];
		if constexpr (type == 0) v[minLevel] = 0;
		else if constexpr (type == 1) v[minLevel] = T::Zero(samples[minLevel].size());
		else v[minLevel] = T(0);
		for (int i = minLevel + 1; i < maxLevel; i++)
			v[i] = v[minLevel];

		int level = minLevel;
		int k = 0;

		while (true) {
			const double bt = ends[level];
			const double at = bt - 1 / double(1 << level);

			// Sample midpoint
			const T sampleM = f(mix(a, b, 0.5 * (at + bt)));
			const T vh = 0.5 * (sampleL + samples[level]);
			const T vhh = 0.25 * (sampleL + 2. * sampleM + samples[level]);
			const T err = (vhh - vh) * (1. / 3.);

			//printf("(%f %f %d) e:%f (%f %f)\n", at, bt, level, err, sampleL, samples[level]);

			double errNormSqr;
			if constexpr (type == 0) errNormSqr = err * err;
			else if constexpr (type == 1) errNormSqr = err.squaredNorm();
			else errNormSqr = dot(err, err);

			if (errNormSqr > sqrTolPerLen && level + 1 < maxLevel) {
				// Local error has exceeded tolerance
				level++;
				samples[level] = sampleM;
				ends[level] = 0.5 * (at + bt);
			}
			else {
				// Local error is good. We tack it on
				v[level] += vhh + err;

				while (ends[level - 1] == ends[level] && level > minLevel)
					level--;
				ends[level] = ends[level - 1];
				sampleL = samples[level];

				if (level <= minLevel) {
					if (++k == (1 << minLevel)) break;
					ends[minLevel - 1] = ends[minLevel] = (k + 1) * minIntSize;
					samples[minLevel] = f(mix(a, b, ends[minLevel]));
					level = minLevel;
				}
				else samples[level] = samples[level - 1];
			}
		}

		v[maxLevel - 1] /= double(1 << (maxLevel - 1));
		for (int i = maxLevel - 2; i >= minLevel; i--)
			v[maxLevel - 1] += v[i] / double(1 << i);

		return (b - a) * v[maxLevel - 1];
	}

	/// <summary>
	/// A constant-memory trapezoidal adaptive romberg integrator. 
	/// Jerry Hsu 2021
	/// </summary>
	/// <param name="f">The integrand</param>
	/// <param name="a">The left hand bound</param>
	/// <param name="b">The right hand bound</param>
	/// <param name="tol">The error tolerance</param>
	/// <returns>The value of the integral down to the given tolerance</returns>
	template<typename T>
	T adpInt(std::function<T(double)> f, const double a, const double b, double tol = 0.001) {
		// We allow anywhere from 2^3=8 intervals to 2^13=8192 intervals.
		// It'll adaptively decide based on the error tolerance
		return adpInt<T, 3, 13>(f, a, b, tol);
	}

	template<typename T>
	T nDiff(std::function<double(T)> f, T x, const double h = 0.001) {
		if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
			return (T)((f(x - 2 * h) - 8 * f(x - h) + 8 * f(x + h) - f(x + 2 * h)) / (12 * h));
		else {
			T nx = x;
			T g;
			int s;
			if constexpr (std::is_same <T, Eigen::VectorXf>::value || std::is_same <T, Eigen::VectorXd>::value) {
				s = x.size();
				g = T::Zero(s);
			}
			else {
				s = x.length();
				g = T(0);
			}
			for (int i = 0; i < s; i++) {
				auto v = nx[i];

				nx[i] = v - 2 * h;
				g[i] += f(nx);
				nx[i] = v - h;
				g[i] -= 8 * f(nx);
				nx[i] = v + h;
				g[i] += 8 * f(nx);
				nx[i] = v + 2 * h;
				g[i] -= f(nx);

				nx[i] = v;
			}
			return g * (1 / (12 * h));
		}
	}

	/// <summary>
	/// Finds the global min of a uni modal function
	/// </summary>
	/// <param name="f"></param>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="tol"></param>
	/// <returns></returns>
	inline dvec2 goldenIntMinSearch(std::function<double(double)> f, double a, double b, double tol = 0.001) {
		const double invPhi = (sqrt(5) - 1) * 0.5;
		const double invPhi2 = pow2(invPhi);

		double h = b - a;

		int	lim = int(ceil(log(tol / h) / log(invPhi)));

		double c = a + invPhi2 * h;
		double d = a + invPhi * h;
		double yc = f(c);
		double yd = f(d);

		for (int k = 0; k < lim; k++)
			if (yc < yd) {
				b = d;
				d = c;
				yd = yc;
				h = invPhi * h;
				c = a + invPhi2 * h;
				yc = f(c);
			}
			else {
				a = c;
				c = d;
				yc = yd;
				h = invPhi * h;
				d = a + invPhi * h;
				yd = f(d);
			}

		return dvec2((yc < yd) ? (a + d) * 0.5 : (c + b) * 0.5, glm::min(yc, yd));
	}

	/// <summary>
	/// Finds a local min of an arbitrary function
	/// </summary>
	/// <param name="numVars"></param>
	/// <param name="f"></param>
	/// <param name="g"></param>
	/// <param name="guess"></param>
	/// <param name="tol"></param>
	/// <param name="m"></param>
	/// <returns></returns>
	Eigen::VectorXf lbfgsMin(int numVars, std::function<float(Eigen::VectorXf)> f,
		std::function<Eigen::VectorXf(Eigen::VectorXf)> g,
		Eigen::VectorXf& guess, const float tol = 1e-6, const int m = 6);

	/// <summary>
	/// Solves arg min(0.5 x^T A x - b^T x)
	/// </summary>
	/// <param name="A">the matrix in Ax = b</param>
	/// <param name="b">the vector in Ax = b</param>
	/// <param name="tol">tolerance</param>
	/// <param name="itrLim">iteration limit. -1 for infinity</param>
	/// <returns></returns>
	Eigen::VectorXd cg(
		Eigen::SparseMatrix<double, Eigen::RowMajor>& A,
		Eigen::VectorXd& b,
		const double tol = 1e-13,
		const int itrLim = -1
	);

	/// <summary>
	/// Solves arg min(0.5 x^T A x - b^T x) s.t. lower <= x
	/// An implementation of the Bound Constrained Conjugate Gradients method
	/// "The Bound-Constrained Conjugate Gradient Method for Non-negative Matrices"
	/// https://link.springer.com/article/10.1007/s10957-013-0499-x
	/// </summary>
	/// <param name="A">the matrix in Ax = b</param>
	/// <param name="b">the vector in Ax = b</param>
	/// <param name="lower">the lower bound for x</param>
	/// <param name="tol">tolerance</param>
	/// <param name="itrLim">iteration limit. -1 for infinity</param>
	/// <returns></returns>
	Eigen::VectorXd bccg(
		Eigen::SparseMatrix<double, Eigen::RowMajor>& A,
		Eigen::VectorXd& b,
		Eigen::VectorXd& lower,
		const double tol = 1e-13,
		const int itrLim = -1
	);


	/// <summary>
	/// Solves arg min(0.5 x^T A x - b^T x + 0.5 alpha(x - s)^T(x - s)) s.t. lower <= x
	/// An implementation of the Bound Constrained Conjugate Gradients method
	/// "The Bound-Constrained Conjugate Gradient Method for Non-negative Matrices"
	/// https://link.springer.com/article/10.1007/s10957-013-0499-x
	/// </summary>
	/// <param name="A">the matrix in Ax = b</param>
	/// <param name="b">the vector in Ax = b</param>
	/// <param name="lower">the lower bound for x</param>
	/// <param name="s">s</param>
	/// <param name="alpha">alpha</param>
	/// <param name="tol">tolerance</param>
	/// <param name="itrLim">iteration limit. -1 for infinity</param>
	/// <returns></returns>
	Eigen::VectorXd rbccg(
		Eigen::SparseMatrix<double, Eigen::RowMajor>& A,
		Eigen::VectorXd& b,
		Eigen::VectorXd& lower,
		Eigen::VectorXd& s,
		const double alpha,
		const double tol = 1e-13,
		const int itrLim = -1
	);

	/// <summary>
	/// Solves arg min(0.5 x^T A x - b^T x) s.t. lower <= x <= upper
	/// An implementation of the enhanced Bound Constrained Conjugate Gradients method
	/// "The Bound-Constrained Conjugate Gradient Method for Non-negative Matrices"
	/// https://link.springer.com/article/10.1007/s10957-013-0499-x
	/// </summary>
	/// <param name="A">the matrix in Ax = b</param>
	/// <param name="b">the vector in Ax = b</param>
	/// <param name="lower">the lower bound for x</param>
	/// <param name="upper">the upper bound for x</param>
	/// <param name="tol">tolerance</param>
	/// <param name="itrLim">iteration limit</param>
	/// <returns></returns>
	Eigen::VectorXd ebccg(
		Eigen::SparseMatrix<double>& A,
		Eigen::VectorXd& b,
		Eigen::VectorXd& lower,
		Eigen::VectorXd& upper,
		const double tol = 1e-13,
		const int itrLim = -1,
		const int k = 4
	);
}