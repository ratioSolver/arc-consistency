#pragma once

#include "lit.hpp"
#include <vector>

namespace arc_consistency
{
  class solver;

  class constraint
  {
  public:
    constraint(solver &slv) noexcept : slv(slv) {}
    virtual ~constraint() = default;

    virtual bool propagate(utils::var v) noexcept = 0;

    virtual std::string to_string() const noexcept = 0;

  protected:
    solver &slv;
  };

  class clause final : public constraint
  {
  public:
    clause(solver &slv, std::vector<utils::lit> &&lits) noexcept;

    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    std::vector<utils::lit> lits;
  };

  class eq final : public constraint
  {
  public:
    eq(solver &slv, utils::var var1, utils::var var2) noexcept;

    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var var1;
    utils::var var2;
  };

  class neq final : public constraint
  {
  public:
    neq(solver &slv, utils::var var1, utils::var var2) noexcept;

    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var var1;
    utils::var var2;
  };
} // namespace arc_consistency
