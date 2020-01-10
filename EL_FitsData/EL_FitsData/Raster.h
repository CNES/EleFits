/**
 * @file EL_FitsData/Raster.h
 * @date 10/21/19
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

#ifndef _EL_FITSDATA_RASTER_H
#define _EL_FITSDATA_RASTER_H

#include <array>
#include <functional>
#include <numeric>
#include <vector>


namespace Euclid {
namespace FitsIO {

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
class Raster {

public:

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
	 * @brief Access the raw data.
	 */
	virtual const std::vector<T>& data() const = 0;

	/**
	 * @brief Length along given axis.
	 */
	template<std::size_t i>
	long length() const;

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
	const T& operator[](const pos_type<n>& pos) const;

	/**
	 * @brief Pixel at given position.
	 */
	T& operator[](const pos_type<n>& pos);

public:

	/**
	 * @brief Raster shape, i.e. length along each axis.
	 */
	pos_type<n> shape;

};


template<typename T, std::size_t n=2>
class SharedRaster : public Raster<T, n> {

public:

	/**
	 * @brief Create a Raster with given shape and values.
	 */
	SharedRaster(pos_type<n> shape, const std::vector<T>& data);

	virtual const std::vector<T>& data() const;

private:

	const std::vector<T>& m_data_ref;

};


template<typename T, std::size_t n=2>
class DataRaster : public Raster<T, n> {

public:

	/**
	 * @brief Create a Raster with given shape and values.
	 */
	DataRaster(pos_type<n> shape, std::vector<T> data);

	virtual const std::vector<T>& data() const;

	/**
	 * @brief Non-const reference to the data, useful to take ownership through move semantics.
	 * @code
	 * std::vector<T> v = std::move(raster.data());
	 * @endcode
	 */
	std::vector<T>& data();

private:

	std::vector<T> m_data;

};


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
Raster<T, n>::Raster(pos_type<n> shape, std::vector<T> data) :
		shape(shape),
		data(data) {

}

template<typename T, std::size_t n>
template<std::size_t i>
inline long Raster<T, n>::length() const {
	return std::get<i>(shape);
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
inline const T& Raster<T, n>::operator[](const pos_type<n>& pos) const {
	return data[index(pos)];
}

template<typename T, std::size_t n>
inline T& Raster<T, n>::operator[](const pos_type<n>& pos) {
	return const_cast<T&>(const_cast<const Raster*>(this)->operator[](pos));
}

}
}

#endif
