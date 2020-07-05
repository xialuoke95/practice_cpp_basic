#include "json_params_manager.h"

#include <boost/filesystem.hpp>
#include <cpputil/log/log.h>

namespace cpputil {
namespace json {

const std::string JsonParamsManager::kConfName = "name";
const std::string JsonParamsManager::kConfInherit = "inherit";

void JsonParamsManager::load_from_directory(const std::string& path_name, bool with_name_check_param) {
    boost::filesystem::path path(path_name);
    boost::filesystem::directory_iterator di(path);
    boost::filesystem::directory_iterator end_di;
    for (; di != end_di; ++di) {
        if (boost::filesystem::is_regular_file(*di)) {
            std::string filename = di->path().filename().native();
            // 点开头的文件不参与load，防止load swp文件
            if (!filename.empty() && filename[0] != '.') {
                std::shared_ptr<JsonParams> params(new JsonParams());
                if (!params->load_json_file(di->path().native())) {
                    throw std::runtime_error("failed parsing json configuration file " + di->path().native());
                }
                std::string name = params->get({kConfName}, "");
                std::string filename_nosuffix = boost::filesystem::path(filename).stem().string();
                if (!name.empty()) {
                    // with_name_check_param为false或者文件名与name值匹配
                    if (!with_name_check_param || name == filename_nosuffix) {
                        params_map_[name] = std::move(params);
                        DEBUG("Load params file : %s", filename.c_str());
                    } else {
                        WARN("Params file`s name : %s is not consistant with `name` in json file : %s", filename_nosuffix.c_str(), name.c_str());
                    }
                } else {
                    WARN("Params file does not have name or name is empty! file name : %s", di->path().string().c_str());
                }
            }
        }
    }

    // 记录已经完成merge的config，防止重复merge
    std::unordered_map<std::string, bool> is_merged;
    for (auto& pair : params_map_) {
        JsonParams& params = *(pair.second);
        // 本配置已经完成merge，跳过后面的merge流程
        if (is_merged[pair.first]) {
            //params.print();
            continue;
        }
        // 继承路径中出现过的配置集，用于检测继承是否有环
        std::set<JsonParams*> inherited_set;
        std::stack<JsonParams*> inherited_stack;
        JsonParams* current = &params;
        while (current != nullptr) {
            // name一定存在
            std::string name = current->get<std::string>({kConfName});
            if (inherited_set.count(current) != 0) {
                // 继承路径中检测到环
                throw std::runtime_error("configuration inherit loop detected!");
            }
            inherited_set.insert(current);
            // 先处理根部（后进先出）
            inherited_stack.push(current);
            // 本配置已经完成merge（已添加），不再向下递归
            if (is_merged[name]) {
                break;
            }
            // 本配置已经是最底层（根节点）
            std::string inherit_name = current->get({kConfInherit}, "");
            if (inherit_name.empty()) {
                is_merged[name] = true;
                break;
            }
            auto iter = params_map_.find(inherit_name);
            if (iter == params_map_.end()) {
                // 继承配置未找到
                throw std::runtime_error("inherited configuration not found!");
            }
            current = iter->second.get();
        }

        // 从继承树的根到叶，将配置项复制到叶子节点
        JsonParams* inherit = inherited_stack.top();
        inherited_stack.pop();
        while (!inherited_stack.empty()) {
            // 对栈顶配置项进行merge操作
            JsonParams* current = inherited_stack.top();
            const std::string& name = current->get<std::string>({kConfName});
            current->inherit(*inherit);
            is_merged[name] = true;
            // 向下循环
            inherit = current;
            inherited_stack.pop();
        }
        //params.print();
    }
}

JsonParamsPtr JsonParamsManager::create_live_params(const std::string& json) const {
    JsonParamsPtr params(new JsonParams());
    params->load_json(json);
    return params;
}

JsonParamsPtr JsonParamsManager::create_live_params(const std::string& json, const std::string& inherit_name) const {
    JsonParamsPtr params(new JsonParams());
    params->load_json(json);
    // set inherit config
    auto iter = params_map_.find(inherit_name);
    if (iter != params_map_.end()) {
        // merge inherit config
        params->inherit(*(iter->second));
    }
    return params;
}

} /* namespace cpputil */
} /* namespace json */
