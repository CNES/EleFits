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

#ifndef _EL_FITSIO_EXAMPLES_OODUNIVERSE_H
#define _EL_FITSIO_EXAMPLES_OODUNIVERSE_H

#include <string>
#include <vector>

#include "EL_FitsData/Raster.h"

namespace Euclid {
namespace FitsIO {
namespace ObjectOriented {

struct Source {

  int ra, dec;
  VecRaster<float> thumbnail;

};

class Universe {

public:

  Universe() {}

  void random(std::size_t count, long width, long height);

  void load(std::string filename);

  void append(Source source);

  const std::vector<Source>& sources() const;

private:

  std::vector<Source> m_sources;

};

}
}
}

#endif
