// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_ACTION_H
#define _ELEFITS_ACTION_H

#include "EleFits/Hdu.h"

#include <chrono>

namespace Euclid {
namespace Fits {

/**
 * @ingroup strategy
 * @brief Base class for strategy actions.
 * 
 * To implement custom actions, just override one or several methods of this class.
 * Early return can be used to skip some HDUs or cases, e.g:
 * 
 * \code
 * void accessed(const Hdu& hdu) override {
 *   if (hdu.matches(HduCategory::Primary || HduCategory::Metadata)) {
 *     return;
 *   }
 *   ...
 * }
 * \endcode
 */
class Action {
public:
  ELEFITS_VIRTUAL_DTOR(Action)
  ELEFITS_COPYABLE(Action)
  ELEFITS_MOVABLE(Action)

  /**
   * @brief Constructor.
  */
  Action() = default;

  /**
   * @brief Method called just after openning the file.
   * 
   * At that time, for new files, the Primary HDU exists.
   */
  virtual void opened(const Hdu&) {}

  /**
   * @brief Method called just after accessing an HDU for the first time.
   * 
   * @warning Created HDUs are not considered, but copied HDUs are.
   */
  virtual void accessed(const Hdu&) {}

  // FIXME copying() applies to the source HDU and copied() to the new HDU?
  // E.g. copying() would call accessed() while copied() would call created()

  /**
   * @brief Method called just after creating an HDU.
   * 
   * @warning Copied HDUs are not considered.
   */
  virtual void created(const Hdu&) {}

  /**
   * @brief Method called just before closing the file.
   */
  virtual void closing(const Hdu&) {}
};

/**
 * @ingroup strategy
 * @brief Checksum update policy of `VerifyChecksums`.
 */
enum class UpdateChecksums {
  None, ///< Do not update checksums
  Outdated, ///< Update checksums of edited HDUs if they exist
  EditedHdu, ///< Update or write missing checksums in edited HDUs
  AnyHdu ///< Update checksums in edited HDUs and write missing checksums in all HDUs
};

/**
 * @ingroup strategy
 * @brief An action which verifies and possibly updates existing checksums.
 * 
 * Just after having accessed an HDU for the first time, its checksums are verified, if any.
 * Before closing the file, checksums of edited HDUs are updated according to the update policy.
 * 
 * @see `UpdateChecksums`
 */
class VerifyChecksums : public Action {

public:
  ELEFITS_VIRTUAL_DTOR(VerifyChecksums)
  ELEFITS_COPYABLE(VerifyChecksums)
  ELEFITS_MOVABLE(VerifyChecksums)

  /**
   * @brief Constructor.
   */
  VerifyChecksums(UpdateChecksums mode = UpdateChecksums::Outdated) : m_mode(mode) {}

  /**
   * @brief Verify the HDU checksums at first access, throw if incorrect.
   */
  void accessed(const Hdu& hdu) override {
    try {
      hdu.verify_checksums();
    } catch (ChecksumError& e) {
      if (e.incorrect()) {
        throw e;
      }
    }
  }

  /**
   * @brief If the HDU was edited, update its checksums before closing.
   */
  void closing(const Hdu& hdu) override {
    switch (m_mode) {
      case UpdateChecksums::None:
        return;
      case UpdateChecksums::Outdated:
        if (edited(hdu) && has_checksums(hdu)) {
          hdu.update_checksums();
        }
        return;
      case UpdateChecksums::EditedHdu:
        if (edited(hdu)) {
          hdu.update_checksums();
        }
        return;
      case UpdateChecksums::AnyHdu:
        if (edited(hdu) || not has_checksums(hdu)) {
          hdu.update_checksums();
        }
        return;
    }
  }

  bool edited(const Hdu& hdu) const {
    return hdu.matches(HduCategory::Edited);
  }

  bool has_checksums(const Hdu& hdu) const {
    return hdu.header().has("CHECKSUM") || hdu.header().has("DATASUM");
  }

private:
  UpdateChecksums m_mode;
};

/**
 * @ingroup strategy
 * @brief An action which cites EleFits in the Primary header as a HISTORY record.
 */
class CiteEleFits : public Action {

public:
  ELEFITS_VIRTUAL_DTOR(CiteEleFits)
  ELEFITS_COPYABLE(CiteEleFits)
  ELEFITS_MOVABLE(CiteEleFits)

  /**
   * @brief Constructor.
   */
  CiteEleFits() : m_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) {}

  /**
   * @brief Write a HISTORY record to the Primary header.
   */
  void closing(const Hdu& hdu) override {
    if (hdu.index() == 0) {
      std::string msg = date_to_string(m_time);
      msg += " This file was edited by EleFits <github.com/CNES/EleFits>";
      hdu.header().write_history(msg);
    }
  }

private:
  static std::string date_to_string(const std::time_t& time) {
    char str[std::size("yyyy-mm-dd")];
    std::strftime(std::data(str), std::size(str), "%F", std::gmtime(&time));
    return str;
  }

  static std::string datetime_to_string(const std::time_t& time) {
    char str[std::size("yyyy-mm-ddThh:mm:ss")];
    std::strftime(std::data(str), std::size(str), "%FT%T", std::gmtime(&time));
    return str;
  }

  /**
   * @brief The time to use in the record.
   */
  std::time_t m_time;
};

} // namespace Fits
} // namespace Euclid

#endif
