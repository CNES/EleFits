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
    m_file(fileSegment), m_memory(Segment::fromSize(memoryIndex, fileSegment.size())) {
  if (m_file.back == -1) {
    m_memory.back = -2;
  }
}

FileMemSegments::FileMemSegments(long fileIndex, const Segment& memorySegment) :
    m_file(Segment::fromSize(fileIndex, memorySegment.size())), m_memory(memorySegment) {
  if (m_memory.back == -1) {
    m_file.back = -2;
  }
}

const Segment& FileMemSegments::file() const {
  return m_file;
}

const Segment& FileMemSegments::memory() const {
  return m_memory;
}

long FileMemSegments::size() const {
  return m_file.size(); // FIXME resolve
}

void FileMemSegments::resolve(long fileBack, long memoryBack) {
  if (m_file.front == -1) {
    m_file.front = fileBack + 1;
  } else if (m_file.back == -1) {
    m_file.back = fileBack;
  }
  if (m_memory.back == -1) {
    m_memory.back = memoryBack;
  }
  if (m_file.back == -2) {
    m_file.back = m_file.front + m_memory.size() - 1;
  } else if (m_memory.back == -2) {
    m_memory.back = m_memory.front + m_file.size() - 1;
  }
}

long FileMemSegments::fileToMemory() const {
  return m_memory.front - m_file.front;
}

long FileMemSegments::memoryToFile() const {
  return m_file.front - m_memory.front;
}

} // namespace FitsIO
} // namespace Euclid
