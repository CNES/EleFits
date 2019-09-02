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

#include "EL_FitsFile/Hdu.h"

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/RecordWrapper.h"

namespace Euclid {
namespace FitsIO {

Hdu::Hdu(fitsfile* fptr, std::size_t index) :
		m_fptr(fptr), m_index(index) {}

std::size_t Hdu::index() const {
	return m_index;
}

std::string Hdu::name() const {
	goto_this_hdu();
	return Cfitsio::HDU::current_name(m_fptr);
}

void Hdu::rename(std::string name) const {
	goto_this_hdu();
	Cfitsio::HDU::update_name(m_fptr, name);
}

std::string Hdu::read_value(std::string keyword) const {
	goto_this_hdu();
	return Cfitsio::Record::read_value(m_fptr, keyword);
}

void Hdu::goto_this_hdu() const {
	Cfitsio::HDU::goto_index(m_fptr, m_index);
}

}
}
