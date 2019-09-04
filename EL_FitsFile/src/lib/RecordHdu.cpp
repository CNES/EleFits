/**
 * @file src/lib/Hdu.cpp
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

#include "EL_FitsFile/RecordHdu.h"

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace FitsIO {

RecordHdu::RecordHdu(fitsfile* fptr, std::size_t index) :
		m_fptr(fptr), m_index(index) {}

std::size_t RecordHdu::index() const {
	return m_index;
}

std::string RecordHdu::name() const {
	goto_this_hdu();
	return Cfitsio::Hdu::current_name(m_fptr);
}

void RecordHdu::rename(std::string name) const {
	goto_this_hdu();
	Cfitsio::Hdu::update_name(m_fptr, name);
}

void RecordHdu::delete_record(std::string keyword) const {
	goto_this_hdu();
	Cfitsio::Header::delete_record(m_fptr, keyword);
}

void RecordHdu::goto_this_hdu() const {
	Cfitsio::Hdu::goto_index(m_fptr, m_index);
}

}
}
