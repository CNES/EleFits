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
 * @ingroup strategy
 * @brief MEF file strategy.
 * 
 * A strategy is a list of actions to be performed at various steps of the MEF file lifetime.
 * Actions generally act on HDUs according to their category.
 * 
 * A specific component of the strategy is the compression strategy.
 * In this case, compression actions are not performed one after the other:
 * instead, they are tried one after the other, and the iteration stops as soon as a suitable compression action is found.
 * If none is suitable, then compression is disabled. 
 */
class Strategy {
  friend class MefFile;

public:

  /// @group_modifiers

  /**
   * @brief Append an action.
   * 
   * If the action is some compression algorithm, it is wrapped into a compression action.
   * Then, if the action is some compression action, then it is appended as a fallback of the compression strategy.
   */
  template <typename TAction>
  void push_back(TAction&& action)
  {
    using Decay = std::decay_t<TAction>;
    if constexpr (std::is_same_v<Strategy, Decay>) {
      for (auto&& e : action.m_compression) {
        m_compression.push_back(std::move(e));
      }
      for (auto&& e : action.m_actions) {
        m_actions.push_back(std::move(e));
      }
    } else if constexpr (std::is_base_of_v<Compression, Decay>) {
      m_compression.push_back(std::make_unique<Compress<Decay>>(std::forward<TAction>(action)));
    } else if constexpr (std::is_base_of_v<CompressionAction, Decay>) {
      m_compression.push_back(std::make_unique<Decay>(std::forward<TAction>(action)));
    } else {
      m_actions.push_back(std::make_unique<Decay>(std::forward<TAction>(action)));
    }
  }

  /**
   * @brief Append actions.
   * 
   * If the method is given a strategy, then each of its actions are appended.
   */
  template <typename TAction0, typename... TActions>
  void append(TAction0&& action0, TActions&&... actions)
  {
    push_back(std::forward<TAction0>(action0));
    if constexpr (sizeof...(TActions) > 0) {
      append(std::forward<TActions>(actions)...);
    }
  }

  /**
   * @brief Clear the strategy.
   */
  Strategy& clear()
  {
    m_compression.clear();
    m_actions.clear();
    return *this;
  }

  /// @group_operations

  /**
   * @copydoc Action::opened
   */
  void opened(const Hdu& hdu)
  {
    for (auto& a : m_actions) {
      a->opened(hdu);
    }
  }

  /**
   * @copydoc Action::accessed
   */
  void accessed(const Hdu& hdu)
  {
    for (auto& a : m_actions) {
      a->accessed(hdu);
    }
  }

  /**
   * @copydoc Action::created
   */
  void created(const Hdu& hdu)
  {
    for (auto& a : m_actions) {
      a->created(hdu);
    }
  }

  /**
   * @copydoc Action::closing
   */
  void closing(const Hdu& hdu)
  {
    for (auto& a : m_actions) {
      a->closing(hdu);
    }
  }

  /// @}

private:

  /**
   * @brief Compress according to the strategy.
   */
  template <typename T>
  bool compress(fitsfile* fptr, const ImageHdu::Initializer<T>& init)
  {
    for (const auto& c : m_compression) {
      if ((*c)(fptr, init)) {
        return true;
      }
    }
    Compress<NoCompression>()(fptr, init);
    return false;
  }

  /**
   * @brief The compression strategy.
   */
  std::vector<std::unique_ptr<CompressionAction>> m_compression;

  /**
   * @brief The actions.
   */
  std::vector<std::unique_ptr<Action>> m_actions;
};

} // namespace Fits
} // namespace Euclid

#endif
