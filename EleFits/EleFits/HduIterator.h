// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_HDUITERATOR_H
#define _ELEFITS_HDUITERATOR_H

#include "EleFits/Hdu.h"

#include <iterator>

namespace Euclid {
namespace Fits {

/**
 * @ingroup iterators
 * @brief Helper class to provide filtered iterators.
 * @details
 * Functions begin(HduSelector) and end(HduSelector) are provided, so that it is possible to loop over HDUs as follows:
 * \code
 * MefFile f(...);
 * for (const auto& hdu : f.filter<ImageHdu>(HduCategory::ImageExt)) {
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
  HduIterator& operator++();

  /**
   * @brief New instance increment operator.
   */
  HduIterator operator++(int) const;

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
 * @relates HduIterator
 * @brief Beginning of an iterator to loop over all HDUs as `Hdu`s.
 */
HduIterator<> begin(MefFile& f);

/**
 * @relates HduIterator
 * @brief End of an iterator to loop over all HDUs as `Hdu`s.
 */
HduIterator<> end(MefFile& f);

/**
 * @relates HduIterator
 * @brief Beginning of an iterator to loop over selected HDUs.
 * @tparam THdu The desired HDU type
 * @param selector The HDU selector
 */
template <typename THdu = Hdu>
HduIterator<THdu> begin(HduSelector<THdu>& selector);

/**
 * @relates HduIterator
 * @brief End of an iterator to loop over selected HDUs.
 */
template <typename THdu = Hdu>
HduIterator<THdu> end(HduSelector<THdu>& selector);

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_HDUITERATOR_IMPL
#include "EleFits/impl/HduIterator.hpp"
#undef _ELEFITS_HDUITERATOR_IMPL
/// @endcond

#endif
