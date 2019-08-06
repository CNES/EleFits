/**
 * @file EL_CFitsIOWrapper/ImageWrapper.h
 * @date 07/25/19
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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include <array>
#include <cfitsio/fitsio.h>
#include <functional>
#include <numeric>
#include <string>
#include <vector>

#include "EL_CFitsIOWrapper/ErrorWrapper.h"
#include "EL_CFitsIOWrapper/FileWrapper.h"
#include "EL_CFitsIOWrapper/TypeWrapper.h"


namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Type for a coordinate along one axis.
 * 
 * Would be nice to use std::array<T, n>::std::size_t
 * but not compliant with CFitsIO which uses long for subscripts
 */
using coord_type = long;

/**
 * @brief Type for a position or shape, i.e. set of coordinates.
 */
template<std::size_t n>
using pos_type = std::array<coord_type, n>;


/**
 * @brief Raster of a n-dimensional image.
 * 
 * 2D by default.
 */
template<typename T, std::size_t n=2>
struct Raster {

	/**
	 * @brief Create a Raster with given shape.
	 */
	Raster(pos_type<n> shape);

	/**
	 * @brief Create an empty Raster.
	 */
	Raster() = default;
	
	/**
	 * @brief Destructor.
	 */
	virtual ~Raster() = default;

	/**
	 * @brief Raster shape, i.e. length along each axis.
	 */
	pos_type<n> shape;

	/**
	 * @brief Raw data.
	 */
	std::vector<T> data;

	/**
	 * @brief Number of pixels.
	 */
	std::size_t size() const;

	/**
	 * @brief Raw index of a position.
	 */
	std::size_t index(const pos_type<n>& pos) const;

	/**
	 * @brief Pixel at given position.
	 */
	const T& operator()(const pos_type<n>& pos) const;

	/**
	 * @brief Pixel at given position.
	 */
	T& operator()(const pos_type<n>& pos);

};

/**
 * @brief Read a Raster in current Image HDU.
 */
template<typename T, std::size_t n=2>
Raster<T, n> read_raster(fitsfile* fptr);

/**
 * @brief Write a Raster in current Image HDU.
 */
template<typename T, std::size_t n=2>
void write_raster(fitsfile* fptr, const Raster<T, n>& raster);


///////////////
// INTERNAL //
/////////////


/// @cond INTERNAL
namespace internal {

template<std::size_t i>
struct Index {
	template<std::size_t n>
	static std::size_t offset(const pos_type<n>& shape, const pos_type<n>& pos);
};

template<std::size_t i>
template<std::size_t n>
inline std::size_t Index<i>::offset(const pos_type<n>& shape, const pos_type<n>& pos) {
	return std::get<n-1-i>(pos) + std::get<n-1-i>(shape) * Index<i-1>::offset(shape, pos);
}

template<>
template<std::size_t n>
inline std::size_t Index<0>::offset(const pos_type<n>& shape, const pos_type<n>& pos) {
	return std::get<n-1>(pos);
}

}
/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
Raster<T, n>::Raster(pos_type<n> shape) :
		shape(shape),
		data(size()) {
}

template<typename T, std::size_t n>
inline std::size_t Raster<T, n>::size() const {
	return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<std::size_t>());
}

template<typename T, std::size_t n>
inline std::size_t Raster<T, n>::index(const pos_type<n>& pos) const {
	return internal::Index<n-1>::offset(shape, pos);
}

template<typename T, std::size_t n>
inline const T& Raster<T, n>::operator()(const pos_type<n>& pos) const {
	return data[index(pos)];
}

template<typename T, std::size_t n>
inline T& Raster<T, n>::operator()(const pos_type<n>& pos) {
	return const_cast<T&>(const_cast<const Raster*>(this)->operator()(pos));
}

template<typename T, std::size_t n>
Raster<T, n> read_raster(fitsfile* fptr) {
	Raster<T, n> raster;
	int status = 0;
	fits_get_img_size(fptr, n, &raster.shape[0], &status);
	may_throw_cfitsio_error(status);
	const auto size = raster.size();
	raster.data.resize(size);
	fits_read_img(fptr, TypeCode<T>::for_image(), 1, size, nullptr, &raster.data.data()[0], nullptr, &status);
	// Number 1 is a 1-base offset (so we read the whole raster here)
	may_throw_cfitsio_error(status);
	return raster;
}

template<typename T, std::size_t n>
void write_raster(fitsfile* fptr, const Raster<T, n>& raster) {
	may_throw_readonly_error(fptr);
	int status = 0;
	std::vector<T> nonconst_data = raster.data; //TODO const-correctness issue?
	fits_write_img(fptr, TypeCode<T>::for_image(), 1, raster.size(), nonconst_data.data(), &status);
	may_throw_cfitsio_error(status);
}

}
}


#endif
