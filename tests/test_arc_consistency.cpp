#include "arc_consistency.hpp"
#include "logging.hpp"
#include <cassert>

class test_enum_val : public arc_consistency::enum_val
{
public:
    explicit test_enum_val(std::string name) : name(std::move(name)) {}

    [[nodiscard]] const std::string &get_name() const { return name; }

private:
    std::string name;
};

void test0()
{
    arc_consistency::solver s;
    const auto v1 = s.new_sat();
    const auto v2 = s.new_sat();
    auto c1 = std::make_shared<arc_consistency::clause>(s, std::vector<utils::lit>{{v1, true}, {v2, false}});
    s.add_constraint(c1);
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    prop = s.assign(v1, arc_consistency::solver::False);
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

int main()
{
    test0();

    return 0;
}
