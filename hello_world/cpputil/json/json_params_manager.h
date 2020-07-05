#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <cpputil/json/json_params.h>

namespace cpputil {
namespace json {

class JsonParamsManager {
public:
    JsonParamsManager() = default;
    ~JsonParamsManager() = default;

    void load_from_directory(const std::string& path, bool with_name_check_param = false);

    std::vector<std::string> get_params_names() const {
        std::vector<std::string> names;
        for (auto& iter : params_map_) {
            names.push_back(iter.first);
        }
        return names;
    }

    JsonParamsConstPtr get_params_by_name(const std::string& name) const {
        auto iter = params_map_.find(name);
        if (iter != params_map_.end()) {
            return iter->second;
        }
        return nullptr;
    }

    JsonParamsPtr create_live_params(const std::string& json) const;

    JsonParamsPtr create_live_params(const std::string& json, const std::string& inherit_name) const;

private:
    std::unordered_map<std::string, JsonParamsPtr> params_map_;

    static const std::string kConfName;
    static const std::string kConfInherit;
};

} /* namespace cpputil */
} /* namespace json */
