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

#include "EL_FitsFile/FileMemSegments.h"

namespace Euclid {
namespace FitsIO {

FileMemSegments::FileMemSegments(const Segment& fileSegment, long memoryIndex) :
    m_file(fileSegment), m_memory(Segment::fromSize(memoryIndex, fileSegment.size())) {}

FileMemSegments::FileMemSegments(long fileIndex, const Segment& memorySegment) :
    m_file(Segment::fromSize(fileIndex, memorySegment.size())), m_memory(memorySegment) {}

const Segment& FileMemSegments::file() const {
  return m_file;
}

const Segment& FileMemSegments::memory() const {
  return m_memory;
}

void FileMemSegments::resolve(long fileBack, long memoryBack) {
  if (m_file.back == -1) {
    m_file.back = fileBack;
    m_memory.back = m_file.back + fileToMemory();
  } else if (m_memory.back == -1) {
    m_memory.back = memoryBack;
    m_file.back = m_memory.back + memoryToFile();
  }
}

long FileMemSegments::fileToMemory() const {
  return m_memory.front - m_file.back;
}

long FileMemSegments::memoryToFile() const {
  return m_file.front - m_memory.back;
}

} // namespace FitsIO
} // namespace Euclid
