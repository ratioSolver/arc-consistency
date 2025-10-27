#include "arc_consistency.hpp"
#include "logging.hpp"
#include <cassert>

class test_enum_val : public arc_consistency::enum_val
{
public:
    explicit test_enum_val(std::string name) : name(std::move(name)) {}

    std::string to_string() const override { return name; }

private:
    std::string name;
};

void test0()
{
    arc_consistency::solver s;
    const auto v1 = s.new_sat();
    const auto v2 = s.new_sat();
    s.add_constraint(s.new_clause({{v1, true}, {v2, false}}));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v1, arc_consistency::solver::False));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

void test1()
{
    arc_consistency::solver s;
    const auto v1 = s.new_sat();
    const auto v2 = s.new_sat();
    s.add_constraint(s.new_equal(v1, v2));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v1, arc_consistency::solver::True));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

void test2()
{
    test_enum_val a("A");
    test_enum_val b("B");
    test_enum_val c("C");

    arc_consistency::solver s;
    const auto v1 = s.new_var({a, b, c});
    const auto v2 = s.new_var({a, b, c});
    s.add_constraint(s.new_equal(v1, v2));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v1, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

void test3()
{
    test_enum_val a("A");
    test_enum_val b("B");
    test_enum_val c("C");

    arc_consistency::solver s;
    const auto v1 = s.new_var({a, b, c});
    const auto v2 = s.new_var({a, b, c});
    const auto v3 = s.new_var({a, b, c});
    auto c1 = s.new_equal(v1, v2);
    auto c2 = s.new_equal(v2, v3);
    s.add_constraint(c1);
    s.add_constraint(c2);
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v1, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.remove_constraint(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v3, b));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

void test4()
{
    test_enum_val a("A");
    test_enum_val b("B");

    arc_consistency::solver s;
    const auto v1 = s.new_var({a, b});
    const auto v2 = s.new_var({a, b});
    const auto v3 = s.new_var({a, b});
    auto c1 = s.new_distinct(v1, v2);
    auto c2 = s.new_distinct(v2, v3);
    s.add_constraint(c1);
    s.add_constraint(c2);
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(v1, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_forbid(v3, a));
    prop = s.propagate();
    assert(!prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.remove_constraint(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

int main()
{
    test0();
    test1();
    test2();
    test3();
    test4();

    return 0;
}
