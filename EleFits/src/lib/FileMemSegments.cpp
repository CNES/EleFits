// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FileMemSegments.h"

namespace Fits {

FileMemSegments::FileMemSegments(const Segment& fileSegment, Linx::Index memoryIndex) :
    m_file(fileSegment), m_memory(Segment::fromSize(memoryIndex, fileSegment.size()))
{
  if (m_file.back == -1) {
    m_memory.back = -2;
  }
}

FileMemSegments::FileMemSegments(Linx::Index fileIndex, const Segment& memorySegment) :
    m_file(Segment::fromSize(fileIndex, memorySegment.size())), m_memory(memorySegment)
{
  if (m_memory.back == -1) {
    m_file.back = -2;
  }
}

const Segment& FileMemSegments::file() const
{
  return m_file;
}

const Segment& FileMemSegments::memory() const
{
  return m_memory;
}

Linx::Index FileMemSegments::size() const
{
  return m_file.size(); // FIXME resolve
}

void FileMemSegments::resolve(Linx::Index fileBack, Linx::Index memoryBack)
{
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

Linx::Index FileMemSegments::fileToMemory() const
{
  return m_memory.front - m_file.front;
}

Linx::Index FileMemSegments::memoryToFile() const
{
  return m_file.front - m_memory.front;
}

} // namespace Fits
