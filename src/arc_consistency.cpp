#include "arc_consistency.hpp"
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

    const std::vector<std::reference_wrapper<utils::enum_val>> solver::domain(utils::var v) const noexcept
    {
        std::vector<std::reference_wrapper<utils::enum_val>> dom_vec;
        for (auto *ev_ptr : dom.at(v))
            dom_vec.emplace_back(*ev_ptr);
        return dom_vec;
    }

    void solver::add_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        for (const auto &v : c->scope())
            watchlist.at(v).insert(c);
    }

    void solver::remove_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        for (const auto &v : c->scope())
            watchlist.at(v).erase(c);
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

    bool solver::remove(utils::var v, const utils::enum_val &val) noexcept
    {
        assert(dom.at(v).find(const_cast<utils::enum_val *>(&val)) != dom.at(v).end());
        dom.at(v).erase(const_cast<utils::enum_val *>(&val));
        if (dom.at(v).empty())
            return false;
        to_propagate.push(v);
        return true;
    }

    std::string to_string(const solver &s) noexcept
    {
        std::string res;
        for (std::size_t i = 0; i < s.dom.size(); ++i)
        {
            res += "v" + std::to_string(i);
            if (std::all_of(s.dom.at(i).begin(), s.dom.at(i).end(), [](const utils::enum_val *v)
                            { return dynamic_cast<const enum_val *>(v); }))
            {
                if (s.dom.at(i).size() == 1)
                    res += " = " + dynamic_cast<const enum_val *>(*s.dom.at(i).begin())->to_string() + "\n";
                else
                {
                    res += " ∈ { ";
                    for (auto it = s.dom.at(i).begin(); it != s.dom.at(i).end(); ++it)
                    {
                        res += dynamic_cast<const enum_val *>(*it)->to_string();
                        if (std::next(it) != s.dom.at(i).end())
                            res += ", ";
                    }
                    res += " }\n";
                }
            }
        }
        return res;
    }
} // namespace arc_consistency
