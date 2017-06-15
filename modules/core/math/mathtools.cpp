/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
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
//                       Various mathematical tools
//=============================================================================

#include "mathtools.h"

#include <algorithm>
#include <numeric>

#include "../system/eptexception.h"

//-----------------------------------------------------------------------------
// constants definition
//-----------------------------------------------------------------------------

const double MathTools::PI     = 3.14159265358979323846;
const double MathTools::TWO_PI = 2 * MathTools::PI;
const double MathTools::LOG2   = 0.69314718055994530942;

//-----------------------------------------------------------------------------
//	              Determine the raw moments of a distribution
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// The moments are determined with respect to the relative array
/// index between 0 and 1
///////////////////////////////////////////////////////////////////////////////

double MathTools::computeMoment (const std::vector<double> &v, const int n)
{
    EptAssert (n>=0 and v.size()>0, "Parameters inconsistent");
    double norm=0,sum=0;
    for (size_t i=0; i<v.size(); ++i)
    {
        norm += v[i];
        sum  += v[i] * pow(1.0*i,1.0*n);
    }
    EptAssert (norm>0, "The norm should be positive");
    return sum / norm;
}


//-----------------------------------------------------------------------------
//	                       Compute Shannon entropy
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// The Shannon entropy is defined as H = - sum ( v_i log v_i ).
/// Although log 0 is divergent, it is assumed that 0 log 0 = 0.
///
/// \param v : Normalized vector of non-negative real values
/// \return Shannon entropy
///////////////////////////////////////////////////////////////////////////////

double MathTools::computeEntropy (const std::vector<double> &v)
{
    EptAssert(v.size()>0,"The entropy of a vector with zero length is meaningless.");
    double sum=0;
    for (auto x : v) sum -= (x>0 ? x*log(x) : 0);
    return sum;
}


//-----------------------------------------------------------------------------
//	                          Compute Renyi entropy
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// The Renyi entropy is a certain kind of deformed entropy (see e.g. Wikipedia).
/// The deformation is controlled by an additional parameter q. In the limit
/// q->1 the Renyi entropy converges towards the usual Shannon entropy. The
/// Renyi entropy is interesting in so far as it probes different properties
/// of a probability distribution. It is, however, no longer directly related
/// to the information content of the distribution.
/// \param v : Vector or normalized non-negative real values.
/// \param q : Renyi deformation parameter (q>0)
/// \return : Renyi entropy
///////////////////////////////////////////////////////////////////////////////

double MathTools::computeRenyiEntropy (const std::vector<double> &v, const double q)
{
    EptAssert(v.size()>0,"The entropy of a vector with zero length is meaningless.");
    EptAssert(q>0,"The Renyi deformation parameter should be positive.");
    if (q==1) return computeEntropy (v);
    double sum=0;
    for (auto x : v) sum += pow(x,q);
    return log(sum) / (1-q);
}


//-----------------------------------------------------------------------------
//	                           Compute the norm
//-----------------------------------------------------------------------------

double MathTools::computeNorm (std::vector<double> &vec)
{
    return std::accumulate(vec.begin(), vec.end(), 0.0);
}


//-----------------------------------------------------------------------------
//	                        Normalize a distribution
//-----------------------------------------------------------------------------

void MathTools::normalize (std::vector<double> &vec)
{
    double norm = computeNorm(vec);
    EptAssert (norm!=0,"Vectors with norm zero cannot be normalized");
    for (auto &x : vec) x /= norm;
}


//-----------------------------------------------------------------------------
//	       Map a probability distribution from one vector to another
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// It is assumed that the vector Y[y] has already a given size.
/// If the function f maps the index range of Y to a superset of the
/// index range of X, then the map preserves the sum over all components.
/// Therefore, the map is suitable for mapping probability distributions.
/// If the function f maps indices in the range of Y to a subset of the
/// index range of X, i.e., if it does not read all of X, the
/// corresponding probability is lost.
///
/// \param X : Given vector of floating point values (probabilities)
/// \param Y : Target vector of a given size.
/// \param f : Function used for coarse-graining
/// \param exponent : exponent used for the mapping
///////////////////////////////////////////////////////////////////////////////


void MathTools::coarseGrainSpectrum (const std::vector<double> &X,
                                     std::vector<double> &Y,
                                     std::function<double(double y)> f,
                                     double exponent)
{
    assert(X.size()>0 and Y.size()>0);
    double xs1 = f(-0.5);
    int x1 = std::max<int>(0, roundToInteger(xs1));
    double leftarea = (x1-xs1+0.5)*X[x1];
    for (int y=0; y<static_cast<int>(Y.size()); ++y)
    {
        double xs2 = f(y+0.5);
        int x2 = std::min<int>(roundToInteger(xs2), static_cast<int>(X.size()) - 1);
        double sum=0;
        for (int x=x1+1; x<=x2; ++x) sum += X[x];
        double rightarea = (x2-xs2+0.5)*X[x2];
        Y[y] = (sum + leftarea - rightarea) * pow(xs1*xs2,exponent);
        x1=x2; xs1=xs2; leftarea=rightarea;
    }
}

//-----------------------------------------------------------------------------
//	                 Find the maximum in a vector
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Find the index of the maximum in a real-valued vector
/// in the index range from i...j-1.
/// \return Index where the maximum is located.
///////////////////////////////////////////////////////////////////////////////

int MathTools::findMaximum (const std::vector<double> &X, int i, int j)
{

    int N = static_cast<int>(X.size());
    assert (i>=0 and i<N and j>i and j<=N);
    return static_cast<int>(std::distance(X.begin(), std::max_element(X.begin()+i,X.begin()+j)));
}

int MathTools::findMaximum (const std::vector<double> &X)
{
    return static_cast<int>(std::distance(X.begin(), std::max_element(X.begin(),X.end())));
}

double MathTools::findSmoothedMaximum (const std::vector<double> &x)
{
    auto maxElem = std::max_element(std::next(x.begin()), std::prev(x.end()));
    int maxIndex = static_cast<int>(std::distance(x.begin(), maxElem));
    // interpolate with neighbours using a parabolas maximum
    double y1 = *std::prev(maxElem);
    double y2 = *maxElem;
    double y3 = *std::next(maxElem);
    double x1 = maxIndex - 1, x2 = maxIndex, x3 = maxIndex + 1;

    // compute max of parabola
    return (x3 * x3 * (y2 - y1) + x2 * x2 * (y1 - y3) + x1 * x1 * (y3 - y2)) / 2.0 / (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2));
}

double MathTools::weightedArithmetricMean(const std::vector<double> &Y, size_t start, size_t end)
{
    double norm = 0;
    double mean = 0;
    end = std::min(end, Y.size());
    for (size_t i = start; i < end; ++i)
    {
        norm += Y[i] * Y[i];
        mean += Y[i] * Y[i] * i;
    }

    return mean / norm;
}

//-----------------------------------------------------------------------------
//	          Restrict floating point value to an interval
//-----------------------------------------------------------------------------

double MathTools::restrictToInterval(double x, double xmin, double xmax)
{
    EptAssert(xmax > xmin, "xmax should be larger than xmin");
    return std::max(xmin, std::min(xmax, x));
}
