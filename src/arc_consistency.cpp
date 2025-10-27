#include "arc_consistency.hpp"
#include "logging.hpp"
#include <cassert>

namespace arc_consistency
{
    bool_val solver::True{true};
    bool_val solver::False{false};

    utils::var solver::new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept
    {
        const auto x = init_domain.size();
        std::unordered_set<utils::enum_val *> domain_set;
        for (const auto &ev_ref : domain)
            domain_set.insert(&ev_ref.get());
        init_domain.emplace_back(std::move(domain_set));
        dom.emplace_back(init_domain.back());
        watchlist.emplace_back();
        return x;
    }

    utils::lbool solver::sat_val(const utils::var &x) const noexcept
    {
        assert(x < dom.size());
        if (dom[x].size() == 1)
            return (*dom[x].begin() == &solver::True) ? utils::True : utils::False;
        else
            return utils::Undefined; // variable is unassigned
    }

    utils::lbool solver::sat_val(const utils::lit &l) const noexcept
    {
        switch (sat_val(utils::variable(l)))
        {
        case utils::True:
            return utils::sign(l) ? utils::True : utils::False;
        case utils::False:
            return utils::sign(l) ? utils::False : utils::True;
        default:
            return utils::Undefined;
        }
    }

    const std::vector<std::reference_wrapper<utils::enum_val>> solver::domain(utils::var v) const noexcept
    {
        std::vector<std::reference_wrapper<utils::enum_val>> dom_vec;
        for (auto *ev_ptr : dom.at(v))
            dom_vec.emplace_back(*ev_ptr);
        return dom_vec;
    }

    std::shared_ptr<constraint> solver::new_clause(std::vector<utils::lit> &&lits) noexcept { return std::make_shared<clause>(*this, std::move(lits)); }
    std::shared_ptr<constraint> solver::new_equal(utils::var x, utils::var y) noexcept { return std::make_shared<eq>(*this, x, y); }
    std::shared_ptr<constraint> solver::new_distinct(utils::var x, utils::var y) noexcept { return std::make_shared<neq>(*this, x, y); }
    std::shared_ptr<constraint> solver::new_assign(utils::var x, utils::enum_val &val) noexcept { return std::make_shared<assign>(*this, x, val); }
    std::shared_ptr<constraint> solver::new_forbid(utils::var x, utils::enum_val &val) noexcept { return std::make_shared<forbid>(*this, x, val); }

    void solver::add_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        for (const auto &v : c->scope())
        {
            watchlist.at(v).insert(c);
            to_propagate.push(v);
        }
        constraints.insert(c);
    }

    void solver::remove_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        std::unordered_set<utils::var> visited;
        std::queue<std::shared_ptr<constraint>> to_restore;
        to_restore.push(c);
        while (!to_restore.empty())
        {
            const auto curr = to_restore.front();
            to_restore.pop();
            for (const auto &v : curr->scope())
                if (visited.insert(v).second)
                {
                    dom.at(v) = init_domain.at(v);
                    to_propagate.push(v);
                    for (const auto &c : watchlist.at(v))
                        to_restore.push(c);
                }
        }
        for (const auto &v : c->scope())
            watchlist.at(v).erase(c);
        constraints.erase(c);
    }

    bool solver::propagate() noexcept
    {
        while (!to_propagate.empty())
        {
            const auto v = to_propagate.front();
            to_propagate.pop();
            for (const auto &c : watchlist.at(v))
                if (!c->propagate(v))
                    return false; // Conflict detected
        }
        return true;
    }

    bool solver::remove(utils::var v, utils::enum_val &val) noexcept
    {
        assert(dom.at(v).find(&val) != dom.at(v).end());
        dom.at(v).erase(&val);
        if (dom.at(v).empty())
            return false;
        LOG_TRACE(to_string(*this, v));
        to_propagate.push(v);
        return true;
    }

    std::string to_string(const solver &s) noexcept
    {
        std::string res = "Solver State:\n";
        for (std::size_t i = 0; i < s.dom.size(); ++i)
            res += to_string(s, i) + "\n";
        res += "Constraints:\n";
        for (const auto &c : s.constraints)
            res += c->to_string() + "\n";
        return res;
    }

    std::string to_string(const solver &s, utils::var v) noexcept
    {
        std::string res = "v" + std::to_string(v);
        ;
        if (std::all_of(s.dom.at(v).begin(), s.dom.at(v).end(), [](const utils::enum_val *val)
                        { return dynamic_cast<const enum_val *>(val); }))
        {
            if (s.dom.at(v).size() == 1)
                res += " = " + dynamic_cast<const enum_val *>(*s.dom.at(v).begin())->to_string();
            else
            {
                res += " ∈ { ";
                for (auto it = s.dom.at(v).begin(); it != s.dom.at(v).end(); ++it)
                {
                    res += dynamic_cast<const enum_val *>(*it)->to_string();
                    if (std::next(it) != s.dom.at(v).end())
                        res += ", ";
                }
                res += " }";
            }
        }
        return res;
    }
} // namespace arc_consistency
