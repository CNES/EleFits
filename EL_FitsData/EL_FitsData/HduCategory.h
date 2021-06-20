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

#include <functional> // function
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @brief An extensible HDU categorization for filtering and iteration.
 * @details
 * A category is defined as a sequence of trits (trinary bits).
 * For example, the type of an HDU can be image, binary table or unconstrained.
 * The following trits are defined:
 * - Primary HDU / extension,
 * - Metadata HDU (empty data unit) / HDU with data,
 * - Image / binary table HDU,
 * - Integer- / real-valued image HDU,
 * - Raw / compressed image HDU.
 * 
 * Other trits should be added in the future, e.g. to characterized the HDU state:
 * - Untouched / accessed,
 * - Opened / created,
 * - Read / edited...
 * 
 * User trits can also be added by extending the class.
 * 
 * Predefined categories are provided as static members, e.g. HduCategory::Primary or HduCategory::RawImage.
 * An HduCategory should not be created with a constructor, but rather by combining those categories with (trinary) bitwise operators.
 * For example, an integer-valued, non-empty image extension can be created using one of the following formulae:
 * \code
 * HduCategory intDataImageExt = HduCategory::IntImage & HduCategory::DataHdu & HduCategory::Ext;
 * HduCategory intDataImageExt = HduCategory::IntImage & HduCategory::DataExt;
 * HduCategory intDataImageExt = HduCategory::ImageExt & ~HduCategory::FloatPrimary;
 * \endcode
 * 
 * Method isInstance is provided to test whether a category validates a model.
 * Yet, in general, RecordHdu::matches is an adequate shortcut.
 * 
 * More complex, multi-category filters can be created with as HduFilter objects.
 */
class HduCategory {

protected:
  /**
   * @brief Trinary values.
   */
  enum class Trit
  {
    First, ///< First constrained option
    Second, ///< Second constrained option
    Unconstrained ///< Unconstrained
  };

  /**
   * @brief The positions of the trinary flags.
   */
  enum class TritPosition
  {
    PrimaryExt, ///< Primary/extension flag
    MetadataData, ///< Metadata/data flag
    ImageBintable, ///< Image/binary table flag
    IntFloatImage, ///< Integer-/real-valued image flag
    RawCompressedImage ///< Raw/compressed image flag
  };

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
   * @brief Equality operator.
   */
  bool operator==(const HduCategory& rhs) const;

  /**
   * @brief Non-equality operator.
   */
  bool operator!=(const HduCategory& rhs) const;

  /**
   * @brief Check whether the category validates (i.e. is more specific than) a given model.
   */
  bool isInstance(const HduCategory& model) const;

protected:
  /**
   * @brief The trinary flag mask.
   * @details
   * The trit positions are given by the TritPosition enumerators
   */
  std::vector<Trit> m_mask;

private:
  /**
   * @brief Toggle if flag is constrained, do nothing otherwise.
   * @details
   * This is a trinary not:
   * - ~First = Second,
   * - ~Second = First,
   * - ~Unconstrained = Unconstrained.
   */
  static Trit toggleFlag(Trit rhs);

  /**
   * @brief Restrict a flag.
   * @details
   * This is a symetric trinary and:
   * - Constrained & Unconstrained = Constrained,
   * - Unconstrained & Unconstrained = Unconstrained,
   * - First & First = First,
   * - Second & Second = Second,
   * - First & Second raises an exception.
   */
  static Trit restrictFlag(Trit lhs, Trit rhs);

  /**
   * @brief Extend a flag.
   * @details
   * This is a symetric trinary or:
   * - Constrained | Unconstrained = Unconstrained,
   * - Unconstraied | Unconstrained = Unconstrained,
   * - First | First = First,
   * - Second | Second = Second,
   * - First | Second = Unconstrained.
  static Trit extendFlag(Trit lhs, Trit rhs);

  /**
   * @brief Apply an unary operator to the flag mask.
   */
  HduCategory& transform(std::function<Trit(Trit)> op);

  /**
   * @brief Apply a binary operator to the flag mask.
   */
  HduCategory& transform(const HduCategory& rhs, std::function<Trit(Trit, Trit)> op);

public:
  /* Basic categories */
  static const HduCategory Any; //< Any HDU
  static const HduCategory Image; ///< Image HDU
  static const HduCategory Primary; ///< Primary image HDU
  static const HduCategory Metadata; ///< HDU without data
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
   * @brief Identity: provided for completeness only.
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
