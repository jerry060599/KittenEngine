#pragma once
#include "Common.h"

namespace Kitten {
	// A more compact representation for symetric matrices
	template <int N, typename T>
	struct SymMat {
		typedef glm::mat<N, N, T, glm::defaultp> mat_type;
		typedef glm::vec<N, T, glm::defaultp> vec_type;
		static constexpr int DATA_LEN = (N * (N + 1)) / 2;

		// Always laid out as diagonal, then the upper triangle going column major
		union {
			T data[DATA_LEN];
			vec_type diag;		// The diagonal
		};

		// Constructors and conversions
		KITTEN_FUNC_DECL SymMat() {}

		constexpr KITTEN_FUNC_DECL SymMat(const T d) : data{} {
			for (int i = 0; i < N; i++)
				data[i] = d;
		}

		constexpr KITTEN_FUNC_DECL SymMat(const vec_type d) : data{} {
			for (int i = 0; i < N; i++)
				data[i] = d[i];
		}

		constexpr KITTEN_FUNC_DECL SymMat(const SymMat& other) : data{} {
			for (int i = 0; i < DATA_LEN; ++i)
				data[i] = other.data[i];
		}

		template <int M, typename U>
		constexpr KITTEN_FUNC_DECL explicit SymMat(const SymMat<M, U>& other) {
			constexpr int K = M < N ? M : N;
			// Copy diaganol
			for (int i = 0; i < K; i++)
				data[i] = other.data[i];
			// Copy upper triangle
			constexpr int KLim = (K * (K + 1)) / 2 - K;
			for (int i = 0; i < KLim; i++)
				data[N + i] = other.data[M + i];
			// Zero out the rest
			for (int i = K + KLim; i < DATA_LEN; i++)
				data[i] = T(0);
		}

		KITTEN_FUNC_DECL SymMat<N, T>& operator=(const SymMat<N, T>& other) {
			for (int i = 0; i < DATA_LEN; ++i)
				data[i] = other.data[i];
			return *this;
		}

		constexpr KITTEN_FUNC_DECL explicit operator mat_type() const {
			mat_type m;
			for (int i = 0; i < N; i++)
				m[i][i] = data[i];

			for (int i = 1; i < N; i++)
				for (int j = 0; j < i; j++) {
					T v = data[N + j + (i * (i - 1)) / 2];
					m[i][j] = v;
					m[j][i] = v;
				}
			return m;
		}

		constexpr KITTEN_FUNC_DECL const vec_type col(const int i) const {
			vec_type v;
			// Grab before diagonal
			for (int j = 0; j < i; j++)
				v[j] = data[j + N + (i * (i - 1)) / 2];
			// Grab diagonal
			v[i] = data[i];
			// Grab after diagonal
			for (int j = i + 1; j < N; j++)
				v[j] = data[i + N + (j * (j - 1)) / 2];

			return v;
		}

		// Linear algebra
		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator+(const T scalar) const {
			SymMat<N, T> result(*this);
			for (int i = 0; i < N; i++)
				result.data[i] = data[i] + scalar;
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator-(const T scalar) const {
			SymMat<N, T> result(*this);
			for (int i = 0; i < N; i++)
				result.data[i] = data[i] - scalar;
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator+(const SymMat<N, T>& other) const {
			SymMat<N, T> result;
			for (int i = 0; i < DATA_LEN; i++)
				result.data[i] = data[i] + other.data[i];
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator-(const SymMat<N, T>& other) const {
			SymMat<N, T> result;
			for (int i = 0; i < DATA_LEN; i++)
				result.data[i] = data[i] - other.data[i];
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator*(const T scalar) const {
			SymMat<N, T> result;
			for (int i = 0; i < DATA_LEN; i++)
				result.data[i] = data[i] * scalar;
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator/(const T scalar) const {
			T invScalar = 1 / scalar;
			SymMat<N, T> result;
			for (int i = 0; i < DATA_LEN; i++)
				result.data[i] = data[i] * invScalar;
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator*(const SymMat<N, T>& other) const {
			SymMat<N, T> result;
			for (int i = 0; i < N; i++)
				result[i] = dot(col(i), other.col(i));
			for (int i = 1; i < N; i++)
				for (int j = 0; j < i; j++)
					result[N + j + (i * (i - 1)) / 2] = dot(col(i), other.col(j));
			return result;
		}

		constexpr KITTEN_FUNC_DECL vec_type operator*(const vec_type& v) const {
			vec_type result = v[0] * col(0);
			for (int i = 1; i < N; i++)
				result += v[i] * col(i);
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T> operator-() const {
			SymMat<N, T> result;
			for (int i = 0; i < DATA_LEN; i++)
				result.data[i] = -data[i];
			return result;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator+=(const T scaler) {
			for (int i = 0; i < N; i++)
				data[i] += scaler;
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator-=(const T scaler) {
			for (int i = 0; i < N; i++)
				data[i] -= scaler;
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator+=(const SymMat<N, T>& other) {
			for (int i = 0; i < DATA_LEN; i++)
				data[i] += other.data[i];
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator-=(const SymMat<N, T>& other) {
			for (int i = 0; i < DATA_LEN; i++)
				data[i] -= other.data[i];
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator*=(const T scalar) {
			for (int i = 0; i < DATA_LEN; i++)
				data[i] *= scalar;
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator/=(const T scalar) {
			T invScalar = 1 / scalar;
			for (int i = 0; i < DATA_LEN; i++)
				data[i] *= invScalar;
			return *this;
		}

		constexpr KITTEN_FUNC_DECL SymMat<N, T>& operator*=(const SymMat<N, T>& other) {
			*this = *this * other;
			return *this;
		}

		constexpr KITTEN_FUNC_DECL T& operator[](const int i) {
			return data[i];
		}

		constexpr KITTEN_FUNC_DECL const T& operator[](const int i) const {
			return data[i];
		}

		constexpr KITTEN_FUNC_DECL static const SymMat<N, T> outer(const vec_type v) {
			SymMat<N, T> result(0);
			for (int i = 0; i < N; i++)
				result[i] = v[i] * v[i];
			for (int i = 1; i < N; i++)
				for (int j = 0; j < i; j++)
					result[N + j + (i * (i - 1)) / 2] = v[i] * v[j];
			return result;
		}
	};

	typedef SymMat<2, float> symmat2;
	typedef SymMat<3, float> symmat3;
	typedef SymMat<4, float> symmat4;

	typedef SymMat<2, double> symdmat2;
	typedef SymMat<3, double> symdmat3;
	typedef SymMat<4, double> symdmat4;
}