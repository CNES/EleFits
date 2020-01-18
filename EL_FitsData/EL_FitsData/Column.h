/**
 * @file EL_FitsData/Column.h
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

#ifndef _EL_FITSDATA_COLUMN_H
#define _EL_FITSDATA_COLUMN_H


#include <string>
#include <tuple>
#include <vector>


namespace Euclid {
namespace FitsIO {

/**
 * @brief Column info, i.e. { name, repeat, unit }
 */
template<typename T>
struct ColumnInfo {

	/**
	 * @brief Column name.
	 */
	std::string name;

	/**
	 * @brief Column unit.
	 */
	std::string unit;

	/**
	 * @brief Repeat count of the column, i.e. number of values per cell.
	 * @warning CFitsIO uses long instead of size_t
	 */
	long repeat;

};


/**
 * @brief Bintable column data and metadata.
 */
template<typename T>
class Column {

public:

	virtual ~Column() = default;
	Column(ColumnInfo<T> info);

	/**
	 * @brief Access the data.
	 */
	virtual const std::vector<T>& data() const = 0;

	/**
	 * @brief Number of elements in the column, i.e. number of rows * repeat count.
	 * @warning For strings, CFitsIO requires nelements to be just the number of rows.
	 */
	std::size_t nelements() const; //TODO long?

	/**
	 * @brief Column metadata.
	 */
	ColumnInfo<T> info;

};


/**
 * @brief Column which references some external data.
 * @details Use it for temporary columns.
 */
template<typename T>
class VecRefColumn : public Column<T> {

public:

	virtual ~VecRefColumn() = default;
	VecRefColumn(const VecRefColumn&) = default;
	VecRefColumn(VecRefColumn&&) = default;
	VecRefColumn& operator=(const VecRefColumn&) = default;
	VecRefColumn& operator=(VecRefColumn&&) = default;

	VecRefColumn(ColumnInfo<T> info, const std::vector<T>& data);

	virtual const std::vector<T>& data() const;

private:

	const std::vector<T>& m_data_ref;

};


/**
 * @brief Column which stores internally the data.
 * @details Use it (via move semantics) if you don't need your data after the write operation.
 */
template<typename T>
class VecColumn : public Column<T> {

public:

	virtual ~VecColumn() = default;
	VecColumn(const VecColumn&) = default;
	VecColumn(VecColumn&&) = default;
	VecColumn& operator=(const VecColumn&) = default;
	VecColumn& operator=(VecColumn&&) = default;

	VecColumn(ColumnInfo<T> info, std::vector<T> data);

	virtual const std::vector<T>& data() const;

	/**
	 * @brief Non-const reference to the data, useful to take ownership through move semantics.
	 * @code
	 * std::vector<T> v = std::move(column.data());
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

/**
 * Helper function to specialize nelements for strings.
 */
template<typename T>
std::size_t column_nelements(const Column<T>& column);



/**
 * In general, nelements is the number of values,
 * i.e. number of rows * repeat count.
 */
template<typename T>
inline std::size_t column_nelements(const Column<T>& column) {
	return column.info.repeat * column.data().size();
}

/**
 * For strings, nelements is the number of rows.
 */
template<>
inline std::size_t column_nelements<std::string>(const Column<std::string>& column) {
	return column.data().size();
}

}

/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Column<T>::Column(ColumnInfo<T> info) :
		info(info) {
}


template<typename T>
std::size_t Column<T>::nelements() const {
	return internal::column_nelements(*this);
}


template<typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> info, const std::vector<T>& data) :
		Column<T>(info),
		m_data_ref(data) {
}

template<typename T>
const std::vector<T>& VecRefColumn<T>::data() const {
	return m_data_ref;
}


template<typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> info, std::vector<T> data) :
		Column<T>(info),
		m_data(data) {
}

template<typename T>
const std::vector<T>& VecColumn<T>::data() const {
	return m_data;
}

template<typename T>
std::vector<T>& VecColumn<T>::data() {
	return m_data;
}


}
}

#endif
