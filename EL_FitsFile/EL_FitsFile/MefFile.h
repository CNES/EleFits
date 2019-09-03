/**
 * @file EL_FitsFile/MefFile.h
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

#ifndef _EL_FITSFILE_MEFFILE_H
#define _EL_FITSFILE_MEFFILE_H

#include "EL_CfitsioWrapper/HduWrapper.h"

#include "EL_FitsFile/FitsFile.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Multi-Extension Fits file reader-writer.
 */
class MefFile : public FitsFile {

public:

	/**
	 * @brief Create a new MefFile with given filename and permission.
	 */
	MefFile(std::string filename, Permission permission);

	virtual ~MefFile() = default;

	/**
	 * @brief Access the Hdu at given index.
	 * @tparam The type of Hdu: ImageHdu, BintableHdu or Hdu to just handle metadata.
	 * @return A reference to the Hdu reader-writer.
	 * @details
	 * The type can be ImageHdu, BintableHdu or unspecified (base class Hdu).
	 * In the latter case, if needs be, the returned Hdu can still be cast to an ImageHdu or BintableHdu,
	 * or merely be used as a metadata reader-writer, e.g.:
	 */
/** @code
auto ext = f.access<>(3);
ext.write_value("KEYWORD", 2.0);
auto image_ext = dynamic_cast<ImageHdu&>(ext);
auto raster = image_ext.read_raster<double>();
@endcode */
	template<class T=Hdu>
	T& access(std::size_t index);

	/**
	 * @brief Access the first Hdu with given name.
	 * @see access
	 */
	template<class T=Hdu>
	T& access_first(std::string name);

	/**
	 * @brief Access the Primary Hdu.
	 * @see access
	 */
	template<class T=Hdu>
	T& access_primary();

	/**
	 * @brief Append an ImageHdu with given shape optional name.
	 * @return A reference to the new ImageHdu reader-writer.
	 * @details
	 * Can be piped with write services, e.g.:
	 */
/** @code
f.append_image_ext("IMAGE").write_raster(raster);
@endcode */
	template<std::size_t n>
	ImageHdu& init_image_ext(Cfitsio::Image::pos_type<n>& shape, std::string name=""); //TODO shape in EL_FitsData

	/**
	 * @brief Append an ImageHdu with given data and optional name.
	 * @see prepare_image_ext
	 */
	template<typename T, std::size_t n>
	ImageHdu& assign_image_ext(Cfitsio::Image::Raster<T, n>& raster, std::string name=""); //TODO idem

protected:

	std::vector<std::unique_ptr<Hdu>> m_hdus;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<class T>
T& MefFile::access(std::size_t index) {
    Cfitsio::HDU::goto_index(m_fptr, index);
    auto hdu_type = Cfitsio::HDU::current_type(m_fptr);
    std::unique_ptr<Hdu> ptr;
    switch (hdu_type) {
    case Cfitsio::HDU::Type::IMAGE:
        ptr.reset(new ImageHdu(m_fptr, index));
        break;
    case Cfitsio::HDU::Type::BINTABLE:
        ptr.reset(new BintableHdu(m_fptr, index));
        break;
    default:
		ptr.reset(new Hdu(m_fptr, index));
        break;
    }
	m_hdus.reserve(index);
    m_hdus.insert(m_hdus.begin() + index-1, std::move(ptr));
    return dynamic_cast<T&>(*m_hdus[index-1].get());
}

template<class T>
T& MefFile::access_first(std::string name) {
    Cfitsio::HDU::goto_name(m_fptr, name);
    return access<T>(Cfitsio::HDU::current_index(m_fptr));
}

template<class T>
T& MefFile::access_primary() {
	return access<T>(1);
}

template<std::size_t n>
ImageHdu& MefFile::init_image_ext(Cfitsio::Image::pos_type<n>& shape, std::string name) {
    Cfitsio::HDU::create_image_extension(m_fptr, name, shape);
	const auto size = m_hdus.size();
	std::unique_ptr<Hdu> ptr(new ImageHdu(m_fptr, size+1));
	m_hdus.push_back(ptr);
	return dynamic_cast<ImageHdu&>(*m_hdus[size].get());
}

template<typename T, std::size_t n>
ImageHdu& MefFile::assign_image_ext(Cfitsio::Image::Raster<T, n>& raster, std::string name) {
    Cfitsio::HDU::create_image_extension(m_fptr, name, raster);
	const auto size = m_hdus.size();
	std::unique_ptr<Hdu> ptr(new ImageHdu(m_fptr, size+1));
	m_hdus.push_back(std::move(ptr));
	return dynamic_cast<ImageHdu&>(*m_hdus[size].get());
}

}
}

#endif
