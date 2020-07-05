#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cpputil/json/json_path.h>
#include <cpputil/json/json_path_utils.h>
#include <rapidjson/document.h>

namespace cpputil {
namespace json {

class JsonParams {
public:
    typedef JsonPathUtils<rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::CrtAllocator> utils_type;
    typedef typename utils_type::doc_type doc_type;
    typedef typename utils_type::node_type node_type;
    typedef typename utils_type::alloc_type alloc_type;

    JsonParams()
    : alloc_(), doc_(&alloc_) {
        clear();
    }

    JsonParams(const std::string& json)
    : alloc_(), doc_(&alloc_) {
        if (!load_json(json)) {
            clear();
        }
    }

    // non-copyable
    JsonParams(const JsonParams& other) = delete;
    JsonParams& operator=(const JsonParams& other) = delete;

    // movable
    JsonParams(JsonParams&& other) = default;
    JsonParams& operator=(JsonParams&& other) = default;

    ~JsonParams() = default;

    bool load_json(const std::string& json);

    bool load_json_file(const std::string& file_name);

    std::string dump_json() const;

    std::string dump_json(const JsonPath& path) const;

    bool dump_json_file(const std::string& file_name) const;

    // 遍历被继承的JSON，将自身未配置的配置项复制过来
    void inherit(const JsonParams& inherited);
    void inherit(const JsonParams& inherited, const bool keep_empty_array);

    // 遍历被继承的JSON，将所有配置项继承到path路径下，不覆盖已有配置项
    void inherit(const JsonPath& path, const JsonParams& inherited);
    void inherit(const JsonPath& path, const JsonParams& inherited, const bool keep_empty_array);

    // 遍历目标JSON，将所有配置项复制过来并覆盖现有配置
    void update(const JsonParams& updated);

    // 遍历目标JSON，将所有配置项复制到path路径下，并覆盖现有配置
    void update(const JsonPath& path, const JsonParams& updated);

    // 遍历目标JSON的制定路径，将所有配置项复制过来并覆盖现有配置
    // 如果clear_when_empty为true，则当目标JSON路径不存在时将本对象置为空Object
    void update(const JsonParams& updated, const JsonPath& updated_path, bool clear_when_empty=true);

    // 遍历目标JSON的制定路径，将所有配置项复制到path路径下，并覆盖现有配置
    // 如目标JSON路径不存在则将path路径置为null
    void update(const JsonPath& path, const JsonParams& updated, const JsonPath& updated_path);

    void update(const std::string& json);

    void update(const JsonPath& path, const std::string& json);

    std::shared_ptr<JsonParams> clone() const;

    std::shared_ptr<JsonParams> clone(const JsonPath& path) const;

    void print() const;

    void clear() {
        doc_.SetObject();
    }

    bool has(const JsonPath& path) const {
        return utils_type::has(doc_, path);
    }

    template <typename T>
    T get(const JsonPath& path) const {
        return utils_type::get<T>(doc_, path);
    }

    template <typename T>
    T get(const JsonPath& path, const T& default_value) const {
        return utils_type::get<T>(doc_, path, default_value);
    }

    std::string get(const JsonPath& path, const char* default_value) const {
        return utils_type::get(doc_, path, default_value);
    }

    template <typename T>
    std::vector<T> get_array(const JsonPath& path) const {
        return utils_type::get<std::vector<T>>(doc_, path);
    }

    template <typename T>
    std::set<T> get_set(const JsonPath& path) const {
        return utils_type::get<std::set<T>>(doc_, path);
    }

    template <typename T>
    std::unordered_set<T> get_uset(const JsonPath& path) const {
        return utils_type::get<std::unordered_set<T>>(doc_, path);
    }

    template <typename T>
    std::map<std::string, T> get_map(const JsonPath& path) const {
        return utils_type::get<std::map<std::string, T>>(doc_, path);
    }

    template <typename T>
    std::unordered_map<std::string, T> get_umap(const JsonPath& path) const {
        return utils_type::get<std::unordered_map<std::string, T>>(doc_, path);
    }

    template <typename T>
    bool try_get(const JsonPath& path, T& value) const {
        return utils_type::try_get<T>(doc_, path, value);
    }

    template <typename T>
    bool set(const JsonPath& path, const T& value) {
        return utils_type::set<T>(doc_, path, value);
    }

    bool del(const JsonPath& path) {
        return utils_type::del(doc_, path);
    }

    // 暴露rapidjson对象，以处理更灵活的数据结构
    doc_type& get_doc() {
        return doc_;
    }

    const doc_type& get_doc() const {
        return doc_;
    }

    node_type* get_node_ptr(const JsonPath& path) {
        return utils_type::walk_path(doc_, path);
    }

    const node_type* get_node_ptr(const JsonPath& path) const {
        return utils_type::walk_path(doc_, path);
    }

private:
    // 遍历给定的json root，在每个node上面应用传入的method
    // method签名为 (const node_type& root, const node_type& node, const JsonPath& path) -> bool
    template <typename T>
    static void traverse_recursive(const node_type& root, const node_type& node, const JsonPath& path, T&& method) {
        if (!method(root, node, path)) {
            // break loop
            return;
        }
        if (node.IsArray()) {
            int i = 0;
            for (auto iter = node.Begin(); iter != node.End(); ++iter) {
                JsonPath next = path / JsonPath::value_type(i);
                traverse_recursive(root, *iter, next, std::forward<T>(method));
                ++i;
            }
        } else if (node.IsObject()) {
            for (auto iter = node.MemberBegin(); iter != node.MemberEnd(); ++iter) {
                JsonPath next = path / JsonPath::value_type(iter->name.GetString());
                traverse_recursive(root, iter->value, next, std::forward<T>(method));
            }
        }
    }

    template <typename T>
    static void traverse(const node_type& root, const JsonPath& path, T&& method) {
        traverse_recursive(root, root, path, std::forward<T>(method));
    }

    // inherit configurations from root to target_root, with the given path and specific source node.
    bool inherit_tree(const node_type& source, const node_type& node, const JsonPath& path, bool keep_empty_array);

    // update configurations from root to target_root, with the given path and specific source node.
    bool update_tree(const node_type& source, const node_type& node, const JsonPath& path);

    alloc_type alloc_;
    doc_type doc_;
};

typedef std::shared_ptr<JsonParams> JsonParamsPtr;
typedef std::shared_ptr<const JsonParams> JsonParamsConstPtr;

} /* namespace cpputil */
} /* namespace json */
