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

#include "EL_FitsIO_Examples/Observation.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <random>

#include "EL_FitsFile/SifFile.h"

namespace Euclid {
namespace FitsIO {
namespace Example {

Gaussian::Gaussian() :
    Gaussian({0, 0, 0, 0, 0, 0}) {}

Gaussian::Gaussian(Parameters p) :
    m_parameters(p) {
  init_abc();
}

void Gaussian::init_abc() {
  const auto sx2 = m_parameters.sigma_x * m_parameters.sigma_x;
  const auto sy2 = m_parameters.sigma_y * m_parameters.sigma_y;
  const auto sin_t = sin(m_parameters.theta);
  const auto sin_2t = sin(2. * m_parameters.theta);
  const auto sin2_t = sin_t * sin_t;
  const auto cos2_t = 1. - sin2_t;
  m_a = cos2_t / (2. * sx2) + sin2_t / (2. * sy2);
  m_b = -sin_2t / (4. * sx2) + sin_2t / (4. * sy2);
  m_c = sin2_t / (2. * sx2) + cos2_t / (2. * sy2);
}

void Gaussian::random(Parameters min, Parameters max, int seed) {
  std::default_random_engine gen(seed * 1000);
  std::uniform_real_distribution<> dis(0., 1.);
  m_parameters.amp = scale(dis(gen), 0., 1.);
  m_parameters.x0 = scale(dis(gen), min.x0, max.x0);
  m_parameters.y0 = scale(dis(gen), min.y0, max.y0);
  m_parameters.sigma_x = scale(dis(gen), min.sigma_x, max.sigma_x);
  m_parameters.sigma_y = scale(dis(gen), min.sigma_y, max.sigma_y);
  m_parameters.theta = scale(dis(gen), min.theta, max.theta);
  init_abc();
}

Gaussian Gaussian::make_random(Parameters min, Parameters max, int seed) {
  Gaussian g;
  g.random(min, max, seed);
  return g;
}

const Gaussian::Parameters& Gaussian::parameters() const {
  return m_parameters;
}

double Gaussian::operator()(double x, double y) const {
  const auto u = x - m_parameters.x0;
  const auto v = y - m_parameters.y0;
  const auto w = m_a * u * u + 2. * m_b * u * v + m_c * v * v;
  return m_parameters.amp * exp(-w);
}

double Gaussian::scale(double input, double min, double max) {
  return min + input * (max - min);
}

Galaxy::Galaxy() :
    m_model() {}

Galaxy::Galaxy(Gaussian model) :
    m_model(model) {}

void Galaxy::random(int seed) {
  Gaussian::Parameters min {0, -180, -90, .1, .1, -M_PI_4};
  Gaussian::Parameters max {1, 180, 90, 2, 2, M_PI_4};
  m_model.random(min, max, seed);
}

Galaxy Galaxy::make_random(int seed) {
  Galaxy g;
  g.random(seed);
  return g;
}

std::complex<double> Galaxy::coordinates() const {
  return {m_model.parameters().x0, m_model.parameters().y0};
}

pos_type<2> Galaxy::shape() const {
  const auto& p = m_model.parameters();
  const double sx = p.sigma_x * abs(sin(p.theta));
  const double sy = p.sigma_y * abs(cos(p.theta));
  const long width = 2 * ceil(5 * sx) + 1;
  const long height = 2 * ceil(5 * sy) + 1;
  return {width, height};
}

void Galaxy::fill(float* data, const pos_type<2>& shape) const {
  const auto w = shape[0];
  const auto h = shape[1];
  const auto x0 = m_model.parameters().x0;
  const auto y0 = m_model.parameters().y0;
  for(long y = 0; y < h; ++y)
    for(long x = 0; x < w; ++x)
      *(data + x + y * w) = m_model(x + x0 - w / 2, y + y0 - h / 2);
}

VecRaster<float> Galaxy::thumbnail() const {
  VecRaster<float> raster(shape());
  fill(raster.data(), shape());
  return raster;
}

Observation::Observation() :
    raster({360, 180}) {
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  std::fill(begin, end, 0.F);
}

void Observation::draw(const Raster<float>& thumbnail, std::complex<double> ra_dec) {
  const auto twidth = thumbnail.length<0>();
  const auto theight = thumbnail.length<1>();
  const long x = 180.5 + ra_dec.real() - twidth;
  const long y = 90.5 + ra_dec.imag() - theight;
  if(x < 0 || x + twidth >= raster.length<0>()) return;
  if(y < 0 || y + theight >= raster.length<1>()) return;
  auto tbegin = thumbnail.data();
  const auto rwidth = raster.length<0>();
  auto rbegin = &raster[{x, y}];
  for(long j=0; j<theight; ++j) {
    std::transform(tbegin, tbegin + twidth, rbegin, rbegin, std::plus<float>());
    tbegin += twidth;
    rbegin += rwidth;
  }
}

void Observation::save(std::string filename) {
  SifFile f(filename, SifFile::Permission::CREATE);
  f.write_raster(raster);
}

}
}
}
