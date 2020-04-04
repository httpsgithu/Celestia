
#include <celutil/arraymap.h>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

struct V1
{
    float val;
    V1(float v) : val(v) {}
    V1() : val(0) {}
};

bool operator==(const V1 &a1, const V1 &a2) { return a1.val == a2.val; }
bool operator!=(const V1 &a1, const V1 &a2) { return a1.val != a2.val; }

typedef ArrayMap<uint16_t, V1, 16> A16_V1;
typedef ArrayMap<uint16_t, V1 *, 16> A16_V1p;

template<>
V1 A16_V1::invalidValue() { return V1(-1); }
template<>
V1 *A16_V1p::invalidValue() { return nullptr; }

typedef MultilevelArrayMap<uint32_t, V1, A16_V1, 16, 32> MA32_16_V1;
typedef MultilevelArrayMap<uint32_t, V1, A16_V1p, 16, 32> MA32_16_V1p;

TEST_CASE("ArrayMap", "[ArrayMap]")
{
    SECTION("class value")
    {
        REQUIRE(V1(0) == V1(0));
        REQUIRE(V1(-1) != V1(0));
        REQUIRE(V1(-1) == V1(-1));
        REQUIRE(V1(0) != V1(1));
        A16_V1 am;
        REQUIRE(am.used() == 0);
        REQUIRE(am.size() == 0x10000);
        REQUIRE(am.getPtr(0) == nullptr);
        REQUIRE(am.getPtr(1) == nullptr);
        REQUIRE(am.getPtr(2) == nullptr);
        REQUIRE(am.has(2) == false);
        REQUIRE(am.insert(0, V1(5)) == true);
        REQUIRE(am.used() == 1);
        REQUIRE(am.getRef(0).val == 5);
        REQUIRE(am.has(0) == true);
        REQUIRE(am.erase(1) == false);
        REQUIRE(am.erase(2) == false);
        REQUIRE(am.erase(1) == false);
        REQUIRE(am.used() == 1);
        REQUIRE(am.erase(0) == true);
        REQUIRE(am.used() == 0);
        REQUIRE(am.getPtr(0) == nullptr);
        REQUIRE(am.has(0) == false);
        REQUIRE(am.insert(0, V1(1)) == true);
        REQUIRE(am.used() == 1);
        REQUIRE(am.insert(0, V1(4)) == false);
        REQUIRE(am.used() == 1);
    }

    SECTION("pointer value")
    {
        A16_V1p am;
        V1 v1(5), v2(4);
        REQUIRE(am.used() == 0);
        REQUIRE(am.size() == 0x10000);
        REQUIRE(am.getValue(0) == nullptr);
        REQUIRE(am.getValue(1) == nullptr);
        REQUIRE(am.getValue(2) == nullptr);
        am.insert(0, &v1);
        REQUIRE(am.used() == 1);
        REQUIRE(am.getRef(0) == &v1);
        am.erase(1);
        am.erase(2);
        am.erase(1);
        REQUIRE(am.used() == 1);
        am.erase(0);
        REQUIRE(am.used() == 0);
        REQUIRE(am.getValue(0) == nullptr);
        am.insert(0, &v1);
        REQUIRE(am.used() == 1);
        am.insert(0, &v2);
        REQUIRE(am.used() == 1);
    }

    SECTION("multilevel class value")
    {
        MA32_16_V1 am;
        REQUIRE(am.used() == 0);
        REQUIRE(am.size() == 0x10000);
        REQUIRE(am.getPtr(0) == nullptr);
        REQUIRE(am.getPtr(1) == nullptr);
        REQUIRE(am.getPtr(2) == nullptr);
        REQUIRE(am.has(2) == false);
        REQUIRE(am.insert(0, V1(5)) == true);
        REQUIRE(am.used() == 1);
        REQUIRE(am.getRef(0).val == 5);
        REQUIRE(am.has(0) == true);
        REQUIRE(am.erase(1) == false);
        REQUIRE(am.erase(2) == false);
        REQUIRE(am.erase(1) == false);
        REQUIRE(am.used() == 1);
        REQUIRE(am.erase(0) == true);
        REQUIRE(am.used() == 0);
        REQUIRE(am.getPtr(0) == nullptr);
        REQUIRE(am.has(0) == false);
        REQUIRE(am.insert(0, V1(1)) == true);
        REQUIRE(am.used() == 1);
        REQUIRE(am.insert(0, V1(4)) == false);
        REQUIRE(am.used() == 1);
        REQUIRE(am.insert(1 << 20, V1(5)) == true);
        REQUIRE(am.used() == 2);
        REQUIRE(am.has(1 << 20) == true);
        REQUIRE(am.getRef(1 << 20).val == 5);
        REQUIRE(am.has(1 << 21) == false);
        REQUIRE(am.getPtr(1 << 21) == nullptr);
    }

    SECTION("load test")
    {
        MA32_16_V1 ma;
        V1 v1(5);
        for (size_t i = 0; i < 2500000; i++)
        {
            ma.insert(i, v1);
        }
        REQUIRE(ma.totalUsed() == 2500000);
    }
}
