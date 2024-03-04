// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_FILEMEMREGIONS_H
#define _ELEFITS_FILEMEMREGIONS_H

#include "Linx/Data/Box.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_handlers
 * @brief Mapping between an in-file (FITS image data unit) region and an in-memory (raster) region
 * for reading and writing image regions.
 * @details
 * In-file and in-memory regions have the same shape.
 * Both back positions cannot be -1 at the same index at the same time.
 */
template <long N = 2>
class FileMemRegions {
public:

  /**
   * @brief Create a mapping from an in-file region and an in-memory position.
   * @details
   * The shape of the in-memory region is deduced from that of the in-file region.
   * This constructor is not marked explicit, which allows casting from a `Box`.
   */
  FileMemRegions(const Linx::Box<N>& fileRegion, const Linx::Position<N>& memoryPosition = Linx::Position<N>::zero()) :
      m_file(fileRegion), m_memory(Linx::Box<N>::from_shape(memoryPosition, fileRegion.shape()))
  {
    if (m_file.back.isMax()) {
      m_memory.back = Linx::Position<N>::zero();
    }
  }

  /**
   * @brief Create a mapping from an in-file position and an in-memory region.
   * @details
   * The shape of the in-file region is deduced from that of the in-memory region.
   * This constructor is not marked explicit, which allows casting from a `Position`.
   */
  FileMemRegions(const Linx::Position<N>& filePosition, const Linx::Box<N>& memoryRegion = Linx::Box<N>::whole()) :
      m_file(Linx::Box<N>::from_shape(filePosition, memoryRegion.shape())), m_memory(memoryRegion)
  {
    if (m_memory.back.isMax()) {
      m_file.back = Linx::Position<N>::zero();
    }
  }

  /**
   * @brief Get the in-file region.
   */
  const Linx::Box<N>& file() const
  {
    return m_file;
  }

  /**
   * @brief Get the in-memory region.
   */
  const Linx::Box<N>& memory() const
  {
    return m_memory;
  }

  /**
   * @brief Resolve the unknown (-1) indices, given known in-file and in-memory back positions.
   */
  void resolve(const Linx::Position<N>& fileBack, const Linx::Position<N>& memoryBack)
  {
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
  Linx::Position<N> fileToMemory() const
  {
    return m_memory.front - m_file.front;
  }

  /**
   * @brief Compute the translation vector from in-memory region to in-file region.
   */
  Linx::Position<N> memoryToFile() const
  {
    return m_file.front - m_memory.front;
  }

private:

  /**
   * @brief The in-file region.
   */
  Linx::Box<N> m_file;

  /**
   * @brief The in-memory region.
   */
  Linx::Box<N> m_memory;
};

/**
 * @relates FileMemRegions
 * @brief Create a `FileMemRegions` with in-file position at origin.
 */
template <long N>
FileMemRegions<N> makeMemRegion(const Linx::Box<N>& memoryRegion)
{
  return FileMemRegions<N>(Linx::Position<N>::zero(), memoryRegion);
}

/**
 * @relates FileMemRegions
 * @brief Create a `FileMemRegions` with whole in-file region.
 */
template <long N>
FileMemRegions<N> makeMemRegion(const Linx::Position<N>& memoryPosition)
{
  return FileMemRegions<N>(Linx::Box<N>::whole(), memoryPosition);
}

} // namespace Fits
} // namespace Euclid

#endif
