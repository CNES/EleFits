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

#if defined(_EL_FITSFILE_HDUITERATOR_IMPL) || defined(CHECK_QUALITY)

  #include "EleFitsFile/HduIterator.h"

namespace Euclid {
namespace FitsIO {

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
const THdu& HduIterator<THdu>::operator++() {
  next();
  return m_hdu->as<THdu>();
}

template <typename THdu>
const THdu* HduIterator<THdu>::operator++(int) {
  next();
  if (not m_hdu) {
    return nullptr;
  }
  return &m_hdu->as<THdu>();
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
  return { selector.mef, 0, selector.filter };
}

template <typename THdu>
HduIterator<THdu> end(HduSelector<THdu>& selector) {
  return { selector.mef, selector.mef.hduCount(), selector.filter };
}

} // namespace FitsIO
} // namespace Euclid

#endif
