/**
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

#ifndef _ELECFITSIOWRAPPER_CFITSIOWRAPPER_H
#define _ELECFITSIOWRAPPER_CFITSIOWRAPPER_H

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/CfitsioUtils.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/FileWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"

/**
 * @brief Euclid SGS namespace.
 */
namespace Euclid {

/**
 * @brief Functions to use CFitsIO with a modern C++ API.
 * @see Euclid::Fits for an Object-Oriented API.
 */
namespace Cfitsio {}
} // namespace Euclid

#endif
