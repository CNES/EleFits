// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_HEADERWRAPPER_H
#define _ELECFITSIOWRAPPER_HEADERWRAPPER_H

#include "EleFitsData/KeywordCategory.h"
#include "EleFitsData/Record.h"
#include "EleFitsData/RecordVec.h"

#include <fitsio.h>
#include <string>
#include <tuple>
#include <typeinfo> // type_info
#include <vector>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Header-related functions.
 */
namespace HeaderIo {

/**
 * @brief Read the whole header as a string.
 * @param fptr A pointer to the fitsfile object.
 * @param inc_non_valued Include non-valued records (COMMENT, HISTORY, blank).
 */
std::string read_header(fitsfile* fptr, bool inc_non_valued = true);

/**
 * @brief List the keywords of selected categories.
 */
std::vector<std::string> list_keywords(fitsfile* fptr, Fits::KeywordCategory categories = Fits::KeywordCategory::All);

/**
 * @brief List the keywords of selected categories, as well as their values.
 */
std::map<std::string, std::string>
list_keywords_values(fitsfile* fptr, Fits::KeywordCategory categories = Fits::KeywordCategory::All);

/**
 * @brief Check whether the current HDU contains a given keyword.
 */
bool has_keyword(fitsfile* fptr, const std::string& keyword);

/**
 * @brief Parse a record.
 */
template <typename T>
Fits::Record<T> parse_record(fitsfile* fptr, const std::string& keyword);

/**
 * @brief Parse records.
 */
template <typename... Ts>
std::tuple<Fits::Record<Ts>...> parse_records(fitsfile* fptr, const std::vector<std::string>& keywords);

/**
 * @brief Parse records and store them in a user-defined structure.
 * @tparam TReturn A class which can be brace-initialized with a pack of records or values.
 */
template <class TReturn, typename... Ts>
TReturn parse_records_as(fitsfile* fptr, const std::vector<std::string>& keywords);

/**
 * @brief Parse homogeneous records and store them in a vector.
 */
template <typename T>
Fits::RecordVec<T> parse_record_vec(fitsfile* fptr, const std::vector<std::string>& keywords);

/**
 * @brief Write a new record.
 */
template <typename T>
void write_record(fitsfile* fptr, const Fits::Record<T>& record);

/**
 * @brief Write new records.
 */
template <typename... Ts>
void write_records(fitsfile* fptr, const Fits::Record<Ts>&... records);

/**
 * @brief Write new records.
 */
template <typename... Ts>
void write_records(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records);

/**
 * @brief Write homogeneous records.
 */
template <typename T>
void write_records(fitsfile* fptr, const std::vector<Fits::Record<T>>& records);

/**
 * @brief Update an existing record or write a new one.
 */
template <typename T>
void update_record(fitsfile* fptr, const Fits::Record<T>& record);

/**
 * @brief Update existing records or write new ones.
 */
template <typename... Ts>
void update_records(fitsfile* fptr, const Fits::Record<Ts>&... records);

/**
 * @brief Update existing records or write new ones.
 */
template <typename... Ts>
void update_records(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records);

/**
 * @brief Update existing homogeneous records or write new ones.
 */
template <typename T>
void update_records(fitsfile* fptr, const std::vector<Fits::Record<T>>& records);

/**
 * @brief Delete an existing record.
 */
void remove_record(fitsfile* fptr, const std::string& keyword);

/**
 * @brief Get the typeid of a record value.
 */
const std::type_info& record_typeid(fitsfile* fptr, const std::string& keyword);

/**
 * @brief Write COMMENT record.
 */
void write_comment(fitsfile* fptr, const std::string& comment);

/**
 * @brief Write HISTORY record.
 */
void write_history(fitsfile* fptr, const std::string& history);

} // namespace HeaderIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_HEADERWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/HeaderWrapper.hpp"
#undef _ELECFITSIOWRAPPER_HEADERWRAPPER_IMPL
/// @endcond

#endif
