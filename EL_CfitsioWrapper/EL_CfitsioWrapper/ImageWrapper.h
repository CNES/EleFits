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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"
#include "EL_FitsData/Raster.h"

#include <fitsio.h>
#include <string>

namespace Euclid {

// FIXME move to Raster.h
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief A _n_-D rectangle region, defined by its first and last positions,
 * or first position and shape.
 */
template <long n = 2>
class Region {
public:
  /**
   * @brief Create a region from a first position and shape.
   */
  static Region<n> fromOver(Position<n> first, Position<n> shape) {
    Region<n> region { first, first, shape };
    for (std::size_t i = 0; i < region.m_first.size(); ++i) { // TODO iterators
      region.m_last[i] += region.m_shape[i] - 1;
    }
    return region;
  }

  /**
   * @brief Create a region from first and last positions.
   */
  static Region<n> fromTo(Position<n> first, Position<n> last) {
    Region<n> region { first, last, last };
    for (std::size_t i = 0; i < region.m_first.size(); ++i) { // TODO iterators
      region.m_shape[i] -= region.m_first[i] - 1;
    }
    return region;
  }

  /**
   * @brief The first position in the region.
   */
  Position<n> first() const {
    return m_first;
  }

  /**
   * @brief The last position in the region.
   */
  Position<n> last() const {
    return m_last;
  }

  /**
   * @brief The region shape.
   */
  Position<n> shape() const {
    return m_shape;
  }

private:
  Region(Position<n> first, Position<n> last, Position<n> shape) : m_first(first), m_last(last), m_shape(shape) {}
  Position<n> m_first;
  Position<n> m_last;
  Position<n> m_shape;
};

/**
 * @ingroup image_data_classes
 * @brief A subraster as a view of a raster region.
 * @details
 * As opposed to a Raster, values of a Subraster are generally not contiguous in memory:
 * they are piece-wise contiguous only.
 * 
 * When a region is indeed contiguous, it is better to rely on a PtrRaster instead:
 * \code
 * VecRaster<char, 3> raster({ 800, 600, 3 });
 * 
 * // Good :)
 * auto region = Region<3>::fromOver({ 100, 100, 0 }, { 100, 100, 3 });
 * Subraster<char, 3> subraster { raster, region };
 * 
 * // Bad :(
 * auto slice = Region<3>::fromTo({ 0, 0, 1 }, { -1, -1, 1 });
 * Subraster<char, 3> contiguousSubraster { raster, slice };
 * 
 * // Good :)
 * PtrRaster<char, 2> ptrRaster({ 800, 600 }, &raster[{ 0, 0, 1 }]);
 * \endcode
 */
template <typename T, long n = 2>
struct Subraster {
  /**
   * @brief The parent raster.
   */
  Raster<T, n>& parent;

  /**
   * @brief The region.
   */
  Region<n> region;
};

} // namespace FitsIO

namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Read the value type of the current image HDU.
 */
const std::type_info& readTypeid(fitsfile* fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <long n = 2>
FitsIO::Position<n> readShape(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, long n = 2>
void updateShape(fitsfile* fptr, const FitsIO::Position<n>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, long n = 2>
FitsIO::VecRaster<T, n> readRaster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster.
 */
template <typename T, long n = 2>
void readRasterTo(fitsfile* fptr, FitsIO::Raster<T, n>& destination); // FIXME implement

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing subraster.
 */
template <typename T, long n = 2>
void readRasterTo(fitsfile* fptr, FitsIO::Subraster<T, n>& destination); // FIXME implement

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, long n = 2>
FitsIO::VecRaster<T, n> readRegion(fitsfile* fptr, const FitsIO::Region<n>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param destination The destination raster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination raster.
 */
template <typename T, long n = 2>
void readRegionTo(
    fitsfile* fptr,
    const FitsIO::Region<n>& region,
    FitsIO::Raster<T, n>& destination); // FIXME implement => non-const data()

/**
 * @brief Read a region of the current image HDU into a pre-existing subraster.
 * @param region The source region
 * @param destination The destination subraster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination subraster.
 */
template <typename T, long n = 2>
void readRegionTo(fitsfile* fptr, const FitsIO::Region<n>& region, FitsIO::Subraster<T, n>& destination);

/**
 * @brief Write a whole raster in the current image HDU.
 */
template <typename T, long n = 2>
void writeRaster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster);

/**
 * @brief Write a whole raster into a region of the current image HDU.
 * @param raster The raster to be written
 * @param region The destination region
 */
template <typename T, long n = 2>
void writeRegion(
    fitsfile* fptr,
    const FitsIO::Raster<T, n>& raster,
    const FitsIO::Region<n>& region); // FIXME implement

/**
 * @brief Write a subraster into a region of the current image HDU.
 * @param subraster The subraster to be written
 * @param region The destination region
 */
template <typename T, long n = 2>
void writeRegion(
    fitsfile* fptr,
    const FitsIO::Subraster<T, n>& subraster,
    const FitsIO::Region<n>& region); // FIXME implement

} // namespace Image
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/ImageWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
