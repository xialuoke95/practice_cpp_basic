#pragma once

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cpputil/json/json_path.h>
#include <rapidjson/document.h>

namespace cpputil {
namespace json {

// default to rapidjson default
template <
    typename Encoding = rapidjson::Value::EncodingType,
    typename Allocator = rapidjson::Value::AllocatorType,
    typename StackAllocator = rapidjson::CrtAllocator>
class JsonPathUtils {
public:
    typedef rapidjson::GenericDocument<Encoding, Allocator, StackAllocator> doc_type;
    typedef rapidjson::GenericValue<Encoding, Allocator> node_type;
    typedef Allocator alloc_type;

    static bool has(const node_type& root, const JsonPath& path) {
        const node_type* current = walk_path(root, path);
        return current != nullptr;
    }

    // get the value or return default-initialized value.
    template <typename T>
    static T get(const node_type& root, const JsonPath& path) {
        T value;
        if (try_get<T>(root, path, value)) {
            return value;
        }
        return T();
    }

    // get the value or return the given default value.
    template <typename T>
    static T get(const node_type& root, const JsonPath& path, const T& default_value) {
        T value;
        if (try_get<T>(root, path, value)) {
            return value;
        }
        return default_value;
    }

    // special case for input as char*.
    static std::string get(const node_type& root, const JsonPath& path, const char* default_value) {
        std::string value;
        if (try_get<std::string>(root, path, value)) {
            return value;
        }
        return default_value;
    }

    // get the value and return true if successful.
    template <typename T>
    static bool try_get(const node_type& root, const JsonPath& path, T& value) {
        const node_type* current = walk_path(root, path);
        if (current) {
            // get value from this node
            return try_get_value<T>(*current, value);
        }
        return false;
    }

    template <typename T>
    static T get_value(const node_type& root) {
        T value;
        if (try_get_value<T>(root, value)) {
            return value;
        }
        return T();
    }

    template <typename T>
    static T get_value(const node_type& root, const T& default_value) {
        T value;
        if (try_get_value<T>(root, value)) {
            return value;
        }
        return default_value;
    }

    // special case for input as char*.
    static std::string get_value(const node_type& root, const char* default_value) {
        std::string value;
        if (try_get_value<std::string>(root, value)) {
            return value;
        }
        return default_value;
    }

    // return true if succeed, assign the value to the reference passed-in;
    // or return false on failure, and the value of passed-in reference is undefined.
    template <typename T>
    static bool try_get_value(const node_type& root, T& value) {
        return try_get_value_impl(root, value);
    }

    // return true if set succesfully, or false if failed.
    template <typename T>
    static bool set(doc_type& root, const JsonPath& path, const T& value) {
        return set<T>(root, root.GetAllocator(), path, value);
    }

    // return true if set succesfully, or false if failed.
    template <typename T>
    static bool set(node_type& root, alloc_type& allocator, const JsonPath& path, const T& value) {
        node_type* current = walk_create_path(root, allocator, path, true);
        if (current) {
            // set value to this node
            set_value<T>(*current, allocator, value);
            return true;
        }
        return false;
    }

    template <typename T>
    static void set_value(doc_type& root, const T& value) {
        set_value<T>(root, root.GetAllocator(), value);
    }

    template <typename T>
    static void set_value(node_type& root, alloc_type& allocator, const T& value) {
        set_value_impl(root, allocator, value);
    }

    static bool del(node_type& root, const JsonPath& path) {
        return walk_erase_path(root, path);
    }

    // walk the path in the document.
    static const node_type* walk_path(const node_type& root, const JsonPath& path);

    // walk the path in the document and return a modifiable node.
    static node_type* walk_path(node_type& root, const JsonPath& path) {
        return const_cast<node_type*>(walk_path(static_cast<const node_type&>(root), path));
    }

    // walk the path in the document and create any missing nodes.
    // if reset_incompatible set, existing incompatible nodes will be destroyed.
    static node_type* walk_create_path(node_type& root, alloc_type& allocator, const JsonPath& path, bool reset_incompatible = false);

    // walk the path in the document and create any missing nodes.
    // if reset_incompatible set, existing incompatible nodes will be destroyed.
    static node_type* walk_create_path(doc_type& root, const JsonPath& path, bool reset_incompatible = false) {
        walk_create_path(root, root.GetAllocator(), path, reset_incompatible);
    }

    // walk the path in the document and remove the result node (from its parent object or array).
    static bool walk_erase_path(node_type& root, const JsonPath& path);

private:
    static bool try_get_value_impl(const node_type& root, int& value) {
        if (root.IsInt()) {
            value = root.GetInt();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, unsigned int& value) {
        if (root.IsUint()) {
            value = root.GetUint();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, int64_t& value) {
        if (root.IsInt64()) {
            value = root.GetInt64();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, uint64_t& value) {
        if (root.IsUint64()) {
            value = root.GetUint64();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, double& value) {
        if (root.IsDouble() || root.IsInt() || root.IsUint() || root.IsInt64() || root.IsUint64()) {
            value = root.GetDouble();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, float& value) {
        if (root.IsDouble() || root.IsInt() || root.IsUint() || root.IsInt64() || root.IsUint64()) {
            value = static_cast<float>(root.GetDouble());
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, bool& value) {
        if (root.IsBool()) {
            value = root.GetBool();
            return true;
        }
        return false;
    }

    static bool try_get_value_impl(const node_type& root, std::string& value) {
        if (root.IsString()) {
            value = root.GetString();
            return true;
        }
        return false;
    }

    template <typename T>
    static bool try_get_value_impl(const node_type& root, std::vector<T>& values) {
        if (root.IsArray()) {
            if (root.Size() > 0) {
                values.reserve(root.Size());
                for (auto iter = root.Begin(); iter != root.End(); ++iter) {
                    T value;
                    if (try_get_value<T>(*iter, value)) {
                        values.push_back(std::move(value));
                    }
                }
            }
            return true;
        }
        return false;
    }

    template <typename T>
    static bool try_get_value_impl(const node_type& root, std::set<T>& values) {
        if (root.IsArray()) {
            for (auto iter = root.Begin(); iter != root.End(); ++iter) {
                T value;
                if (try_get_value<T>(*iter, value)) {
                    values.emplace(std::move(value));
                }
            }
            return true;
        }
        return false;
    }

    template <typename T>
    static bool try_get_value_impl(const node_type& root, std::unordered_set<T>& values) {
        if (root.IsArray()) {
            if (root.Size() > 0) {
                values.reserve(root.Size());
                for (auto iter = root.Begin(); iter != root.End(); ++iter) {
                    T value;
                    if (try_get_value<T>(*iter, value)) {
                        values.emplace(std::move(value));
                    }
                }
            }
            return true;
        }
        return false;
    }

    template <typename T>
    static bool try_get_value_impl(const node_type& root, std::map<std::string, T>& values) {
        if (root.IsObject()) {
            for (auto iter = root.MemberBegin(); iter != root.MemberEnd(); ++iter) {
                std::string name;
                T value;
                if (try_get_value_impl(iter->name, name) && try_get_value_impl(iter->value, value)) {
                    values.emplace(std::move(name), std::move(value));
                }
            }
            return true;
        }
        return false;
    }

    template <typename T>
    static bool try_get_value_impl(const node_type& root, std::unordered_map<std::string, T>& values) {
        if (root.IsObject()) {
            if (root.MemberCount() > 0) {
                values.reserve(root.MemberCount());
                for (auto iter = root.MemberBegin(); iter != root.MemberEnd(); ++iter) {
                    std::string name;
                    T value;
                    if (try_get_value_impl(iter->name, name) && try_get_value_impl(iter->value, value)) {
                        values.emplace(std::move(name), std::move(value));
                    }
                }
            }
            return true;
        }
        return false;
    }

    template <typename... Args>
    static bool try_get_value_impl(const node_type& root, std::tuple<Args...>& values) {
        if (root.IsArray() && root.Size() >= sizeof...(Args)) {
            return try_get_value_impl<0, Args...>(root, values);
        }
        return false;
    }

    // root is array and has at least N items
    template <int N, typename... Args>
    static auto try_get_value_impl(const node_type& root, std::tuple<Args...>& values)
    -> typename std::enable_if<N < sizeof...(Args), bool>::type {
        return try_get_value_impl(root[N], std::get<N>(values))
            && try_get_value_impl<N+1, Args...>(root, values);
    }

    template <int N, typename... Args>
    static auto try_get_value_impl(const node_type& root, std::tuple<Args...>& values)
    -> typename std::enable_if<N == sizeof...(Args), bool>::type {
        return true;
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, int value) {
        root.SetInt(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, unsigned int value) {
        root.SetUint(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, int64_t value) {
        root.SetInt64(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, uint64_t value) {
        root.SetUint64(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, double value) {
        root.SetDouble(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, bool value) {
        root.SetBool(value);
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, std::nullptr_t value) {
        root.SetNull();
    }

    static void set_value_impl(node_type& root, alloc_type& allocator, const std::string& value) {
        root.SetString(value.c_str(), value.length(), allocator);
    }

    // Special version for C-string
    static void set_value_impl(node_type& root, alloc_type& allocator, const char* value) {
        root.SetString(value, allocator);
    }

    // Copy value from an existing node
    static void set_value_impl(node_type& root, alloc_type& allocator, const node_type& value) {
        root.CopyFrom(value, allocator);
    }

    template <typename T>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::vector<T>& values) {
        if (!root.IsArray()) {
            root.SetArray();
        } else {
            root.Clear();
        }
        root.Reserve(values.size(), allocator);
        for (const auto& v: values) {
            node_type value;
            set_value_impl(value, allocator, v);
            root.PushBack(std::move(value), allocator);
        }
    }

    template <typename T>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::set<T>& values) {
        if (!root.IsArray()) {
            root.SetArray();
        } else {
            root.Clear();
        }
        root.Reserve(values.size(), allocator);
        for (const auto& v: values) {
            node_type value;
            set_value_impl(value, allocator, v);
            root.PushBack(std::move(value), allocator);
        }
    }

    template <typename T>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::unordered_set<T>& values) {
        if (!root.IsArray()) {
            root.SetArray();
        } else {
            root.Clear();
        }
        root.Reserve(values.size(), allocator);
        for (const auto& v: values) {
            node_type value;
            set_value_impl(value, allocator, v);
            root.PushBack(std::move(value), allocator);
        }
    }

    template <typename T>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::map<std::string, T>& values) {
        if (!root.IsObject()) {
            root.SetObject();
        } else {
            root.RemoveAllMembers();
        }
        for (const auto& kv : values) {
            node_type name;
            set_value_impl(name, allocator, kv.first);
            node_type value;
            set_value_impl(value, allocator, kv.second);
            root.AddMember(std::move(name), std::move(value), allocator);
        }
    }

    template <typename T>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::unordered_map<std::string, T>& values) {
        if (!root.IsObject()) {
            root.SetObject();
        } else {
            root.RemoveAllMembers();
        }
        for (const auto& kv : values) {
            node_type name;
            set_value_impl(name, allocator, kv.first);
            node_type value;
            set_value_impl(value, allocator, kv.second);
            root.AddMember(std::move(name), std::move(value), allocator);
        }
    }

    template <typename... Args>
    static void set_value_impl(node_type& root, alloc_type& allocator, const std::tuple<Args...>& values) {
        if (!root.IsArray()) {
            root.SetArray();
        } else {
            root.Clear();
        }
        root.Reserve(sizeof...(Args), allocator);
        set_value_impl<0, Args...>(root, allocator, values);
    }

    // root is array
    template <int N, typename... Args>
    static auto set_value_impl(node_type& root, alloc_type& allocator, const std::tuple<Args...>& values)
    -> typename std::enable_if<N < sizeof...(Args), void>::type {
        node_type value;
        set_value_impl(value, allocator, std::get<N>(values));
        root.PushBack(std::move(value), allocator);
        set_value_impl<N+1, Args...>(root, allocator, values);
    }

    // root is array
    template <int N, typename... Args>
    static auto set_value_impl(node_type& root, alloc_type& allocator, const std::tuple<Args...>& values)
    -> typename std::enable_if<N == sizeof...(Args), void>::type {
    }

    // root should be Object!
    static node_type* create_object_child(node_type& root, alloc_type& allocator, const std::string& name, rapidjson::Type type) {
        root.AddMember(node_type(name.c_str(), name.length(), allocator), node_type(type), allocator);
        return &(root.FindMember(name.c_str())->value);
    }

    // root should be Array!
    static node_type* create_array_child(node_type& root, alloc_type& allocator, rapidjson::Type type) {
        root.PushBack(node_type(type), allocator);
        return &(root[root.Size()-1]);
    }

};

template <typename Encoding, typename Allocator, typename StackAllocator>
inline
auto JsonPathUtils<Encoding, Allocator, StackAllocator>::walk_path(
        const node_type& root, const JsonPath& path)
-> const node_type* {
    const node_type* current = &root;
    for (auto iter = path.cbegin(); iter != path.cend(); ++iter) {
        const auto& node = *iter;
        if (node.is_string()) {
            if (current->IsObject()) {
                auto m = current->FindMember(node.get_string().c_str());
                if (m != current->MemberEnd()) {
                    current = &(m->value);
                } else {
                    // node name not found
                    return nullptr;
                }
            } else {
                // node is not object
                return nullptr;
            }
        } else if (node.is_number()) {
            if (current->IsArray()) {
                int num = node.get_number();
                if (num >= 0 && num < static_cast<int>(current->Size())) {
                    current = &(*current)[num];
                }
                // python style negative index
                else if (num < 0 && num >= -static_cast<int>(current->Size())) {
                    current = &(*current)[current->Size() + num];
                } else {
                    // out of range
                    return nullptr;
                }
            } else {
                // node is not array
                return nullptr;
            }
        } else {
            // never happens
            return nullptr;
        }
    }
    return current;
}

template <typename Encoding, typename Allocator, typename StackAllocator>
inline
auto JsonPathUtils<Encoding, Allocator, StackAllocator>::walk_create_path(
        node_type& root, alloc_type& allocator, const JsonPath& path, bool reset_incompatible /* = false */)
-> node_type* {
    node_type* current = &root;
    // A flag indicates that a newly created node is required.
    bool need_create = false;
    // If not null, means the newly created node is a member of current;
    // Or else if null, the newly created node should be pushed back to current array.
    const std::string* create_name = nullptr;

    for (auto iter = path.cbegin(); iter != path.cend(); ++iter) {
        const auto& node = *iter;
        if (node.is_string()) {
            // need an object here
            if (need_create) {
                if (create_name != nullptr) {
                    current = create_object_child(*current, allocator, *create_name, rapidjson::kObjectType);
                } else {
                    current = create_array_child(*current, allocator, rapidjson::kObjectType);
                }
                need_create = false;
            }
            if (!(current->IsObject())) {
                if (reset_incompatible) {
                    current->SetObject();
                } else {
                    return nullptr;
                }
            }
            // current is an object
            auto m = current->FindMember(node.get_string().c_str());
            if (m != current->MemberEnd()) {
                current = &(m->value);
            } else {
                need_create = true;
                create_name = &(node.get_string());
            }
        } else if (node.is_number()) {
            // need an array here
            if (need_create) {
                if (create_name != nullptr) {
                    current = create_object_child(*current, allocator, *create_name, rapidjson::kArrayType);
                } else {
                    current = create_array_child(*current, allocator, rapidjson::kArrayType);
                }
                need_create = false;
            }
            if (!(current->IsArray())) {
                if (reset_incompatible) {
                    current->SetArray();
                } else {
                    return nullptr;
                }
            }
            // crrent is an array
            int num = node.get_number();
            if (num >= 0 && num < static_cast<int>(current->Size())) {
                current = &(*current)[num];
            }
            // python style negative index
            else if (num < 0 && num >= -static_cast<int>(current->Size())) {
                current = &(*current)[current->Size() + num];
            }
            // create the next element if not exist
            else if (num == static_cast<int>(current->Size())) {
                need_create = true;
                create_name = nullptr;
            }
            else {
                // out of range
                return nullptr;
            }
        } else {
            // never happens
            return nullptr;
        }
    }
    // need a value here
    if (need_create) {
        if (create_name != nullptr) {
            current = create_object_child(*current, allocator, *create_name, rapidjson::kNullType);
        } else {
            current = create_array_child(*current, allocator, rapidjson::kNullType);
        }
        need_create = false;
    }
    return current;
}

template <typename Encoding, typename Allocator, typename StackAllocator>
inline
auto JsonPathUtils<Encoding, Allocator, StackAllocator>::walk_erase_path(node_type& root, const JsonPath& path)
-> bool {
    node_type* current = &root;
    for (auto iter = path.cbegin(); iter != path.cend();) {
        const auto& node = *iter;
        ++iter;
        if (node.is_string()) {
            if (current->IsObject()) {
                auto m = current->FindMember(node.get_string().c_str());
                if (m != current->MemberEnd()) {
                    if (iter != path.cend()) {
                        current = &(m->value);
                    } else {
                        current->EraseMember(m);
                        return true;
                    }
                } else {
                    // node name not found
                    return false;
                }
            } else {
                // node is not object
                return false;
            }
        } else if (node.is_number()) {
            if (current->IsArray()) {
                int num = node.get_number();
                if (num >= 0 && num < static_cast<int>(current->Size())) {
                    if (iter != path.cend()) {
                        current = &(*current)[num];
                    } else {
                        current->Erase(current->Begin() + num);
                        return true;
                    }
                }
                // python style negative index
                else if (num < 0 && num >= -static_cast<int>(current->Size())) {
                    if (iter != path.cend()) {
                        current = &(*current)[current->Size() + num];
                    } else {
                        current->Erase(current->End() + num);
                        return true;
                    }
                } else {
                    // out of range
                    return false;
                }
            } else {
                // node is not array
                return false;
            }
        } else {
            // never happens
            return false;
        }
    }
    return false;
}

} /* namespace json */
} /* namespace cpputil */
