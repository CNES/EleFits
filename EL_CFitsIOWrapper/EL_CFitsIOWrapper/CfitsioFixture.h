/**
 * @file EL_CFitsIOWrapper/CfitsioFixture.h
 * @date 07/31/19
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

#ifndef _EL_CFITSIOWRAPPER_CFITSIOFIXTURE_H
#define _EL_CFITSIOWRAPPER_CFITSIOFIXTURE_H

#include "ElementsKernel/Temporary.h"

#include "FileWrapper.h"
#include "HduWrapper.h"
#include "ImageWrapper.h"

namespace Cfitsio {
namespace Test {

class MinimalFile {

protected:

	Elements::TempFile tmp;

public:

	MinimalFile();
	
	std::string filename;
	
	fitsfile* fptr;

};

class SmallRaster : public Image::Raster<float> {

public:

	SmallRaster(long width=3, long height=2);
	
	virtual ~SmallRaster() = default;
	
	bool approx(const Image::Raster<float>& other, float tol=0.1) const;
	
	long width, height;

};

}
}


#endif
