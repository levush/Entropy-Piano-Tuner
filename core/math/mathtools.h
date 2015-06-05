/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

//=============================================================================
//                        Various mathematical tools
//=============================================================================


#ifndef MATHTOOLS_H
#define MATHTOOLS_H

#include <vector>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include "../system/prerequisites.h"

namespace MathTools {

const double PI     = 3.14159265358979323846;
const double TWO_PI = 2 * PI;
const double LOG2   = 0.69314718055994530942;

/// Round a floating point number to an integer
template<class T>
int roundToInteger (T x) { return static_cast<int>(lround(x)); }

/// Determine the n-th moment of a distribution stored in a vector
double computeMoment (const std::vector<double> &v, const int n);

/// Normalize a distribution stored in a vector
void normalize (std::vector<double> &vec);

/// Compute the norm of a vector
double computeNorm (std::vector<double> &vec);

/// Map a distribution in the vector X[x] to another vector Y[y]
/// by means of a (possibly nonlinear) function x=f(y).
void coarseGrainSpectrum (const std::vector<double> &X,
                          std::vector<double> &Y,
                          std::function<double(double y)> f,
                          double exponent=0);

/// Compute the Shannon entropy of a normalized probability distribution
double computeEntropy (const std::vector<double> &v);

/// Compute the Renyi entropy of a normalized probability distribution
double computeRenyiEntropy (const std::vector<double> &v, const double q);

/// Find the component where the vector has its maximum
int findMaximum (const std::vector<double> &X, int i, int j);
int findMaximum (const std::vector<double> &X);

/// Use a parabola to fit the maximum.
double findSmoothedMaximum (const std::vector<double> &x);

/// Computes the weighted arithmetric mean index of the given Y data
double weightedArithmetricMean(const std::vector<double> &Y, size_t start = 0, size_t end = std::numeric_limits<size_t>::max());

/// Restrict floating point value to an interval
double restrictToInterval (double x, double xmin, double xmax);

/// Map a vector to a different one by a unary map
template <typename T>
void transformVector (const std::vector<T> &v, std::vector<T> &w,
                      std::function<T(T)> f)
{ w.resize(v.size()); for (int i=0; i<v.size(); i++) w[i]=f(v[i]); }

template <typename T>
std::vector<T> transformVector (const std::vector<T> &v, std::function<T(T)> f)
{ std::vector<T> w(v.size()); for (uint i=0; i<v.size(); i++) w[i]=f(v[i]); return w; }

} // MathTools

#endif // MATHTOOLS_H
