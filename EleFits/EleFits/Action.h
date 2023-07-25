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
 * @brief Base class for strategy actions.
 * 
 * To implement custom actions, just override one or several methods of this class.
 * Early return can be used to skip some HDUs or cases, e.g:
 * 
 * \code
 * void afterAccessing(const Hdu& hdu) override {
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
  Action() = default;

  /**
   * @brief Action performed just after openning the file.
   */
  virtual void afterOpening(const Hdu&) {}

  /**
   * @brief Action performed just after accessing the HDU for the first time.
   */
  virtual void afterAccessing(const Hdu&) {}

  /**
   * @brief Action performed just before closing the file.
   */
  virtual void beforeClosing(const Hdu&) {}
};

/**
 * @brief An action which verifies existing checksums.
 */
class VerifyChecksums : public Action {

public:
  ELEFITS_VIRTUAL_DTOR(VerifyChecksums)
  ELEFITS_COPYABLE(VerifyChecksums)
  ELEFITS_MOVABLE(VerifyChecksums)
  VerifyChecksums() = default;

  /**
   * @brief Verify the HDU checksums at first access, throw if incorrect.
   */
  void afterAccessing(const Hdu& hdu) override {
    try {
      hdu.verifyChecksums();
    } catch (ChecksumError& e) {
      if (e.incorrect()) {
        throw e;
      }
    }
  }
};

/**
 * @brief An action which updates existing checksums of edited HDUs.
 */
class UpdateChecksums : public Action {

public:
  ELEFITS_VIRTUAL_DTOR(UpdateChecksums)
  ELEFITS_COPYABLE(UpdateChecksums)
  ELEFITS_MOVABLE(UpdateChecksums)
  UpdateChecksums() = default;

  /**
   * @brief If the HDU was edited, update its checksums before closing.
  */
  void beforeClosing(const Hdu& hdu) override {
    if (not hdu.matches(HduCategory::Edited)) {
      return;
    }
    if (hdu.header().has("CHECKSUM") || hdu.header().has("DATASUM")) {
      hdu.updateChecksums();
    }
  }
};

/**
 * @brief An action which cites EleFits in the Primary header as a HISTORY record.
 */
class CiteEleFits : public Action {

public:
  ELEFITS_VIRTUAL_DTOR(CiteEleFits)
  ELEFITS_COPYABLE(CiteEleFits)
  ELEFITS_MOVABLE(CiteEleFits)
  CiteEleFits() : m_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) {}

  /**
   * @brief Write a HISTORY record to the Primary header.
   */
  void beforeClosing(const Hdu& hdu) override {
    if (hdu.matches(HduCategory::Primary)) {
      std::string msg = toDateString(m_time);
      msg += " This file was edited by EleFits <github.com/CNES/EleFits>";
      hdu.header().writeHistory(msg);
    }
  }

private:
  static std::string toDateString(const std::time_t& time) {
    char str[std::size("yyyy-mm-dd")];
    std::strftime(std::data(str), std::size(str), "%F", std::gmtime(&time));
    return str;
  }

  static std::string toDateTimeString(const std::time_t& time) {
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
