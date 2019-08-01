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
	
	~MinimalFile();
	
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

class SmallTable {

public:

	using id_t = int;
	using radec_t = std::complex<float>;
	using name_t = std::string;

	SmallTable();
	
	std::string extname;
	
	std::vector<id_t> ids;
	std::vector<radec_t> radecs;
	std::vector<name_t> names;
	
	Bintable::Column<id_t> id_col;
	Bintable::Column<radec_t> radec_col;
	Bintable::Column<name_t> name_col;

};

template<typename T>
class SmallScalarColumn : public Bintable::Column<T> {

public:

	SmallScalarColumn();
	virtual ~SmallScalarColumn() = default;

};

class SmallStringColumn : public Bintable::Column<std::string> {

public:

	SmallStringColumn();
	virtual ~SmallStringColumn() = default;

};

template<typename T>
class SmallVectorColumn : public Bintable::Column<std::vector<T>> {

public:

	SmallVectorColumn();
	virtual ~SmallVectorColumn() = default;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
SmallScalarColumn<T>::SmallScalarColumn() :
	Bintable::Column<T> { "SCALAR", 1, "m", { T(0), T(1), T(2) } } {
}

SmallStringColumn::SmallStringColumn() :
	Bintable::Column<std::string> { "STRING", 4, "m", { "A", "AB", "ABC" } } {
}


}
}

#endif

