// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_FILEMEMREGIONS_H
#define _ELEFITS_FILEMEMREGIONS_H

#include "EleFitsData/Region.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_handlers
 * @brief Mapping between an in-file (Fits image data unit) region and an in-memory (raster) region
 * for reading and writing image regions.
 * @details
 * In-file and in-memory regions have the same shape.
 * Both back positions cannot be -1 at the same index at the same time.
 */
template <long n = 2>
class FileMemRegions {

public:
  /**
   * @brief Create a mapping from an in-file region and an in-memory position.
   * @details
   * The shape of the in-memory region is deduced from that of the in-file region.
   * This constructor is not marked explicit, which allows casting from a `Region`.
   */
  FileMemRegions(const Region<n>& fileRegion, const Position<n>& memoryPosition = Position<n>::zero()) :
      m_file(fileRegion), m_memory(Region<n>::fromShape(memoryPosition, fileRegion.shape())) {
    if (m_file.back.isMax()) {
      m_memory.back = Position<n>::zero();
    }
  }

  /**
   * @brief Create a mapping from an in-file position and an in-memory region.
   * @details
   * The shape of the in-file region is deduced from that of the in-memory region.
   * This constructor is not marked explicit, which allows casting from a `Position`.
   */
  FileMemRegions(const Position<n>& filePosition, const Region<n>& memoryRegion = Region<n>::whole()) :
      m_file(Region<n>::fromShape(filePosition, memoryRegion.shape())), m_memory(memoryRegion) {
    if (m_memory.back.isMax()) {
      m_file.back = Position<n>::zero();
    }
  }

  /**
   * @brief Get the in-file region.
   */
  const Region<n>& file() const {
    return m_file;
  }

  /**
   * @brief Get the in-memory region.
   */
  const Region<n>& memory() const {
    return m_memory;
  }

  /**
   * @brief Resolve the unknown (-1) indices, given known in-file and in-memory back positions.
   */
  void resolve(const Position<n>& fileBack, const Position<n>& memoryBack) {
    const auto ftom = fileToMemory();
    for (auto fit = m_file.back.begin(),
              fitEnd = m_file.back.end(),
              mit = m_memory.back.begin(),
              fbit = fileBack.begin(),
              mbit = memoryBack.begin(),
              ftomit = ftom.begin();
         fit != fitEnd;
         ++fit, ++mit, ++fbit, ++mbit, ++ftomit) {
      // TODO check that fit and mit are not -1 at the same time? In Ctor?
      if (*fit == -1) {
        *fit = *fbit;
        *mit = *fit + *ftomit;
      } else if (*mit == -1) {
        *mit = *mbit;
        *fit = *mit - *ftomit;
      }
    }
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
 * @relates FileMemRegions
 * @brief Create a `FileMemRegions` with in-file position at origin.
 */
template <long n>
FileMemRegions<n> makeMemRegion(const Region<n>& memoryRegion) {
  return FileMemRegions<n>(Position<n>::zero(), memoryRegion);
}

/**
 * @relates FileMemRegions
 * @brief Create a `FileMemRegions` with whole in-file region.
 */
template <long n>
FileMemRegions<n> makeMemRegion(const Position<n>& memoryPosition) {
  return FileMemRegions<n>(Region<n>::whole(), memoryPosition);
}

} // namespace Fits
} // namespace Euclid

#endif
