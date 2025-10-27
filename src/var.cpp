#include "var.hpp"
#include <algorithm>
#include <cassert>

namespace arc_consistency
{
    bool_val bool_val::True{true};
    bool_val bool_val::False{false};

    var::var(const std::unordered_set<utils::enum_val *> &&init_dom) noexcept : init_domain(init_dom), dom(std::move(init_dom)) {}

    std::string to_string(const var &x) noexcept
    {
        if (x.dom.empty())
            return "∅";
        else if (std::all_of(x.init_domain.begin(), x.init_domain.end(), [&x](utils::enum_val *v)
                             { return dynamic_cast<const enum_val *>(v); }))
        {
            if (x.dom.size() == 1)
                return static_cast<const enum_val *>(*x.dom.begin())->to_string();
            std::string res = " {";
            for (auto it = x.dom.begin(); it != x.dom.end(); ++it)
            {
                const auto *ev = static_cast<const enum_val *>(*it);
                res += ev->to_string();
                if (std::next(it) != x.dom.end())
                    res += ", ";
            }
            res += "}";
            return res;
        }
        else
            return "var";
    }
} // namespace arc_consistency
