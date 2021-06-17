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

#include <vector>

namespace Euclid {
namespace FitsIO {

/**
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
enum HduCategory
{
  Metadata = 0b10'11'11'1111'111111, ///< HDU without data
  Data = 0b0111'11'1111'111111, ///< HDU with data
  Primary = 0b11'10'10'1111'111111, ///< Primary HDU
  MetadataPrimary = Metadata & Primary, ///< Primary HDU without data
  DataPrimary = Data & Primary, ///< Primary HDU with data
  Ext = 0b11'01'11'1111'111111, ///< Extension
  MetadataExt = Metadata & Ext, ///< Extension without data
  DataExt = Data & Ext, ///< Extension with data
  Image = 0b11'11'10'1111'111111, ///< Image HDU
  MetadataImage = Metadata & Image, ///< Image HDU without data
  DataImage = Data & Image, ///< Image HDU with data
  IntImage = 0b11'11'10'1011'111111, ///< Integer-valued image HDU
  FloatImage = 0b11'11'10'0111'111111, ///< Real-valued image HDU
  ImageExt = Image & Ext, ///< Image extension
  MetadataImageExt = Metadata & ImageExt, ///< Image extension without data
  DataImageExt = Data & ImageExt, ///< Image extension with data
  IntImageExt = IntImage & Ext, ///< Integer-valued image extension
  FloatImageExt = FloatImage & Ext, ///< Real-valued image extension
  Bintable = 0b11'01'01'1111'111111, ///< Binary table HDU
  Any = 0b11'11'11'1111'111111 ///< Any HDU
};

/**
 * @brief Bit-wise OR operator for masking.
 * @see HduCategory
 */
inline HduCategory operator|(HduCategory a, HduCategory b) {
  return static_cast<HduCategory>(static_cast<int>(a) | static_cast<int>(b));
}

/**
 * @brief In-place bit-wise OR operator for masking.
 * @see HduCategory
 */
inline HduCategory& operator|=(HduCategory& a, HduCategory b) {
  a = a | b;
  return a;
}

/**
 * @brief Bit-wise AND operator for masking.
 * @see HduCategory
 */
inline HduCategory operator&(HduCategory a, HduCategory b) {
  return static_cast<HduCategory>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * @brief In-place bit-wise AND operator for masking.
 * @see HduCategory
 */
inline HduCategory& operator&=(HduCategory& a, HduCategory b) {
  a = a & b;
  return a;
}

/**
 * @brief Bit-wise binary NOT operator for masking.
 * @see HduCategory
 */
inline HduCategory operator~(HduCategory a) {
  return static_cast<HduCategory>(~static_cast<int>(a));
}

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
