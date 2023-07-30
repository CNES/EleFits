// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/HduCategory.h"

#include <algorithm> // transform

namespace Euclid {
namespace Fits {

HduCategory::HduCategory() : m_mask(static_cast<std::size_t>(TritPosition::TritCount), Trit::Unconstrained) {}

HduCategory::HduCategory(HduCategory::TritPosition position, HduCategory::Trit value) : HduCategory() {
  m_mask[static_cast<int>(position)] = value;
}

HduCategory HduCategory::type() const {
  auto trit = m_mask[static_cast<std::size_t>(TritPosition::ImageBintable)];
  if (trit == Trit::First) {
    return HduCategory::Image;
  } else if (trit == Trit::Second) {
    return HduCategory::Bintable;
  }
  return HduCategory();
}

HduCategory HduCategory::operator~() const {
  HduCategory res(*this);
  return res.transform(toggleFlag);
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

HduCategory& HduCategory::operator<<=(const HduCategory& rhs) {
  return transform(rhs, overwriteFlag);
}

HduCategory HduCategory::operator<<(const HduCategory& rhs) const {
  HduCategory res(*this);
  res <<= rhs;
  return res;
}

bool HduCategory::operator==(const HduCategory& rhs) const {
  return m_mask == rhs.m_mask;
}

bool HduCategory::operator!=(const HduCategory& rhs) const {
  return not operator==(rhs);
}

bool HduCategory::isInstance(const HduCategory& model) const {
  try {
    return (*this & model) == *this;
  } catch (IncompatibleTrits&) {
    return false;
  }
}

HduCategory::Trit HduCategory::toggleFlag(HduCategory::Trit rhs) {
  switch (rhs) {
    case Trit::First:
      return Trit::Second;
    case Trit::Second:
      return Trit::First;
    default:
      return Trit::Unconstrained;
  }
}

HduCategory::Trit HduCategory::restrictFlag(HduCategory::Trit lhs, HduCategory::Trit rhs) {

  if (lhs == rhs) {
    return lhs;
  }
  if (lhs == Trit::Unconstrained) {
    return rhs;
  }
  if (rhs == Trit::Unconstrained) {
    return lhs;
  }

  throw HduCategory::IncompatibleTrits();
}

HduCategory::Trit HduCategory::extendFlag(HduCategory::Trit lhs, HduCategory::Trit rhs) {
  return lhs == rhs ? lhs : Trit::Unconstrained;
}

HduCategory::Trit HduCategory::overwriteFlag(Trit lhs, Trit rhs) {
  return rhs == Trit::Unconstrained ? lhs : rhs;
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
const HduCategory HduCategory::Image {HduCategory::TritPosition::ImageBintable, HduCategory::Trit::First};
const HduCategory HduCategory::Primary {
    HduCategory::Image& HduCategory {HduCategory::TritPosition::PrimaryExt, HduCategory::Trit::First}};
const HduCategory HduCategory::Metadata {HduCategory::TritPosition::MetadataData, HduCategory::Trit::First};
const HduCategory HduCategory::IntImage {
    HduCategory::Image& HduCategory {HduCategory::TritPosition::IntFloatImage, HduCategory::Trit::First}};
const HduCategory HduCategory::RawImage {
    HduCategory::Image& HduCategory {HduCategory::TritPosition::RawCompressedImage, HduCategory::Trit::First}};

const HduCategory HduCategory::Ext {HduCategory::TritPosition::PrimaryExt, HduCategory::Trit::Second};
const HduCategory HduCategory::Data {~HduCategory::Metadata};
const HduCategory HduCategory::Bintable {HduCategory::Ext & ~HduCategory::Image};
const HduCategory HduCategory::FloatImage {
    HduCategory::Image& HduCategory {HduCategory::TritPosition::IntFloatImage, HduCategory::Trit::Second}};
const HduCategory HduCategory::CompressedImageExt {
    HduCategory::Image& HduCategory {HduCategory::TritPosition::RawCompressedImage, HduCategory::Trit::Second}};

const HduCategory HduCategory::MetadataPrimary {HduCategory::Metadata & HduCategory::Primary};
const HduCategory HduCategory::DataPrimary {HduCategory::Data & HduCategory::Primary};
const HduCategory HduCategory::IntPrimary {HduCategory::IntImage & HduCategory::Primary};
const HduCategory HduCategory::FloatPrimary {HduCategory::FloatImage & HduCategory::Primary};
const HduCategory HduCategory::ImageExt {HduCategory::Image & HduCategory::Ext};
const HduCategory HduCategory::MetadataExt {HduCategory::Metadata & HduCategory::Ext};
const HduCategory HduCategory::DataExt {HduCategory::Data & HduCategory::Ext};
const HduCategory HduCategory::IntImageExt {HduCategory::IntImage & HduCategory::Ext};
const HduCategory HduCategory::FloatImageExt {HduCategory::FloatImage & HduCategory::Ext};

const HduCategory HduCategory::Untouched {HduCategory::TritPosition::UntouchedTouched, HduCategory::Trit::First};
const HduCategory HduCategory::Touched {~HduCategory::Untouched};
const HduCategory HduCategory::Existed {HduCategory::TritPosition::ExistedCreated, HduCategory::Trit::First};
const HduCategory HduCategory::OnlyRead {
    HduCategory::Touched& HduCategory {HduCategory::TritPosition::ReadEdited, HduCategory::Trit::First}};
const HduCategory HduCategory::Edited {HduCategory::TritPosition::ReadEdited, HduCategory::Trit::Second};
const HduCategory HduCategory::Created {~HduCategory::Existed & HduCategory::Edited};

template <>
HduCategory HduCategory::forClass<Hdu>() {
  return HduCategory::Any;
}

template <>
HduCategory HduCategory::forClass<Header>() {
  return HduCategory::Any;
}

template <>
HduCategory HduCategory::forClass<ImageHdu>() {
  return HduCategory::Image;
}

template <>
HduCategory HduCategory::forClass<ImageRaster>() {
  return HduCategory::Image;
}

template <>
HduCategory HduCategory::forClass<BintableHdu>() {
  return HduCategory::Bintable;
}

template <>
HduCategory HduCategory::forClass<BintableColumns>() {
  return HduCategory::Bintable;
}

HduFilter::HduFilter(const HduCategory& category) : m_accept {category}, m_reject {} {}

HduFilter::HduFilter(const std::vector<HduCategory>& accept, const std::vector<HduCategory>& reject) :
    m_accept {accept}, m_reject {reject} {}

HduFilter& HduFilter::operator+=(const HduCategory& accept) {
  m_accept.push_back(accept);
  return *this;
}

HduFilter HduFilter::operator+(const HduCategory& accept) const {
  HduFilter filter(*this);
  filter += accept;
  return filter;
}

HduFilter& HduFilter::operator+() {
  return *this;
}

HduFilter& HduFilter::operator*=(const HduCategory& constraint) {
  for (auto& a : m_accept) {
    a &= constraint;
  }
  return *this;
}

HduFilter HduFilter::operator*(const HduCategory& constraint) const {
  HduFilter filter(*this);
  filter *= constraint;
  return filter;
}

HduFilter& HduFilter::operator-=(const HduCategory& reject) {
  m_reject.push_back(reject);
  return *this;
}

HduFilter HduFilter::operator-(const HduCategory& reject) const {
  HduFilter filter(*this);
  filter -= reject;
  return filter;
}

HduFilter& HduFilter::operator-() {
  std::swap(m_accept, m_reject);
  return *this;
}

HduFilter& HduFilter::operator/=(const HduCategory& constraint) {
  for (auto& r : m_reject) {
    r &= constraint;
  }
  return *this;
}

HduFilter HduFilter::operator/(const HduCategory& constraint) const {
  HduFilter filter(*this);
  filter /= constraint;
  return filter;
}

bool HduFilter::accepts(const HduCategory& input) const {
  for (auto r : m_reject) {
    if (input.isInstance(r)) {
      return false;
    }
  }
  if (m_accept.size() == 0) {
    return true;
  }
  for (auto a : m_accept) {
    if (input.isInstance(a)) {
      return true;
    }
  }
  return false;
}

} // namespace Fits
} // namespace Euclid
