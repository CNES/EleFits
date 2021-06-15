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

#ifndef _EL_FITSFILE_HDUITERATOR_H
#define _EL_FITSFILE_HDUITERATOR_H

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsFile/RecordHdu.h"

#include <iterator>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Iterator for MefFile's HDUs.
 * @tparam TCategories The categories to be filtered
 * @tparam THdu The type of HDU class (iterator elements), can generally be unspecified (see details below)
 * @details
 * If THdu is not specified, the following deduction rules are applied:
 * - If bit Image = 1 and bit Bintable = 0 in TCategories, THdu = ImageHdu;
 * - If bit Image = 0 and bit Bintable = 1 in TCategories, THdu = Bintable;
 * - Otherwise, THdu = RecordHdu.
 * 
 * If this default value is not adequate, THdu can be specified.
 */
template <HduCategory TCategories = HduCategory::Any, typename THdu = typename HduCategoryTraits<TCategories>::HduClass>
class HduIterator : public std::iterator<std::output_iterator_tag, const THdu> {
public:
  /**
   * @brief Constructor.
   */
  HduIterator(MefFile& f, long index) : m_f(f), m_index(index - 1), m_hdu(nullptr) {
    next();
  }

  /**
   * @brief Dereference operator.
   */
  const THdu& operator*() const {
    return m_hdu->as<THdu>();
  }

  const THdu* operator->() const {
    return &m_hdu->as<THdu>();
  }

  /**
   * @brief Increment operator.
   */
  const THdu& operator++() {
    next();
    return m_hdu->as<THdu>();
  }

  /**
   * @brief Increment operator.
   */
  const THdu* operator++(int) {
    next();
    return &m_hdu->as<THdu>();
  }

  /**
   * @brief Non-equality operator.
   */
  bool operator!=(const HduIterator& rhs) const {
    return m_index != rhs.m_index; // TODO Should we test m_f, too?
  }

private:
  /**
   * @brief Move to next HDU with valid category.
   */
  void next() {
    do {
      m_index++;
      if (m_index == m_f.hduCount()) {
        return;
      }
      m_hdu = &m_f[m_index];
    } while (not(m_hdu->isInstance<TCategories>()));
  }

  /**
   * @brief The MEF file handler.
   */
  MefFile& m_f;

  /**
   * @brief The current HDU index.
   */
  long m_index;

  /**
   * @brief The current HDU.
   */
  const RecordHdu* m_hdu;
};

/**
 * @brief Beginning of an iterator to loop over HDUs as RecordHdus.
 */
HduIterator<> begin(MefFile& f) {
  return { f, 0 };
}

/**
 * @brief End of an iterator to loop over HDUs as RecordHdus.
 */
HduIterator<> end(MefFile& f) {
  return { f, f.hduCount() };
}

template <HduCategory TCategories = HduCategory::Any, typename THdu = typename HduCategoryTraits<TCategories>::HduClass>
HduIterator<TCategories, THdu> begin(MefFile::Selector<TCategories, THdu>& selector) {
  return { selector.mef, 0 };
}

template <HduCategory TCategories = HduCategory::Any, typename THdu = typename HduCategoryTraits<TCategories>::HduClass>
HduIterator<TCategories, THdu> end(MefFile::Selector<TCategories, THdu>& selector) {
  return { selector.mef, selector.mef.hduCount() };
}

} // namespace FitsIO
} // namespace Euclid

#endif
