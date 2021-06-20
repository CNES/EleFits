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

#ifndef _EL_FITSDATA_HDUCATEGORY_H
#define _EL_FITSDATA_HDUCATEGORY_H

#include <boost/logic/tribool.hpp>
#include <functional>
#include <vector>

namespace Euclid {
namespace FitsIO {

/*
 * @brief HDU category.
 * @details
 * The enumerators are designed to be used as bitmasks to filter on some properties.
 * For example, an HDU of category Primary is also of category ImageHdu and Hdu, but not of category Ext.
 * 
 * For image HDUs, two categories are defined, which mainly aims at simplifying compression-related features:
 * - Integer-valued for values of integral type;
 * - Real-valued for values of type float or double.
 * 
 * For any HDU, the data unit may be empty (i.e. `NAXIS = 0` or `NAXISn = 0` for at least one axis).
 * This is modeled as category Metadata, as opposed to category Data, which means there are values in the data unit.
 * 
 * They can be combined, e.g. to filter on integer-valued image extensions with non-empty data:
 * \code
 * HduCategory intImageExtWithData = HduCategory::IntImageExt & ~HduCategory::Metadata;
 * \endcode
 * 
 * Shortcuts are provided for common combinations, e.g. `ImageExt = Image | Ext`.
 * 
 * To test categories, bitwise boolean operators (`&`, `|`, `~`) are defined.
 * Additionally, operator <= allows checking whether an HDU category matches a set of categories.
 * 
 * Bits are ordered as follows:
 * - Data emptyness
 *   - Metadata
 *   - Data
 * - HDU index
 *   - Primary
 *   - Extension
 * - HDU type
 *   - Image
 *   - Binary table
 * - Image attributes
 *   - Integer pixels
 *   - Floating point pixels
 *   - Raw image
 *   - Compressed image
 * - 6 spare bits, which could later be used for states like:
 *   - Untouched
 *   - Accessed
 *   - Created
 *   - Edited
 */
class HduCategory;
/**
 * @brief An extensible HDU categorization for filtering and iteration.
 * @details FIXME
 */
class HduCategory {
protected:
  /**
   * @brief The positions of the trinary flags.
   */
  enum class TritPosition
  {
    PrimaryExt, ///< Primary/extension flag
    MetadataData, ///< Metadata/data flag
    ImageBinary, ///< Image/binary table flag
    IntFloatImage, ///< Integer-/real-valued image flag
    RawCompressedImage ///< Raw/compressed image flag
  };

  /**
   * @brief Shortcut for a trinary flag.
   */
  using Trit = boost::logic::tribool;

  /**
   * @brief Create an unconstrained category.
   */
  HduCategory();

  /**
   * @brief Create a category with a single flag constrained.
   */
  HduCategory(TritPosition position, Trit value);

public:
  /**
   * @brief Toggle flags.
   */
  HduCategory operator~() const;

  /**
   * @brief Restrict category (constrain flags).
   */
  HduCategory& operator&=(const HduCategory& rhs);

  /**
   * @copydoc operator&=
   */
  HduCategory operator&(const HduCategory& rhs) const;

  /**
   * @brief Extend category (release flags).
   */
  HduCategory& operator|=(const HduCategory& rhs);

  /**
   * @copydoc operator|=
   */
  HduCategory operator|(const HduCategory& rhs) const;

  /**
   * @brief Compare to category.
   */
  bool operator==(const HduCategory& rhs) const;

protected:
  /**
   * @brief the trinary flag mask.
   * @details
   * Each flag of the mask is either set to true, false, or indeterminate.
   * The trit positions are given by the TritPosition enumerators
   */
  std::vector<Trit> m_mask;

private:
  static Trit toggleFlag(Trit rhs);
  static Trit restrictFlag(Trit lhs, Trit rhs);
  static Trit extendFlag(Trit lhs, Trit rhs);
  HduCategory& transform(std::function<Trit(Trit)> op);
  HduCategory& transform(const HduCategory& rhs, std::function<Trit(Trit, Trit)> op);

public:
  /* Basic categories */
  static const HduCategory Any; //< Any HDU
  static const HduCategory Primary; ///< Primary HDU
  static const HduCategory Metadata; ///< HDU without data
  static const HduCategory Image; ///< Image HDU
  static const HduCategory IntImage; ///< Integer-valued image HDU
  static const HduCategory RawImage; ///< Raw (non-compressed) image HDU

  /* Opposite categories */
  static const HduCategory Ext; ///< Extension
  static const HduCategory Data; ///< HDU with data
  static const HduCategory Bintable; ///< Binary table HDU (necessarily an extension)
  static const HduCategory FloatImage; ///< Real-valued image HDU
  static const HduCategory
      CompressedImageExt; ///< Compressed image HDU (effectively written as a binary table extension)

  /* Compound categories */
  static const HduCategory MetadataPrimary; ///< Primary HDU without data
  static const HduCategory DataPrimary; ///< Primary HDU with data
  static const HduCategory IntPrimary; ///< Integer-valued Primary HDU
  static const HduCategory FloatPrimary; ///< Real-valued Primary HDU
  static const HduCategory ImageExt; ///< Image extension
  static const HduCategory MetadataExt; ///< Extension without data
  static const HduCategory DataExt; ///< Extension with data
  static const HduCategory IntImageExt; ///< Image extension with data
  static const HduCategory FloatImageExt; ///< Image extension without data
};

/**
 * @brief Check whether an input is an instance of (i.e. is more specific than) a given category.
 * @return True if all bits of input are set in category; False otherwise.
 */
inline bool isInstance(HduCategory input, HduCategory category) {
  return (input & category) == input;
}

/**
 * @brief HDU filter built from HDU categories.
 * @details
 * The class defines two lists of categories: accepted and rejected categories.
 * They are used in method accepts to check whether an HDU is of at least one accepted categories,
 * and of no rejected categories.
 */
class HduFilter {

public:
  /**
   * @brief Casting constructor to make a HduFilter from a HduCategory.
   */
  HduFilter(HduCategory category);

  /**
   * @brief Constructor.
   */
  HduFilter(const std::vector<HduCategory>& accept, const std::vector<HduCategory>& reject);

  /**
   * @brief Add an accepted category in place.
   */
  HduFilter& operator+=(HduCategory accept);

  /**
   * @brief Add an accepted category.
   */
  HduFilter operator+(HduCategory accept) const;

  /**
   * @brief Identity: provided from completeness only.
   */
  HduFilter& operator+();

  /**
   * @brief Add a rejected category in place.
   */
  HduFilter& operator-=(HduCategory reject);

  /**
   * @brief Add a rejected category.
   */
  HduFilter operator-(HduCategory reject) const;

  /**
   * @brief Negation operator: swap accepted and rejected categories.
   */
  HduFilter& operator-();

  /**
   * @brief Check whether the filter accepts a given input.
   * @return True if the input is an instance of *one of* the accepted categories
   * *and* is *not* an instance of *any of* the rejected categories.
   */
  bool accepts(HduCategory input) const;

private:
  /**
   * @brief Accepted categories.
   */
  std::vector<HduCategory> m_accept;

  /**
   * @brief Rejected categories.
   */
  std::vector<HduCategory> m_reject;
};

/**
 * @brief Combine two accepted categories as a filter.
 */
inline HduFilter operator+(HduCategory lhs, HduCategory rhs) {
  return HduFilter({ lhs, rhs }, {});
}

/**
 * @brief Combine an accepted and rejected categories as a filter.
 */
inline HduFilter operator-(HduCategory lhs, HduCategory rhs) {
  return HduFilter({ lhs }, { rhs });
}

/**
 * @brief Consider a category as a filter with only this category as the accepted category.
 */
inline HduFilter operator+(HduCategory rhs) {
  return HduFilter({ rhs }, {});
}

/**
 * @brief Consider a category as a filter with only this category as the rejected category.
 */
inline HduFilter operator-(HduCategory rhs) {
  return HduFilter({}, { rhs });
}

} // namespace FitsIO
} // namespace Euclid

#endif
