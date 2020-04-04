
#include <iostream>
#include <string>
#include <fmt/printf.h>

#include <celutil/packedrangeset.h>
#include <celutil/arraymap.h>

using namespace std;

struct V1
{
    uint32_t val;
    V1(uint32_t v) : val(v) {}
    V1() : val(0) {}
    bool operator==(const V1 &other) const { return val == other.val; }
};

typedef PackedRangeSet<uint32_t, V1> PackedV1Set;

template<>
uint32_t PackedV1Set::getKey(const V1 &v) { return v.val; }

template<>
uint32_t PackedV1Set::invalidKey() { return 0xffffffff; }

template<>
V1 PackedV1Set::invalidValue() { return V1(0xffffffff); }

void dump(const PackedV1Set &v, string name = "")
{
    if (v.getSize() == 0)
    {
        fmt::fprintf(cout, "Set \"%s\" empty!\n", name);
        return;
    }
    fmt::fprintf(cout, "Set \"%s\" size: %u, range [%u, %u]\n", name, v.getSize(), v.getMinKey(), v.getMaxKey());
    for (size_t i = 0; i < v.getSize(); i++)
        fmt::fprintf(cout, "  v[%u] = { %i }\n", i, v[i].val);
}

typedef MultilevelArrayMap<uint32_t, V1, PackedV1Set, 22, 32> MV1Array;

#define DUMP(a) dump(a, #a)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("packedarraymap", "[packedarraymap]")
{
    SECTION("basic")
    {
        cout << MV1Array::ArraySize << endl;
        MV1Array *mam = new MV1Array;;
        REQUIRE(mam->getValue(0) == V1(0xffffffff));
        REQUIRE(mam->insert(0, V1(0)));
        REQUIRE(mam->insert(1, V1(1)));
        REQUIRE(mam->used() == 1);
        REQUIRE(mam->totalUsed() == 2);
        REQUIRE(mam->has(1) == true);
        REQUIRE(mam->has(0) == true);
        REQUIRE(mam->has(2) == false);
        REQUIRE(mam->erase(0) == true);
        REQUIRE(mam->erase(3) == false);
        REQUIRE(mam->totalUsed() == 1);
        REQUIRE(mam->used() == 1);
    }

    SECTION("load test")
    {
        size_t N = 2500000;
        size_t sN = 10000;
        MV1Array *mam = new MV1Array;
        for (size_t i = 0; i < N; i++)
        {
            mam->insert(i, V1(i));
        }
        REQUIRE(mam->totalUsed() == N);
        fmt::printf("Inserted %u elements, ranges number: %u\n", mam->totalUsed(), mam->used());
        auto data = mam->data();
//         DUMP(*data[0]);
//         DUMP(*data[1]);
        for (size_t i = 0; i < N; i++)
        {
//             fmt::printf("Accessing element %u, within range %u\n", i, MV1Array::arrayKey(i));
            REQUIRE(mam->getValue(i).val == i);
        }
        fmt::printf("Checked %u elements\n", mam->totalUsed());
    }
}
