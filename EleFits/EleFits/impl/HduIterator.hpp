// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_HDUITERATOR_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/HduIterator.h"

namespace Euclid {
namespace Fits {

template <typename THdu>
HduIterator<THdu>::HduIterator(MefFile& f, long index, HduFilter filter) :
    m_f(f), m_index(index - 1), m_hdu(nullptr), m_filter(filter), m_dummyHdu() {
  next();
}

template <typename THdu>
const THdu& HduIterator<THdu>::operator*() const {
  return m_hdu->as<THdu>();
}

template <typename THdu>
const THdu* HduIterator<THdu>::operator->() const {
  if (not m_hdu) {
    return nullptr;
  }
  return &m_hdu->as<THdu>();
}

template <typename THdu>
HduIterator<THdu>& HduIterator<THdu>::operator++() {
  next();
  return *this;
}

template <typename THdu>
HduIterator<THdu> HduIterator<THdu>::operator++(int) const {
  auto out = *this;
  ++out;
  return out;
}

template <typename THdu>
bool HduIterator<THdu>::operator==(const HduIterator<THdu>& rhs) const {
  return m_index == rhs.m_index; // TODO Should we test m_f, too?
}

template <typename THdu>
bool HduIterator<THdu>::operator!=(const HduIterator<THdu>& rhs) const {
  return not(*this == rhs);
}

template <typename THdu>
void HduIterator<THdu>::next() {
  do {
    m_index++;
    if (m_index >= m_f.hduCount()) {
      m_index = m_f.hduCount();
      m_hdu = &m_dummyHdu;
      return;
    }
    m_hdu = &m_f[m_index];
  } while (not(m_hdu->matches(m_filter)));
}

template <typename THdu>
HduIterator<THdu> begin(HduSelector<THdu>& selector) {
  return {selector.mef, 0, selector.filter};
}

template <typename THdu>
HduIterator<THdu> end(HduSelector<THdu>& selector) {
  return {selector.mef, selector.mef.hduCount(), selector.filter};
}

} // namespace Fits
} // namespace Euclid

#endif
