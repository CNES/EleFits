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

#include "EleFits/Hdu.h"

#include <iterator>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup iterators
 * @brief Helper class to provide filtered iterators.
 * @details
 * Functions begin(HduSelector) and end(HduSelector) are provided, so that it is possible to loop over HDUs as follows:
 * \code
 * MefFile f(...);
 * for (const auto& hdu : f.selectAs<ImageHdu>(HduCategory::ImageExt)) {
 *   ... // hdu is an image extension of type ImageHdu
 * }
 * \endcode
 */
template <typename THdu>
struct HduSelector {
  /**
     * @brief The MefFile to apply the selector on.
     */
  MefFile& mef;
  /**
     * @brief The HDU filter to be applied.
     */
  HduFilter filter;
};

/**
 * @ingroup iterators
 * @brief Iterator for MefFile's HDUs.
 * @tparam TCategories The categories to be filtered
 * @tparam THdu The type of HDU class (iterator elements), can generally be unspecified (see details below)
 * @details
 * If THdu is not specified, the following deduction rules are applied:
 * - If bit Image = 1 and bit Bintable = 0 in TCategories, THdu = ImageHdu;
 * - If bit Image = 0 and bit Bintable = 1 in TCategories, THdu = Bintable;
 * - Otherwise, THdu = Hdu.
 * 
 * If this default value is not adequate, THdu can be specified.
 */
template <typename THdu = Hdu>
class HduIterator : public std::iterator<std::input_iterator_tag, const THdu> {
public:
  /**
   * @brief Constructor.
   */
  HduIterator(MefFile& f, long index, HduFilter filter = HduCategory::Any);

  /**
   * @brief Dereference operator.
   */
  const THdu& operator*() const;

  /**
   * @brief Arrow operator.
   */
  const THdu* operator->() const;

  /**
   * @brief Increment operator.
   */
  const THdu& operator++();

  /**
   * @brief Increment operator.
   */
  const THdu* operator++(int);

  /**
   * @brief Equality operator.
   */
  bool operator==(const HduIterator<THdu>& rhs) const;

  /**
   * @brief Non-equality operator.
   */
  bool operator!=(const HduIterator<THdu>& rhs) const;

private:
  /**
   * @brief Move to next HDU with valid category.
   */
  void next();

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
  const Hdu* m_hdu;

  /**
   * @brief The HDU filter.
   */
  HduFilter m_filter;

  /**
   * @brief Dummy HDU for past-the-last element access.
   */
  const THdu m_dummyHdu;
};

/**
 * @ingroup iterators
 * @brief Beginning of an iterator to loop over all HDUs as `Hdu`s.
 */
HduIterator<> begin(MefFile& f);

/**
 * @ingroup iterators
 * @brief End of an iterator to loop over all HDUs as `Hdu`s.
 */
HduIterator<> end(MefFile& f);

/**
 * @ingroup iterators
 * @brief Beginning of an iterator to loop over selected HDUs.
 * @tparam THdu The desired HDU type
 * @param selector The HDU selector
 */
template <typename THdu = Hdu>
HduIterator<THdu> begin(HduSelector<THdu>& selector);

/**
 * @ingroup iterators
 * @brief End of an iterator to loop over selected HDUs.
 */
template <typename THdu = Hdu>
HduIterator<THdu> end(HduSelector<THdu>& selector);

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_HDUITERATOR_IMPL
#include "EleFits/impl/HduIterator.hpp"
#undef _EL_FITSFILE_HDUITERATOR_IMPL
/// @endcond

#endif
