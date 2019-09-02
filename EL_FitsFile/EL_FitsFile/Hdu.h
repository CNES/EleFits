/**
 * @file EL_FitsFile/Hdu.h
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

#ifndef _EL_FITSFILE_HDU_H
#define _EL_FITSFILE_HDU_H

#include "EL_CfitsioWrapper/RecordWrapper.h"
#include "EL_FitsFile/FitsFile.h"

namespace Euclid {
namespace FitsIO {

class Hdu {

public:

	template<typename T>
	using Record = Cfitsio::Record::record_type<T>;

	Hdu(FitsFile& file, std::size_t index);

	virtual ~Hdu() = default;

	std::size_t index() const;

	std::string name() const;

	void rename(std::string) const;

	std::string read_value(std::string keyword) const;

	template<typename T>
	T parse_value(std::string keyword) const;

	template<typename T>
	void write_value(std::string keyword, T value) const;

	template<typename T>
	void update_value(std::string keyword, T value) const;

	template<typename T>
	Record<T> parse_record(std::string keyword) const;

	template<typename T>
	void write_record(const Record<T>& record) const;

	template<typename T>
	void update_record(const Record<T>& record) const;

protected:

	FitsFile& m_file;

	std::size_t m_index;

};

}
}

#endif
