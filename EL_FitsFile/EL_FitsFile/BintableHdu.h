/**
 * @file EL_FitsFile/BintableHdu.h
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

#ifndef _EL_FITSFILE_BINTABLEHDU_H
#define _EL_FITSFILE_BINTABLEHDU_H

#include "EL_CfitsioWrapper/BintableWrapper.h" // TODO New module EL_FitsData for Column
#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

class BintableHdu : public RecordHdu {

public:

	template<typename T>
	using Column = Cfitsio::Bintable::Column<T>;

	BintableHdu(fitsfile* fptr, std::size_t index);

	virtual ~BintableHdu() = default;

	/**
	 * @brief Read a Column with given name.
	 */
	template<typename T>
	Column<T> read_column(std::string name) const;

	/**
	 * @brief Write a Column.
	 */
	template<typename T>
	void write_column(const Column<T>& column) const;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
BintableHdu::Column<T> BintableHdu::read_column(std::string name) const {
	return Cfitsio::Bintable::read_column<T>(m_fptr, name);
}

template<typename T>
void BintableHdu::write_column(const BintableHdu::Column<T>& column) const {
	Cfitsio::Bintable::write_column(m_fptr, column);
}

}
}

#endif
