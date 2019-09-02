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

#include "EL_CfitsioWrapper/RecordWrapper.h" //TODO new module EL_FitsData for Record

namespace Euclid {
namespace FitsIO {

class Hdu {

public:

	template<typename T>
	using Record = Cfitsio::Record::record_type<T>;

	Hdu(fitsfile* file, std::size_t index);

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

protected:

	void goto_this_hdu() const;

	fitsfile* m_fptr;

	std::size_t m_index;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
T Hdu::parse_value(std::string keyword) const {
	goto_this_hdu();
	return Cfitsio::Record::parse_value<T>(m_fptr, keyword);
}

template<typename T>
void Hdu::write_value(std::string keyword, T value) const {
	goto_this_hdu();
	Cfitsio::Record::write_value(m_fptr, keyword, value);
}

template<typename T>
void Hdu::update_value(std::string keyword, T value) const {
	goto_this_hdu();
	Cfitsio::Record::update_value(m_fptr, keyword, value);
}

template<typename T>
Hdu::Record<T> Hdu::parse_record(std::string keyword) const {
	goto_this_hdu();
	Cfitsio::Record::parse_record<T>(m_fptr, keyword);
}

template<typename T>
void Hdu::write_record(const Hdu::Record<T>& record) const {
	goto_this_hdu();
	Cfitsio::Record::write_record(m_fptr, record);
}

}
}

#endif
