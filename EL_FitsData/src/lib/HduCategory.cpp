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

#include <algorithm> // transform

namespace Euclid {
namespace FitsIO {

HduCategory::HduCategory() : m_mask(5, Trit {}) {}
HduCategory::HduCategory(HduCategory::TritPosition position, HduCategory::Trit value) : HduCategory {} {
  m_mask[static_cast<int>(position)] = value;
}

HduCategory HduCategory::operator~() const {
  HduCategory res(*this);
  return res.transform(boost::logic::operator!);
}

HduCategory& HduCategory::operator&=(const HduCategory& rhs) {
  return transform(rhs, restrictFlag);
}

HduCategory HduCategory::operator&(const HduCategory& rhs) const {
  HduCategory res(*this);
  res &= rhs;
  return res;
}

HduCategory& HduCategory::operator|=(const HduCategory& rhs) {
  return transform(rhs, extendFlag);
}

HduCategory HduCategory::operator|(const HduCategory& rhs) const {
  HduCategory res(*this);
  res |= rhs;
  return res;
}

bool HduCategory::operator==(const HduCategory& rhs) const {
  return m_mask == rhs.m_mask;
}

HduCategory::Trit HduCategory::toggleFlag(HduCategory::Trit rhs) {
  return !rhs;
}

HduCategory::Trit HduCategory::restrictFlag(HduCategory::Trit lhs, HduCategory::Trit rhs) {
  if (lhs == rhs) {
    return lhs;
  }
  if (lhs == HduCategory::Trit {}) {
    return rhs;
  }
  if (rhs == HduCategory::Trit {}) {
    return lhs;
  }
  throw std::runtime_error("Cannot restrict incompatible flags."); // FIXME Exception class
}

HduCategory::Trit HduCategory::extendFlag(HduCategory::Trit lhs, HduCategory::Trit rhs) {
  return lhs == rhs ? lhs : HduCategory::Trit {};
}

HduCategory& HduCategory::transform(std::function<Trit(Trit)> op) {
  std::transform(m_mask.begin(), m_mask.end(), m_mask.begin(), op);
  return *this;
}

HduCategory& HduCategory::transform(const HduCategory& rhs, std::function<Trit(Trit, Trit)> op) {
  std::transform(m_mask.begin(), m_mask.end(), rhs.m_mask.begin(), m_mask.begin(), op);
  return *this;
}

const HduCategory HduCategory::Any {};
const HduCategory HduCategory::Primary { HduCategory::TritPosition::PrimaryExt, false };
const HduCategory HduCategory::Metadata { HduCategory::TritPosition::MetadataData, false };
const HduCategory HduCategory::Image { HduCategory::TritPosition::ImageBinary, false };
const HduCategory HduCategory::IntImage { HduCategory::Image &
                                          HduCategory { HduCategory::TritPosition::IntFloatImage, false } };
const HduCategory HduCategory::RawImage { HduCategory::Image &
                                          HduCategory { HduCategory::TritPosition::RawCompressedImage, false } };

const HduCategory HduCategory::Ext { ~HduCategory::Primary };
const HduCategory HduCategory::Data { ~HduCategory::Metadata };
const HduCategory HduCategory::Bintable { HduCategory::Ext & ~HduCategory::Image };
const HduCategory HduCategory::FloatImage { HduCategory::Image &
                                            HduCategory { HduCategory::TritPosition::IntFloatImage, true } };
const HduCategory HduCategory::CompressedImageExt {
  HduCategory::Image & HduCategory { HduCategory::TritPosition::RawCompressedImage, true }
};

const HduCategory HduCategory::MetadataPrimary { HduCategory::Metadata & HduCategory::Primary };
const HduCategory HduCategory::DataPrimary { HduCategory::Data & HduCategory::Primary };
const HduCategory HduCategory::IntPrimary { HduCategory::IntImage & HduCategory::Primary };
const HduCategory HduCategory::FloatPrimary { HduCategory::FloatImage & HduCategory::Primary };
const HduCategory HduCategory::ImageExt { HduCategory::Image & HduCategory::Ext };
const HduCategory HduCategory::MetadataExt { HduCategory::Metadata & HduCategory::Ext };
const HduCategory HduCategory::DataExt { HduCategory::Data & HduCategory::Ext };
const HduCategory HduCategory::IntImageExt { HduCategory::IntImage & HduCategory::Ext };
const HduCategory HduCategory::FloatImageExt { HduCategory::FloatImage & HduCategory::Ext };

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