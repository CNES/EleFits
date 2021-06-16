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

#include "EL_FitsData/HduCategory.h"

namespace Euclid {
namespace FitsIO {

HduFilter::HduFilter(HduCategory category) : m_accept { category }, m_reject {} {}

HduFilter::HduFilter(const std::vector<HduCategory>& accept, const std::vector<HduCategory>& reject) :
    m_accept { accept }, m_reject { reject } {}

HduFilter& HduFilter::operator+=(HduCategory accept) {
  m_accept.push_back(accept);
  return *this;
}

HduFilter HduFilter::operator+(HduCategory accept) const {
  HduFilter group(*this);
  group += accept;
  return group;
}

HduFilter& HduFilter::operator+() {
  return *this;
}

HduFilter& HduFilter::operator-=(HduCategory reject) {
  m_reject.push_back(reject);
  return *this;
}

HduFilter HduFilter::operator-(HduCategory reject) const {
  HduFilter group(*this);
  group -= reject;
  return group;
}

HduFilter& HduFilter::operator-() {
  std::swap(m_accept, m_reject);
  return *this;
}

bool HduFilter::accepts(HduCategory input) const {
  for (auto r : m_reject) {
    if (isInstance(input, r)) {
      return false;
    }
  }
  if (m_accept.size() == 0) {
    return true;
  }
  for (auto a : m_accept) {
    if (isInstance(input, a)) {
      return true;
    }
  }
  return false;
}

} // namespace FitsIO
} // namespace Euclid