#include "constraint.hpp"

namespace arc_consistency
{
    clause::clause(solver &slv, std::vector<utils::lit> &&lits) noexcept : constraint(slv), lits{std::move(lits)} {}

    std::string clause::to_string() const noexcept
    {
        std::string result = "(";
        for (auto it = lits.begin(); it != lits.end(); ++it)
        {
            result += utils::to_string(*it);
            if (it + 1 != lits.end())
                result += " ∨ ";
        }
        result += ")";
        return result;
    }

    eq::eq(solver &slv, utils::var var1, utils::var var2) noexcept : constraint(slv), var1{var1}, var2{var2} {}

    std::string eq::to_string() const noexcept { return "v" + std::to_string(var1) + " = v" + std::to_string(var2); }

    neq::neq(solver &slv, utils::var var1, utils::var var2) noexcept : constraint(slv), var1{var1}, var2{var2} {}

    std::string neq::to_string() const noexcept { return "v" + std::to_string(var1) + " ≠ v" + std::to_string(var2); }
} // namespace arc_consistency
