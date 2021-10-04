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

#ifndef _EL_FITSFILE_MEMFILEREGIONS_H
#define _EL_FITSFILE_MEMFILEREGIONS_H

#include "EL_FitsData/Region.h"

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup handlers
 * @brief Mapping between an in-memory (raster or column) region and an in-file (Fits data unit) region
 * for reading and writing data unit regions.
 */
template <long n = 2>
class MemFileRegions {

public:
  /**
   * @brief Create a mapping as an in-memory region and an in-file position.
   */
  MemFileRegions(const Region<n>& inMemory, const Position<n>& inFile) :
      m_memory(inMemory), m_file(Region<n>::fromShape(inFile, inMemory.shape())) {}

  /**
   * @brief Create a mapping as an in-memory position and an in-file region.
   */
  MemFileRegions(const Position<n>& inMemory, const Region<n>& inFile) :
      m_memory(Region<n>::fromShape(inMemory, inFile.shape())), m_file(inFile) {}

  /**
   * @brief Create a mapping with the same region in memory and in file.
   */
  MemFileRegions(const Region<n>& inMemoryAndFile) : m_memory(inMemoryAndFile), m_file(inMemoryAndFile) {}

  /**
   * @brief Get the in-memory region.
   */
  const Region<n>& inMemory() const {
    return m_memory;
  }

  /**
   * @brief Get the in-file region.
   */
  const Region<n>& inFile() const {
    return m_file;
  }

  /**
   * @brief Compute the translation vector from in-memory region to in-file region.
   */
  Position<n> memoryToFile() const {
    return m_file.front - m_memory.front;
  }

  /**
   * @brief Compute the translation vector from in-file region to in-memory region.
   */
  Position<n> fileToMemory() const {
    return m_memory.front - m_file.front;
  }

private:
  /**
   * @brief The in-memory region.
   */
  Region<n> m_memory;

  /**
   * @brief The in-file region.
   */
  Region<n> m_file;
};

/**
 * @ingroup handlers
 * @brief Create a `MemFileRegions` with in-file region at origin.
 */
template <long n>
MemFileRegions<n> makeMemRegion(const Region<n>& inMemory) {
  return MemFileRegions<n>(inMemory, Position<n>());
}

/**
 * @ingroup handlers
 * @brief Create a `MemFileRegions` with in-memory region at origin.
 */
template <long n>
MemFileRegions<n> makeFileRegion(const Region<n>& inFile) {
  return MemFileRegions<n>(Position<n>(), inFile);
}

} // namespace FitsIO
} // namespace Euclid

#endif
