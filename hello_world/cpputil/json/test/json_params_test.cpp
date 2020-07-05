#include <cpputil/json/json_params.h>
#include <cpputil/log/log.h>

#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

using namespace cpputil::json;

std::string json = R"({
    "name": "abc",
    "id": 123,
    "msg": null,
    "score": 456.789,
    "score2": 1.1e10,
    "test": true,
    "keys": ["a", "b", "c"],
    "more_keys": ["x", 1, null, true, {"foo": "bar"}],
    "child": {
        "a": 1,
        "b": [2],
        "c": {"name":3}
    }
})";

TEST(JsonParamsTest, load_json) {
    {
        JsonParams jp;
        auto ret = jp.load_json(json);
        EXPECT_TRUE(ret);
    }
    {
        // empty object
        JsonParams jp;
        auto ret = jp.load_json("{}");
        EXPECT_TRUE(ret);
        // load again
        ret = jp.load_json(json);
        EXPECT_TRUE(ret);
        // load again
        ret = jp.load_json(json);
        EXPECT_TRUE(ret);
    }
    {
        // 需兼容空字符串
        JsonParams jp;
        auto ret = jp.load_json("");
        EXPECT_TRUE(ret);
    }
}

TEST(JsonParamsTest, load_json_err) {
    cpputil::log::init("test/data/log4j.properties");
    {
        std::string json_err = R"({
            "name
        })";
        JsonParams jp;
        auto ret = jp.load_json(json_err);
        EXPECT_FALSE(ret);
    }
    {
        std::string json_err = R"({
            "name":["a":"b"]
        })";
        JsonParams jp;
        auto ret = jp.load_json(json_err);
        EXPECT_FALSE(ret);
    }
    {
        std::string json_err = R"({
            "name":"abc",
        })";
        JsonParams jp;
        auto ret = jp.load_json(json_err);
        EXPECT_FALSE(ret);
    }
    {
        std::string json_err = R"({
            "name":1abc,
        })";
        JsonParams jp;
        auto ret = jp.load_json(json_err);
        EXPECT_FALSE(ret);
    }
}

TEST(JsonParamsTest, dump_json) {
    // empty
    {
        JsonParams jp;
        auto ret = jp.load_json("");
        ASSERT_TRUE(ret);
        auto s = jp.dump_json();
        EXPECT_EQ("{}", s);
    }

    {
        // 如果有浮点数会造成结果不准确
        std::string j = R"({
            "name":"abc",
            "id":123,
            "msg":null,
            "test":true,
            "keys":["a","b","c"],
            "more_keys":["x",1,null,true,{"foo":"bar"}],
            "child":{"a":1,"b":[2],"c":{"name":3}}
        })";
        std::string j1 = R"({"name":"abc","id":123,"msg":null,"test":true,"keys":["a","b","c"],"more_keys":["x",1,null,true,{"foo":"bar"}],"child":{"a":1,"b":[2],"c":{"name":3}}})";
        std::string j2 = R"(["x",1,null,true,{"foo":"bar"}])";
        std::string j3 = R"({"a":1,"b":[2],"c":{"name":3}})";
        JsonParams jp;
        auto ret = jp.load_json(j);
        ASSERT_TRUE(ret);
        auto s1 = jp.dump_json();
        EXPECT_EQ(j1, s1);

        // dump 子节点
        auto s2 = jp.dump_json({"more_keys"});
        EXPECT_EQ(j2, s2);
        auto s3 = jp.dump_json({"child"});
        EXPECT_EQ(j3, s3);
        auto s4 = jp.dump_json({"id"});
        EXPECT_EQ("123", s4);
        auto s5 = jp.dump_json({"msg"});
        EXPECT_EQ("null", s5);
        auto s6 = jp.dump_json({"more_keys", 4, "foo"});
        EXPECT_EQ(R"("bar")", s6);
    }
}

TEST(JsonParamsTest, has) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);
    // has name
    ret = jp.has({"name"});
    EXPECT_TRUE(ret);
    // has id
    ret = jp.has({"id"});
    EXPECT_TRUE(ret);
    // has msg (even if it is null)
    ret = jp.has({"msg"});
    EXPECT_TRUE(ret);
    // has score
    ret = jp.has({"score"});
    EXPECT_TRUE(ret);
    // has score2
    ret = jp.has({"score2"});
    EXPECT_TRUE(ret);
    // has keys
    ret = jp.has({"keys"});
    EXPECT_TRUE(ret);
    // has child
    ret = jp.has({"child"});
    EXPECT_TRUE(ret);
    // has not baz
    ret = jp.has({"baz"});
    EXPECT_FALSE(ret);

    // nested array
    ret = jp.has({"keys", 0});
    EXPECT_TRUE(ret);
    ret = jp.has({"keys", 2});
    EXPECT_TRUE(ret);
    // neg array index (python style)
    ret = jp.has({"keys", -1});
    EXPECT_TRUE(ret);
    ret = jp.has({"keys", -3});
    EXPECT_TRUE(ret);
    // out of bound
    ret = jp.has({"keys", 3});
    EXPECT_FALSE(ret);
    ret = jp.has({"keys", 10000000});
    EXPECT_FALSE(ret);
    ret = jp.has({"keys", -4});
    EXPECT_FALSE(ret);

    // more complex nested array
    ret = jp.has({"more_keys", 2});
    EXPECT_TRUE(ret);
    ret = jp.has({"more_keys", 4, "foo"});
    EXPECT_TRUE(ret);
    // not found
    ret = jp.has({"more_keys", 2, "foo"});
    EXPECT_FALSE(ret);
    ret = jp.has({"more_keys", 4, "baz"});
    EXPECT_FALSE(ret);
}

TEST(JsonParamsTest, get) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);

    // get int
    {
        int i = jp.get<int>({"id"});
        EXPECT_EQ(123, i);
    }
    {  // default 0
        int i = jp.get<int>({"id2"});
        EXPECT_EQ(0, i);
    }
    {  // default value
        int i = jp.get({"id"}, 999);
        EXPECT_EQ(123, i);
    }
    {  // default value
        int i = jp.get({"id3"}, 3);
        EXPECT_EQ(3, i);
    }
    {  // default value if type not match
        int i = jp.get({"msg"}, 4);
        EXPECT_EQ(4, i);
    }

    // get string
    {
        std::string s = jp.get<std::string>({"name"});
        EXPECT_EQ("abc", s);
    }
    {  // default empty
        std::string s = jp.get<std::string>({"name2"});
        EXPECT_EQ("", s);
    }
    {  // default value
        std::string s = jp.get({"name"}, "zz");
        EXPECT_EQ("abc", s);
    }
    {  // default value
        std::string s = jp.get({"name3"}, "xx");
        EXPECT_EQ("xx", s);
    }
    {  // default value if type not match
        std::string s = jp.get({"msg"}, "yy");
        EXPECT_EQ("yy", s);
    }

    // floating point
    {
        float f = jp.get<float>({"score"});
        EXPECT_FLOAT_EQ(456.789, f);
    }
    {
        double df = jp.get<double>({"score"});
        EXPECT_DOUBLE_EQ(456.789, df);
    }
    {
        double df = jp.get<double>({"score2"});
        EXPECT_DOUBLE_EQ(1.1e10, df);
    }
    {  // default value
        double df = jp.get<double>({"score3"});
        EXPECT_DOUBLE_EQ(0.0, df);
    }
    {
        double df = jp.get({"score4"}, 111.222);
        EXPECT_DOUBLE_EQ(111.222, df);
    }
    {  // compatible with int
        double df = jp.get<double>({"id"});
        EXPECT_DOUBLE_EQ(123, df);
    }

    // bool
    {
        bool b = jp.get<bool>({"test"});
        EXPECT_TRUE(b);
    }
    {  // default value
        bool b = jp.get({"test"}, false);
        EXPECT_TRUE(b);
    }
    {  // default value if not found
        bool b = jp.get({"test2"}, false);
        EXPECT_FALSE(b);
    }

    // more nested
    {
        auto i = jp.get<int>({"child", "a"});
        EXPECT_EQ(1, i);
    }
    {
        auto i = jp.get<int>({"child", "b", 0});
        EXPECT_EQ(2, i);
    }
    {
        auto i = jp.get<int>({"child", "c", "name"});
        EXPECT_EQ(3, i);
    }
    {
        auto i = jp.get<int>({"child", "d"});
        EXPECT_EQ(0, i);
    }
}

TEST(JsonParamsTest, get_array) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);

    {
        std::vector<std::string> keys = jp.get_array<std::string>({"keys"});
        EXPECT_EQ(3u, keys.size());
        if (3u == keys.size()) {
            EXPECT_EQ("a", keys[0]);
            EXPECT_EQ("b", keys[1]);
            EXPECT_EQ("c", keys[2]);
        }
    }
    {
        std::vector<std::string> keys =
            jp.get<std::vector<std::string>>({"keys"});
        EXPECT_EQ(3u, keys.size());
        if (3u == keys.size()) {
            EXPECT_EQ("a", keys[0]);
            EXPECT_EQ("b", keys[1]);
            EXPECT_EQ("c", keys[2]);
        }
    }
    {
        std::vector<int> v = jp.get_array<int>({"child", "b"});
        EXPECT_EQ(1u, v.size());
        if (1u == v.size()) {
            EXPECT_EQ(2, v[0]);
        }
    }
    {
        std::vector<int> v = jp.get_array<int>({"child", "a"});
        EXPECT_EQ(0u, v.size());
    }
    {
        std::vector<int> v = jp.get_array<int>({"baz"});
        EXPECT_EQ(0u, v.size());
    }
}

TEST(JsonParamsTest, get_set) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);

    {
        std::set<std::string> keys = jp.get_set<std::string>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
    {  // generic get
        std::set<std::string> keys = jp.get<std::set<std::string>>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
    {
        std::set<int> v = jp.get_set<int>({"child", "b"});
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1u, v.count(2));
    }
    {
        std::set<int> v = jp.get_set<int>({"child", "a"});
        EXPECT_EQ(0u, v.size());
    }
    {
        std::set<int> v = jp.get_set<int>({"baz"});
        EXPECT_EQ(0u, v.size());
    }

    ret = jp.load_json(R"({"keys":["a","b","a","c"]})");
    ASSERT_TRUE(ret);
    {
        std::set<std::string> keys = jp.get<std::set<std::string>>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
}

TEST(JsonParamsTest, get_uset) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);

    {
        std::unordered_set<std::string> keys =
            jp.get_uset<std::string>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
    {  // generic get
        std::unordered_set<std::string> keys =
            jp.get<std::unordered_set<std::string>>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
    {
        std::unordered_set<int> v = jp.get_uset<int>({"child", "b"});
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1u, v.count(2));
    }
    {
        std::unordered_set<int> v = jp.get_uset<int>({"child", "a"});
        EXPECT_EQ(0u, v.size());
    }
    {
        std::unordered_set<int> v = jp.get_uset<int>({"baz"});
        EXPECT_EQ(0u, v.size());
    }

    ret = jp.load_json(R"({"keys":["a","b","a","c"]})");
    ASSERT_TRUE(ret);
    {
        std::unordered_set<std::string> keys =
            jp.get<std::unordered_set<std::string>>({"keys"});
        EXPECT_EQ(3u, keys.size());
        EXPECT_EQ(1u, keys.count("a"));
        EXPECT_EQ(1u, keys.count("b"));
        EXPECT_EQ(1u, keys.count("c"));
    }
}

TEST(JsonParamsTest, get_map) {
    JsonParams jp;
    auto ret = jp.load_json(
        R"({"kvs":{"a": 1, "b": 2, "c": 3}, "more_kvs": {"key":"value"}})");
    ASSERT_TRUE(ret);
    {
        std::map<std::string, int> kvs = jp.get_map<int>({"kvs"});
        EXPECT_EQ(3u, kvs.size());
        EXPECT_EQ(1, kvs["a"]);
        EXPECT_EQ(2, kvs["b"]);
        EXPECT_EQ(3, kvs["c"]);
    }
    {  // generic get
        std::map<std::string, int> kvs =
            jp.get<std::map<std::string, int>>({"kvs"});
        EXPECT_EQ(3u, kvs.size());
        EXPECT_EQ(1, kvs["a"]);
        EXPECT_EQ(2, kvs["b"]);
        EXPECT_EQ(3, kvs["c"]);
    }
    {
        std::map<std::string, std::string> kvs =
            jp.get_map<std::string>({"more_kvs"});
        EXPECT_EQ(1u, kvs.size());
        EXPECT_EQ("value", kvs["key"]);
    }
    {
        std::map<std::string, std::string> kvs =
            jp.get_map<std::string>({"baz"});
        EXPECT_EQ(0u, kvs.size());
    }
}

TEST(JsonParamsTest, get_umap) {
    JsonParams jp;
    auto ret = jp.load_json(
        R"({"kvs":{"a": 1, "b": 2, "c": 3}, "more_kvs": {"key":"value"}})");
    ASSERT_TRUE(ret);
    {
        std::unordered_map<std::string, int> kvs = jp.get_umap<int>({"kvs"});
        EXPECT_EQ(3u, kvs.size());
        EXPECT_EQ(1, kvs["a"]);
        EXPECT_EQ(2, kvs["b"]);
        EXPECT_EQ(3, kvs["c"]);
    }
    {  // generic get
        std::unordered_map<std::string, int> kvs =
            jp.get<std::unordered_map<std::string, int>>({"kvs"});
        EXPECT_EQ(3u, kvs.size());
        EXPECT_EQ(1, kvs["a"]);
        EXPECT_EQ(2, kvs["b"]);
        EXPECT_EQ(3, kvs["c"]);
    }
    {
        std::unordered_map<std::string, std::string> kvs =
            jp.get_umap<std::string>({"more_kvs"});
        EXPECT_EQ(1u, kvs.size());
        EXPECT_EQ("value", kvs["key"]);
    }
    {
        std::unordered_map<std::string, std::string> kvs =
            jp.get_umap<std::string>({"baz"});
        EXPECT_EQ(0u, kvs.size());
    }
}

TEST(JsonParamsTest, get_complex_types) {
    {
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "weights": [
                ["id1", 1, true],
                ["id2", 2.2, false],
                ["id3", 3e4, true]
            ]
        })");
        ASSERT_TRUE(ret);

        std::vector<std::tuple<std::string, double, bool>> vt =
            jp.get<std::vector<std::tuple<std::string, double, bool>>>(
                {"weights"});
        EXPECT_EQ(3u, vt.size());
        if (3u == vt.size()) {
            EXPECT_EQ("id1", std::get<0>(vt[0]));
            EXPECT_DOUBLE_EQ(1, std::get<1>(vt[0]));
            EXPECT_TRUE(std::get<2>(vt[0]));

            EXPECT_EQ("id2", std::get<0>(vt[1]));
            EXPECT_DOUBLE_EQ(2.2, std::get<1>(vt[1]));
            EXPECT_FALSE(std::get<2>(vt[1]));

            EXPECT_EQ("id3", std::get<0>(vt[2]));
            EXPECT_DOUBLE_EQ(3e4, std::get<1>(vt[2]));
            EXPECT_TRUE(std::get<2>(vt[2]));
        }
    }
    {
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "multimap": {
                "id1": [1, 2, 3],
                "id2": [4, 5],
                "id3": []
            }
        })");
        ASSERT_TRUE(ret);

        std::map<std::string, std::vector<int>> mm =
            jp.get<std::map<std::string, std::vector<int>>>({"multimap"});
        EXPECT_EQ(3u, mm.size());

        EXPECT_TRUE(mm.find("id1") != mm.end());
        EXPECT_EQ(3u, mm["id1"].size());
        if (3u == mm["id1"].size()) {
            EXPECT_EQ(1, mm["id1"][0]);
            EXPECT_EQ(2, mm["id1"][1]);
            EXPECT_EQ(3, mm["id1"][2]);
        }

        EXPECT_TRUE(mm.find("id2") != mm.end());
        EXPECT_EQ(2u, mm["id2"].size());
        if (2u == mm["id2"].size()) {
            EXPECT_EQ(4, mm["id2"][0]);
            EXPECT_EQ(5, mm["id2"][1]);
        }

        EXPECT_TRUE(mm.find("id3") != mm.end());
        EXPECT_EQ(0u, mm["id3"].size());
    }
}

TEST(JsonParamsTest, try_get) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);
    {  // try get -- found
        int i;
        bool ret = jp.try_get({"id"}, i);
        EXPECT_TRUE(ret);
        EXPECT_EQ(123, i);
    }
    {  // try get -- not match
        int i;
        bool ret = jp.try_get({"name"}, i);
        EXPECT_FALSE(ret);
    }
    {  // try get -- not match
        int i;
        bool ret = jp.try_get({"keys"}, i);
        EXPECT_FALSE(ret);
    }
    {  // try get -- null
        int i;
        bool ret = jp.try_get({"msg"}, i);
        EXPECT_FALSE(ret);
    }
    {  // try get -- not found
        int i;
        bool ret = jp.try_get({"id2"}, i);
        EXPECT_FALSE(ret);
    }
    {  // try get container -- non-empty
        std::vector<int> v;
        bool ret = jp.try_get({"child", "b"}, v);
        EXPECT_TRUE(ret);
        EXPECT_EQ(1u, v.size());
        if (1u == v.size()) {
            EXPECT_EQ(2, v[0]);
        }
    }
    {  // try get container -- not match
        std::vector<int> v;
        bool ret = jp.try_get({"child", "a"}, v);
        EXPECT_FALSE(ret);
    }
    {  // try get container -- not found
        std::vector<int> v;
        bool ret = jp.try_get({"child", "z"}, v);
        EXPECT_FALSE(ret);
    }
}

TEST(JsonParamsTest, set) {
    {  // set int
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        ret = jp.set({"id"}, 456);
        EXPECT_TRUE(ret);
        EXPECT_EQ(456, jp.get<int>({"id"}));

        ret = jp.set({"id2"}, 789);
        EXPECT_TRUE(ret);
        EXPECT_EQ(789, jp.get<int>({"id2"}));

        // override existing non-match node
        // long int
        ret = jp.set({"test"}, 999999999999L);
        EXPECT_TRUE(ret);
        EXPECT_EQ(999999999999L, jp.get<long>({"test"}));
    }

    {  // set float
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        ret = jp.set({"test"}, 0.3f);
        EXPECT_TRUE(ret);
        EXPECT_FLOAT_EQ(0.3f, jp.get<float>({"test"}));

        ret = jp.set({"test"}, 1.23456789e10);
        EXPECT_TRUE(ret);
        EXPECT_DOUBLE_EQ(1.23456789e10, jp.get<double>({"test"}));
    }

    {  // set string
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        ret = jp.set({"name"}, "joker");
        EXPECT_TRUE(ret);
        EXPECT_EQ("joker", jp.get<std::string>({"name"}));

        ret = jp.set({"name2"}, "joker");
        EXPECT_TRUE(ret);
        EXPECT_EQ("joker", jp.get<std::string>({"name2"}));
    }

    {  // set bool
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        ret = jp.set({"name"}, true);
        EXPECT_TRUE(ret);
        EXPECT_TRUE(jp.get<bool>({"name"}));
    }

    {  // set null
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        ret = jp.set({"name"}, nullptr);
        EXPECT_TRUE(ret);
        EXPECT_EQ("null", jp.dump_json({"name"}));
    }

    {  // set vector
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        std::vector<int> v = {1, 2, 3};
        ret = jp.set({"keys"}, v);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1,2,3]", jp.dump_json({"keys"}));

        ret = jp.set({"child"}, v);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1,2,3]", jp.dump_json({"child"}));
    }

    {  // set set
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        std::set<int> s = {1, 2, 3};
        ret = jp.set({"keys"}, s);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1,2,3]", jp.dump_json({"keys"}));

        ret = jp.set({"child"}, s);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1,2,3]", jp.dump_json({"child"}));
    }

    {  // set map
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
        ret = jp.set({"keys"}, m);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"a":1,"b":2})", jp.dump_json({"keys"}));

        ret = jp.set({"child"}, m);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"a":1,"b":2})", jp.dump_json({"child"}));
    }

    {  // set tuple
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        std::tuple<std::string, int, bool> t =
            std::make_tuple<std::string, int, bool>("a", 1, true);
        ret = jp.set({"keys"}, t);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"(["a",1,true])", jp.dump_json({"keys"}));
    }

    {  // set complex tuple
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        std::vector<std::tuple<std::string, int, bool>> vt = {
            std::make_tuple<std::string, int, bool>("a", 1, true),
            std::make_tuple<std::string, int, bool>("b", 2, false)};
        ret = jp.set({"keys"}, vt);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"([["a",1,true],["b",2,false]])", jp.dump_json({"keys"}));

        std::map<std::string, std::tuple<std::string, int, bool>> mt = {
            {"a", std::make_tuple<std::string, int, bool>("a", 1, true)},
            {"b", std::make_tuple<std::string, int, bool>("b", 2, false)}};
        ret = jp.set({"keys"}, mt);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"a":["a",1,true],"b":["b",2,false]})",
                  jp.dump_json({"keys"}));
    }

    {  // set item in array
        JsonParams jp;
        bool ret = jp.load_json(json);
        ASSERT_TRUE(ret);

        // set path in array
        ret = jp.set({"keys", 1}, 111);
        EXPECT_TRUE(ret);
        EXPECT_EQ(111, jp.get<int>({"keys", 1}));

        // create a new item at the end of array
        ret = jp.set({"keys", 3}, 333);
        EXPECT_TRUE(ret);
        EXPECT_EQ(333, jp.get<int>({"keys", 3}));

        // create a new item at the end of array
        ret = jp.set({"keys", 4}, nullptr);
        EXPECT_TRUE(ret);
        EXPECT_EQ("null", jp.dump_json({"keys", 4}));

        // create a new object at the end of array
        ret = jp.set({"keys", 5, "a"}, 555);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"a":555})", jp.dump_json({"keys", 5}));

        // create a new array at the end of array
        ret = jp.set({"keys", 6, 0}, 1);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1]", jp.dump_json({"keys", 6}));

        // add another item to the new array (and test neg index)
        ret = jp.set({"keys", -1, 1}, 2);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[1,2]", jp.dump_json({"keys", 6}));

        // fail to create at an out-of-bound index
        // (only the immediate one pass-the-end is allowed)
        ret = jp.set({"keys", 100}, 100);
        EXPECT_FALSE(ret);

        // reset to object
        ret = jp.set({"keys", 1, "a"}, 100);
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"a":100})", jp.dump_json({"keys", 1}));

        // reset to object
        ret = jp.set({"keys", 6, "b"}, "200");
        EXPECT_TRUE(ret);
        EXPECT_EQ(R"({"b":"200"})", jp.dump_json({"keys", 6}));

        // reset to array
        ret = jp.set({"keys", 2, 0}, 100);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[100]", jp.dump_json({"keys", 2}));

        // reset to array
        ret = jp.set({"keys", 5, 0}, true);
        EXPECT_TRUE(ret);
        EXPECT_EQ("[true]", jp.dump_json({"keys", 5}));
    }
}

TEST(JsonParamsTest, inherit) {
    {  // simple inherit
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":"2"
        })");
        ASSERT_TRUE(ret);

        JsonParams base;
        ret = base.load_json(R"({
            "b":"3",
            "c":{"cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.inherit(base);
        EXPECT_EQ(1, jp.get<int>({"a"}));
        EXPECT_EQ("2", jp.get<std::string>({"b"}));
        EXPECT_EQ(999, jp.get<int>({"c", "cc"}));
    }

    {  // a little more complex
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "c":{"xx":123}
        })");
        ASSERT_TRUE(ret);

        JsonParams base;
        ret = base.load_json(R"({
            "b":null,
            "c":{"cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.inherit(base);
        EXPECT_EQ(1, jp.get<int>({"a"}));
        EXPECT_EQ("2", jp.get<std::string>({"b", 0}));
        EXPECT_EQ("3", jp.get<std::string>({"b", 1}));
        EXPECT_EQ(123, jp.get<int>({"c", "xx"}));
        EXPECT_EQ(999, jp.get<int>({"c", "cc"}));
    }

    {  // inherit to given path
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "z": {
                "b":["2","3"],
                "c":{"xx":123},
                "d":1
            }
        })");
        ASSERT_TRUE(ret);

        JsonParams base;
        ret = base.load_json(R"({
            "b":null,
            "c":{"cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.inherit({"z"}, base);
        EXPECT_EQ(1, jp.get<int>({"a"}));
        EXPECT_EQ("2", jp.get<std::string>({"b", 0}));
        EXPECT_EQ("3", jp.get<std::string>({"b", 1}));
        EXPECT_EQ(123, jp.get<int>({"z", "c", "xx"}));
        EXPECT_EQ(999, jp.get<int>({"z", "c", "cc"}));
        EXPECT_EQ(1, jp.get<int>({"z", "d"}));
    }
}

TEST(JsonParamsTest, update) {
    {  // simple update
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":"2"
        })");
        ASSERT_TRUE(ret);

        JsonParams up;
        ret = up.load_json(R"({
            "b":"3",
            "c":{"cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.update(up);
        EXPECT_EQ(1, jp.get<int>({"a"}));
        EXPECT_EQ("3", jp.get<std::string>({"b"}));
        EXPECT_EQ(999, jp.get<int>({"c", "cc"}));
    }

    {  // a little more complex
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "c":{"xx":123,"yy":456}
        })");
        ASSERT_TRUE(ret);

        JsonParams up;
        ret = up.load_json(R"({
            "b":null,
            "c":{"xx":"abc", "cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.update(up);
        EXPECT_EQ(1, jp.get<int>({"a"}));
        EXPECT_EQ("null", jp.dump_json({"b"}));
        EXPECT_EQ("abc", jp.get<std::string>({"c", "xx"}));
        EXPECT_EQ(456, jp.get<int>({"c", "yy"}));
        EXPECT_EQ(999, jp.get<int>({"c", "cc"}));
    }

    {  // array will not merge
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3","4"],
            "c":{"xx":123,"yy":456}
        })");
        ASSERT_TRUE(ret);

        jp.update(R"({
            "b":[5,6]
        })");
        EXPECT_EQ("[5,6]", jp.dump_json({"b"}));
    }

    {  // object override existing value
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "c":{"xx":123,"yy":456}
        })");
        ASSERT_TRUE(ret);

        jp.update({"c"}, R"({
            "xx":{"foo":"bar","test":false}
        })");
        EXPECT_EQ(R"({"foo":"bar","test":false})", jp.dump_json({"c", "xx"}));
        EXPECT_EQ("456", jp.dump_json({"c", "yy"}));
    }

    {  // partial update
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "c":{"xx":123,"yy":456}
        })");
        ASSERT_TRUE(ret);

        JsonParams up;
        ret = up.load_json(R"({
            "b":null,
            "c":{"a":"aaa", "xx":"abc", "cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.update(up, {"c"});
        EXPECT_EQ(R"("aaa")", jp.dump_json({"a"}));
        EXPECT_EQ(R"(["2","3"])", jp.dump_json({"b"}));
        EXPECT_EQ(R"("abc")", jp.dump_json({"xx"}));
        EXPECT_EQ("999", jp.dump_json({"cc"}));
    }

    {  // partial update
        JsonParams jp;
        auto ret = jp.load_json(R"({
            "a":1,
            "b":["2","3"],
            "c":{"xx":123,"yy":456}
        })");
        ASSERT_TRUE(ret);

        JsonParams up;
        ret = up.load_json(R"({
            "b":null,
            "zzz":{"a":"aaa", "xx":"abc", "cc":999}
        })");
        ASSERT_TRUE(ret);

        jp.update({"c"}, up, {"zzz"});
        EXPECT_EQ("1", jp.dump_json({"a"}));
        EXPECT_EQ(R"(["2","3"])", jp.dump_json({"b"}));
        EXPECT_EQ("abc", jp.get<std::string>({"c", "xx"}));
        EXPECT_EQ(456, jp.get<int>({"c", "yy"}));
        EXPECT_EQ(999, jp.get<int>({"c", "cc"}));
    }
    {
	std::string json_str1 = R"({
            "array":[],
            "array1":[1],
            "array2":[
                {"a":1}
            ]
        })";
        std::string json_str2 = R"({
            "array":[1],
            "array1":[1,2,3],
            "array2":[
                {"a":2}
            ]
        })";
	JsonParams json1;
	JsonParams json2;
	json1.load_json(json_str1);
	json2.load_json(json_str2);
	JsonParamsPtr test1 = json1.clone();
	JsonParamsPtr test2 = json2.clone();
	test2->update(*test1);
        EXPECT_EQ(R"({"array":[],"array1":[1],"array2":[{"a":1}]})", test2->dump_json());
	test1 = json1.clone();
	test2 = json2.clone();
	test1->inherit(*test2);
        EXPECT_EQ(R"({"array":[1],"array1":[1],"array2":[{"a":1}]})", test1->dump_json());
	test1 = json1.clone();
	test2 = json2.clone();
	test1->inherit(*test2, true);
        EXPECT_EQ(R"({"array":[],"array1":[1],"array2":[{"a":1}]})", test1->dump_json());
    }
}

TEST(JsonParamsTest, clear) {
    JsonParams jp;
    auto ret = jp.load_json(json);
    ASSERT_TRUE(ret);

    jp.clear();
    EXPECT_EQ("{}", jp.dump_json());
}
