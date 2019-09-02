/**
 * @file EL_FitsFile/FitsFile.h
 * @date 07/23/19
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

#ifndef _EL_FITSFILE_FITSFILE_H
#define _EL_FITSFILE_FITSFILE_H

#include <cfitsio/fitsio.h>
#include <string>

#include "EL_FitsFile/Hdu.h"
#include "EL_FitsFile/ImageHdu.h"
#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {

/**
 * @brief Wrapper classes to handle Fits files and their contents.
 * @details The classes themeselves do not store any data;
 * They just provide IO services, e.g.:
 * @code
 * FitsFile f("file.fits");
 * auto raster = f.image_hdu("IMAGE").read_raster<float>();
 * auto column = f.bintable_hdu("BINTABLE").read_column<std::string>("COL");
 * @endcode
 * instantiates an image raster and a vector which are neither owned nor referenced by f.
 */
namespace FitsIO {

/**
 * @brief Fits file handler.
 */
class FitsFile {

public:

	enum class Permission {
		READ,
		EDIT,
		CREATE,
		OVERWRITE
	};

	/**
	 * @brief Create a new Fits file handler with given filename and permission.
	 */
	FitsFile(std::string filename, Permission permission);

	/**
	 * @brief Destroy the FitsFile handler.
	 */
	virtual ~FitsFile();

	/**
	 * @brief Open a Fits file.
	 */
	void open(std::string filename, Permission permission);

	/**
	 * @brief Close the Fits file.
	 */
	void close();

	/**
	 * @brief Close and delete the Fits file.
	 */
	void close_and_delete();

	/**
	 * @brief Handle the Hdu of unspecified type with given index.
	 * @details
	 * If needs be, the returned Hdu can still be cast to ImageHdu& or BintableHdu&,
	 * or merely be used as a Record reader-writer, e.g.:
	 * @code
	 * auto ext = f.hdu(3);
	 * ext.write_value<float>("KEYWORD", 2);
	 * auto image_ext = dynamic_cast<ImageHdu&> ext;
	 * image_ext.read_raster<double>();
	 * @endcode
	 */
	Hdu& hdu(std::size_t index);

	/**
	 * @brief Handle the Hdu of unspecified type with given name.
	 * @see hdu(std::size_t)
	 */
	Hdu& hdu(std::string name);

	/**
	 * @brief Handle the ImageHdu with given index.
	 */
	ImageHdu& image_hdu(std::size_t index);

	/**
	 * @brief Handle the ImageHdu with given name.
	 */
	ImageHdu& image_hdu(std::string name);

	/**
	 * @brief Append an ImageHdu with optional name.
	 */
	ImageHdu& append_image_hdu(std::string name="");

	/**
	 * @brief Handle the BintableHdu with given index.
	 */
	BintableHdu& bintable_hdu(std::size_t index);

	/**
	 * @brief Handle the BintableHdu with given name.
	 */
	BintableHdu& bintable_hdu(std::string name);

	/**
	 * @brief Append a bintable extension with optional name.
	 */
	BintableHdu& append_bintable_hdu(std::string name="");

private:

	fitsfile* m_fptr;

	std::vector<std::unique_ptr<Hdu>> m_hdus;

};

}
}

#endif
