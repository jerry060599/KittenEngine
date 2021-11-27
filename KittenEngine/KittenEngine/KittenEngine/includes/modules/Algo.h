#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Eigen/Eigen>

#include "Common.h"

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
double adpInt(std::function<T(double)> f, const double a, const double b, double tol = 0.001) {
	tol = sqrt(tol / (b - a));
	const double minIntSize = 1 / double(1 << minLevel);

	double samples[maxLevel];
	double sampleL = f(a);
	samples[minLevel] = f(mix(a, b, minIntSize));

	double ends[maxLevel];
	ends[minLevel - 1] = ends[minLevel] = minIntSize;

	T v = 0;
	int level = minLevel;
	int k = 0;

	while (true) {
		const double bt = ends[level];
		const double at = bt - 1 / double(1 << level);

		// Sample midpoint
		const double sampleM = f(mix(a, b, 0.5 * (at + bt)));
		const double vh = 0.5 * (sampleL + samples[level]);
		const double vhh = 0.25 * (sampleL + 2 * sampleM + samples[level]);
		const double err = (vhh - vh) / 3;

		//printf("(%f %f %d) e:%f (%f %f)\n", at, bt, level, err, sampleL, samples[level]);

		if (abs(err) > tol && level + 1 < maxLevel) {
			// Local error has exceeded tolerance
			level++;
			samples[level] = sampleM;
			ends[level] = 0.5 * (at + bt);
		}
		else {
			// Local error is good. We tack it on
			v += (vhh + err) / double(1 << level);

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

	return (b - a) * v;
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
double adpInt(std::function<T(double)> f, const double a, const double b, double tol = 0.001) {
	// We allow anywhere from 2^3=8 intervals to 2^13=8192 intervals.
	// It'll adaptively decide based on the error tolerance
	return adpInt<T, 3, 13>(f, a, b, tol);
}

/// <summary>
/// Finds the global min of a uni modal function
/// </summary>
/// <param name="f"></param>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="tol"></param>
/// <returns></returns>
inline double goldenIntMinSearch(std::function<double(double)> f, double a, double b, double tol = 0.001) {
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

	return (yc < yd) ? (a + d) * 0.5 : (c + b) * 0.5;
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
inline Eigen::VectorXf lbfgsMin(int numVars, std::function<float(Eigen::VectorXf)> f,
	std::function<Eigen::VectorXf(Eigen::VectorXf)> g,
	Eigen::VectorXf& guess, const float tol = 1e-6, const int m = 6) {
	using namespace Eigen;

	MatrixXf s(numVars, m);
	MatrixXf y(numVars, m);
	VectorXf rho(m);
	VectorXf a(m);

	int buffSize = 0;
	int buffInd = 0;

	VectorXf lastX = guess;
	VectorXf lastGv = g(guess);
	VectorXf x = guess - 0.001f * lastGv;
	float fv = f(x);
	VectorXf gv;
	VectorXf z;
	int itr = 0;
	while (true) {
		itr++;

		const int ind = buffInd % m;

		// Find descent direction with lbfgs

		gv = g(x);
		float k = (gv - lastGv).dot(x - lastX);
		if (k <= 0) {
			x -= 0.001f * gv;
			if (gv.norm() < tol) break;
			continue;
		}

		s.col(ind) = x - lastX;
		y.col(ind) = gv - lastGv;
		rho[ind] = 1 / k;

		lastX = x;
		lastGv = gv;
		z = gv;
		buffSize = std::min(m, buffSize + 1);

		for (int i = 0; i < buffSize; i++) {
			const int oi = (buffInd + m - i) % m;
			a[oi] = rho[oi] * z.dot(s.col(oi));
			z -= a[oi] * y.col(oi);
		}

		float Y = y.col(ind).dot(y.col(ind));
		if (Y != 0) {
			Y = s.col(ind).dot(y.col(ind)) / Y;
			z *= Y;
		}
		for (int i = 0; i < buffSize; i++) {
			const int oi = (buffInd + m + i - buffSize + 1) % m;
			z += s.col(oi) * (a[oi] - rho[oi] * y.col(oi).dot(z));
		}

		z *= -1;
		// Perform backtracking line search
		float t = 0.5f * gv.dot(z);
		float a = 1.f;
		float v = f(x);
		while (f(x + a * z) > v + a * t)
			a *= 0.5f;

		// Update guess
		x += a * z;
		float newFv = f(x);
		float diff = newFv - fv;
		fv = newFv;

		buffInd++;

		if (gv.norm() < tol && abs(diff) < tol) break;
	}
	// printf("ITR: %d\n", itr);
	return x;
}