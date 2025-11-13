#include "constraint.hpp"
#include "arc_consistency.hpp"
#include <unordered_set>
#include <cassert>

namespace arc_consistency
{
    bool constraint::remove(utils::var v, const utils::enum_val &val) noexcept { return slv.remove(v, val, *this); }
    std::unordered_set<const utils::enum_val *> &constraint::domain(utils::var v) const noexcept
    {
        assert(v < slv.dom.size());
        return slv.dom[v];
    }

    assign::assign(solver &slv, utils::var v, utils::enum_val &val) noexcept : constraint(slv), v{v}, val{val} {}

    std::vector<utils::var> assign::scope() const noexcept { return {v}; }

    bool assign::propagate(utils::var) noexcept
    {
        auto &var_dom = domain(v);
        for (auto it = var_dom.begin(); it != var_dom.end();)
            if (*it != &val)
            {
                if (!remove(v, **it))
                    return false;     // Domain wipeout
                it = var_dom.begin(); // Restart iteration after modification
            }
            else
                ++it;
        return true;
    }

    std::string assign::to_string() const noexcept { return "v" + std::to_string(v) + " -> " + (dynamic_cast<const enum_val *>(&val) ? static_cast<const enum_val &>(val).to_string() : "<unknown>"); }

    forbid::forbid(solver &slv, utils::var v, utils::enum_val &val) noexcept : constraint(slv), v{v}, val{val} {}

    std::vector<utils::var> forbid::scope() const noexcept { return {v}; }

    bool forbid::propagate(utils::var) noexcept
    {
        if (domain(v).find(&val) == domain(v).end())
            return true; // Value already not in domain
        return remove(v, val);
    }

    std::string forbid::to_string() const noexcept { return "v" + std::to_string(v) + " != " + (dynamic_cast<const enum_val *>(&val) ? static_cast<const enum_val &>(val).to_string() : "<unknown>"); }

    clause::clause(solver &slv, std::vector<utils::lit> &&lits) noexcept : constraint(slv), lits{std::move(lits)} {}

    std::vector<utils::var> clause::scope() const noexcept
    {
        std::vector<utils::var> scope;
        for (const auto &l : lits)
            scope.push_back(utils::variable(l));
        return scope;
    }

    bool clause::propagate(utils::var) noexcept
    {
        std::size_t unassigned_count = 0;
        utils::lit unassigned_lit;
        for (const auto &l : lits)
            switch (slv.sat_val(l))
            {
            case utils::True:
                return true; // Clause is already satisfied
            case utils::False:
                break; // Literal is false, continue
            case utils::Undefined:
                if (++unassigned_count > 1)
                    return true; // More than one unassigned literal, nothing to do
                unassigned_lit = l;
                break;
            }
        if (unassigned_count == 0)
            return false; // Clause is unsatisfied
        return remove(utils::variable(unassigned_lit), utils::sign(unassigned_lit) ? solver::False : solver::True);
    }

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

    std::vector<utils::var> eq::scope() const noexcept { return {var1, var2}; }

    bool eq::propagate(utils::var v) noexcept
    {
        auto &var_dom = domain(v);
        auto other_var = (v == var1) ? var2 : var1;
        auto other_dom = domain(other_var);
        for (auto &other_val : other_dom)
            if (var_dom.find(other_val) == var_dom.end() && !remove(other_var, *other_val))
                return false; // Domain wipeout

        return true;
    }

    std::string eq::to_string() const noexcept { return "v" + std::to_string(var1) + " = v" + std::to_string(var2); }

    neq::neq(solver &slv, utils::var var1, utils::var var2) noexcept : constraint(slv), var1{var1}, var2{var2} {}

    std::vector<utils::var> neq::scope() const noexcept { return {var1, var2}; }

    bool neq::propagate(utils::var v) noexcept
    {
        auto &var_dom = domain(v);
        auto other_var = (v == var1) ? var2 : var1;
        auto &other_dom = domain(other_var);
        if (var_dom.size() == 1)
        {
            auto sole_val = *var_dom.begin();
            if (other_dom.find(sole_val) != other_dom.end() && !remove(other_var, *sole_val))
                return false; // Domain wipeout
        }
        return true;
    }

    std::string neq::to_string() const noexcept { return "v" + std::to_string(var1) + " ≠ v" + std::to_string(var2); }
} // namespace arc_consistency
