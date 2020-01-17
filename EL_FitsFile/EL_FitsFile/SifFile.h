/**
 * @file EL_FitsFile/SifFile.h
 * @date 08/30/19
 * @author user
 *
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

#ifndef _EL_FITSFILE_SIFFILE_H
#define _EL_FITSFILE_SIFFILE_H

#include "EL_FitsFile/FitsFile.h"
#include "EL_FitsFile/ImageHdu.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Single image Fits file handler.
 */
class SifFile : public FitsFile {

public:

	using FitsFile::Permission;

	SifFile(std::string filename, Permission permission);

	virtual ~SifFile() = default;

	const ImageHdu& hdu() const;

private:

	ImageHdu m_hdu;

};

}
}

#endif
