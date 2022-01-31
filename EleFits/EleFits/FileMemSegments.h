// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_FILEMEMSEGMENTS_H
#define _ELEFITS_FILEMEMSEGMENTS_H

#include "EleFitsData/Segment.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup bintable_handlers
 * @brief Mapping between an in-file (Fits binary table data unit) segment and an in-memory (column) segment
 * for reading and writing bintable segments.
 */
class FileMemSegments {

public:
  /**
   * @brief Create a mapping from an in-file segment and an in-memory position.
   */
  FileMemSegments(const Segment& fileSegment, long memoryIndex = 0);

  /**
   * @brief Create a mapping from an in-file position and an in-file segment.
   */
  FileMemSegments(long fileIndex, const Segment& memorySegment = Segment::whole());

  /**
   * @brief Get the in-file segment.
   */
  const Segment& file() const;

  /**
   * @brief Get the in-memory segment.
   */
  const Segment& memory() const;

  /**
   * @brief Get the common segment size.
   */
  long size() const;

  /**
   * @brief Resolve the unknown (-1) indices, given known in-file and in-memory back indices.
   */
  void resolve(long fileBack, long memoryBack);

  /**
   * @brief Compute the index offset from in-file segment to in-memory segment.
   */
  long fileToMemory() const;

  /**
   * @brief Compute the index offset from in-memory segment to in-file segment.
   */
  long memoryToFile() const;

private:
  /**
   * @brief The in-file segment.
   */
  Segment m_file;

  /**
   * @brief The in-memory segment.
   */
  Segment m_memory;
};

} // namespace Fits
} // namespace Euclid

#endif
