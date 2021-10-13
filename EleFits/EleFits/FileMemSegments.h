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

#ifndef _ELEFITS_FILEMEMSEGMENTS_H
#define _ELEFITS_FILEMEMSEGMENTS_H

#include "EleFitsData/DataUtils.h" // Segment

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
