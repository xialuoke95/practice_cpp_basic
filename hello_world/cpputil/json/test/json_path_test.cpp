#include <gtest/gtest.h>
#include <cpputil/json/json_path.h>

using namespace cpputil::json;

TEST(JsonPathTest, json_path) {
    std::vector<JsonPath::JsonPathNode> v({"a", "b"});
    JsonPath p(v);
    EXPECT_STREQ("a.b", p.dump().c_str());
}
