#pragma once

#include "var.hpp"
#include "constraint.hpp"
#include <functional>

namespace arc_consistency
{
  class solver
  {
  public:
    [[nodiscard]] utils::var new_sat() noexcept;
    [[nodiscard]] utils::var new_var(const std::vector<std::reference_wrapper<const utils::enum_val>> &domain) noexcept;

  private:
    std::vector<var> vars; // index is the variable id
  };
} // namespace arc_consistency
