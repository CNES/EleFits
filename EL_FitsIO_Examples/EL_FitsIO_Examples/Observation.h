/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _EL_FITSIO_EXAMPLES_OBSERVATION_H
#define _EL_FITSIO_EXAMPLES_OBSERVATION_H

#include<complex>

#include "EL_FitsData/Raster.h"

namespace Euclid {
namespace FitsIO {
namespace Example {

class Gaussian {

public:

  struct Parameters {
    double amp;
    double x0, y0;
    double sigma_x, sigma_y;
    double theta;
  };

  Gaussian();

  Gaussian(Parameters p);

  void random(Parameters min, Parameters max, int seed);

  static Gaussian make_random(Parameters min, Parameters max, int seed);

  const Parameters& parameters() const;

  double operator()(double x, double y) const;

private:

  void init_abc();

  static double scale(double input, double min, double max);

  Parameters m_parameters;
  double m_a, m_b, m_c;

};


class Galaxy {

public:

  Galaxy();

  Galaxy(Gaussian model);

  void random(int seed);

  static Galaxy make_random(int seed);

  std::complex<double> coordinates() const;

  pos_type<2> shape() const;

  void fill(float* data, const pos_type<2>& shape) const;

  VecRaster<float> thumbnail() const;

private:

  Gaussian m_model;

};


class Observation {

public:

  Observation();

  void draw(const Raster<float>& thumbnail, std::complex<double> ra_dec);

  void save(std::string filename);

private:

  VecRaster<float> raster;

};

}
}
}

#endif
