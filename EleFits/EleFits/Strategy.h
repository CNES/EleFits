// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_STRATEGY_H
#define _ELEFITS_STRATEGY_H

#include "EleFits/Action.h"
#include "EleFits/CompressionStrategy.h"

#include <memory>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @brief MEF file strategy.
 * 
 * A strategy is a list of actions to be performed at various steps of the MEF file lifetime.
 * Actions generally act on HDUs according to their category.
 * 
 * A specific component of the strategy is the compression strategy.
 * In this case, compression actions are not performed one after the other:
 * instead, they are tries one after the other, and the iteration stops as soon as a suitable compression action is found.
 * If none is suitable, then compression is disabled. 
 */
class Strategy {
  friend class MefFile;

public:
  /**
   * @brief Constructor.
   */
  Strategy() : m_compression(), m_actions() {}

  /**
   * @brief Append an action.
   * 
   * If the action is some compression algorithm, it is wrapped into a compression action.
   * Then, if the action is some compression action, then it is appended as a fallback of the compression strategy.
   */
  template <typename TAction>
  void append(TAction&& action) {
    if constexpr (std::is_base_of_v<Compression, std::decay_t<TAction>>) {
      m_compression.push_back(std::make_unique<Compress<TAction>>(std::forward<TAction>(action)));
    } else if constexpr (std::is_base_of_v<CompressionStrategy, std::decay_t<TAction>>) {
      m_compression.push_back(std::make_unique<TAction>(std::forward<TAction>(action)));
    } else {
      m_actions.push_back(std::make_unique<Action>(std::forward<TAction>(action)));
    }
  }

  void clear() {
    m_compression.clear();
  }

private:
  /**
   * @brief Compress according to the strategy.
   */
  template <typename T>
  bool compress(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {
    for (const auto& c : m_compression) {
      if (c->visit(fptr, init)) {
        return true;
      }
    }
    Compress<NoCompression>()(fptr, init);
    return false;
  }

  /**
   * @brief The compression strategy.
   */
  std::vector<std::unique_ptr<CompressionStrategy>> m_compression;

  /**
   * @brief The actions.
   */
  std::vector<std::unique_ptr<Action>> m_actions;
};

} // namespace Fits
} // namespace Euclid

#endif
