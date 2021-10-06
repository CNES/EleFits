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

#ifndef _EL_FITSFILE_FILEMEMREGIONS_H
#define _EL_FITSFILE_FILEMEMREGIONS_H

#include "EL_FitsData/Region.h"

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup handlers
 * @brief Mapping between an in-file (Fits data unit) region and an in-memory (raster or column) region
 * for reading and writing data unit regions.
 * @warning
 * In-file and in-memory regions have the same shape.
 * Yet, if the back position of an input region is `Position::max()`,
 * then the other back position is conventionally set to `Position::zero()`.
 * This case has to be handled by users.
 * Both back positions cannot be `Position::max()` at the same time.
 */
template <long n = 2>
class FileMemRegions {

public:
  /**
   * @brief Create a mapping from an in-file region and an in-memory position.
   */
  FileMemRegions(const Region<n>& fileRegion, const Position<n>& memoryPosition = Position<n>::zero()) :
      m_file(fileRegion), m_memory(Region<n>::fromShape(memoryPosition, fileRegion.shape())) {
    if (m_file.back.isMax()) {
      m_memory.back = Position<n>::zero();
    }
  }
  // Not explicit to allow casting, e.g. readRegionTo(region, raster)

  /**
   * @brief Create a mapping from an in-file position and an in-memory region.
   */
  FileMemRegions(const Position<n>& filePosition, const Region<n>& memoryRegion = Region<n>::whole()) :
      m_file(Region<n>::fromShape(filePosition, memoryRegion.shape())), m_memory(memoryRegion) {
    if (m_memory.back.isMax()) {
      m_file.back = Position<n>::zero();
    }
  }

  /**
   * @brief Get the in-file region.
   * @details
   * See the warning at class level.
   */
  const Region<n>& file() const {
    return m_file;
  }

  /**
   * @brief Get the in-memory region.
   * @details
   * See the warning at class level.
   */
  const Region<n>& memory() const {
    return m_memory;
  }

  bool resolve(const Position<n>& fileShape, const Position<n>& memoryShape) {
    for (auto fit = m_file.back.begin(),
              fitEnd = m_file.back.end(),
              mit = m_memory.back.begin(),
              fsit = fileShape.begin(),
              msit = memoryShape.begin();
         fit != fitEnd;
         ++fit, ++mit, ++fsit, ++msit) {
      if (*fit == -1) {
        *fit += fsit;
      } else if (*mit == -1) {
        *mit += msit;
      }
    }
  }

  /**
   * @brief Update the back position of the file region if needed.
   * @return `true` if the back position was `max()`; `false` otherwise.
   * @details
   * The memory region is updated accordingly.
   * If the back position was not `max()`, nothing is done.
   */
  bool setFileBackIfMax(const Position<n>& back) {
    if (not m_file.back.isMax()) {
      return false;
    }
    m_file.back = back;
    m_memory.back = back + fileToMemory();
    return true;
  }

  /**
   * @brief Update the back position of the memory region if needed.
   * @return `true` if the back position was `max()`; `false` otherwise.
   * @details
   * The file region is updated accordingly.
   * If the back position was not `max()`, nothing is done.
   */
  bool setMemoryBackIfMax(const Position<n>& back) {
    if (not m_memory.back.isMax()) {
      return false;
    }
    m_memory.back = back;
    m_file.back = back + memoryToFile();
    return true;
  }

  /**
   * @brief Compute the translation vector from in-file region to in-memory region.
   */
  Position<n> fileToMemory() const {
    return m_memory.front - m_file.front;
  }

  /**
   * @brief Compute the translation vector from in-memory region to in-file region.
   */
  Position<n> memoryToFile() const {
    return m_file.front - m_memory.front;
  }

private:
  /**
   * @brief The in-file region.
   */
  Region<n> m_file;

  /**
   * @brief The in-memory region.
   */
  Region<n> m_memory;
};

/**
 * @ingroup handlers
 * @brief Create a `FileMemRegions` with in-file region at origin.
 */
template <long n>
FileMemRegions<n> makeMemRegion(const Region<n>& memoryRegion) {
  return FileMemRegions<n>(Position<n>::zero(), memoryRegion);
}

/**
 * @ingroup handlers
 * @brief Create a `FileMemRegions` with in-memory region at origin.
 */
template <long n>
FileMemRegions<n> makeFileRegion(const Region<n>& fileRegion) {
  return FileMemRegions<n>(fileRegion, Position<n>::zero());
}

} // namespace FitsIO
} // namespace Euclid

#endif
