#include "json_params.h"

#include <deque>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>

#include <cpputil/log/log.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace cpputil {
namespace json {

DEFINE_LOGGER(JsonParams);

bool JsonParams::load_json(const std::string& json) {
    try {
        // 空字符串视为空Object
        if (json.empty()) {
            doc_.SetObject();
            return true;
        }

        rapidjson::StringStream is(json.c_str());
        doc_.ParseStream(is);
        if (doc_.HasParseError()) {
            LOGGER_WARN(JsonParams, "json parse error: %s (%zu)!", rapidjson::GetParseError_En(doc_.GetParseError()), doc_.GetErrorOffset());
            return false;
        }
        return true;
    } catch (std::exception& e) {
        LOGGER_WARN(JsonParams, "error while loading json configuration. reason: %s", e.what());
    } catch (...) {
        LOGGER_WARN(JsonParams, "error while loading json configuration. reason unknown.");
    }
    return false;
}

bool JsonParams::load_json_file(const std::string& file_name) {
    try {
        FILE* fp = fopen(file_name.c_str(), "r");
        if (fp == NULL) {
            return false;
        }
        std::shared_ptr<FILE> fp_guard(fp, fclose);
        char buffer[256];
        rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
        doc_.ParseStream(is);
        if (doc_.HasParseError()) {
            LOGGER_WARN(JsonParams, "json parse error: %s (%zu)!", rapidjson::GetParseError_En(doc_.GetParseError()), doc_.GetErrorOffset());
            return false;
        }
        return true;
    } catch (std::exception& e) {
        LOGGER_WARN(JsonParams, "error while loading json configuration. reason: %s", e.what());
    } catch (...) {
        LOGGER_WARN(JsonParams, "error while loading json configuration. reason unknown.");
    }
    return false;
}

std::string JsonParams::dump_json() const {
    try {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc_.Accept(writer);
        return buffer.GetString();
    } catch (std::exception& e) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason: %s", e.what());
    } catch (...) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason unknown.");
    }
    return "";
}

std::string JsonParams::dump_json(const JsonPath& path) const {
    try {
        const node_type* node = utils_type::walk_path(doc_, path);
        if (node) {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            node->Accept(writer);
            return buffer.GetString();
        } else {
            return "{}";
        }
    } catch (std::exception& e) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason: %s", e.what());
    } catch (...) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason unknown.");
    }
    return "";
}

bool JsonParams::dump_json_file(const std::string& file_name) const {
    try {
        FILE* fp = fopen(file_name.c_str(), "w");
        std::shared_ptr<FILE> fp_guard(fp, fclose);
        char buffer[256];
        rapidjson::FileWriteStream os(fp, buffer, sizeof(buffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        doc_.Accept(writer);
        return true;
    } catch (std::exception& e) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason: %s", e.what());
    } catch (...) {
        LOGGER_WARN(JsonParams, "error while dumping json configuration. reason unknown.");
    }
    return false;
}

// 将目标节点（target_root）中未设置的配置项复制过来
bool JsonParams::inherit_tree(const node_type& source_root, const node_type& node, const JsonPath& path, const bool keep_empty_array) {
    node_type* target = utils_type::walk_path(doc_, path);
    // 非叶节点，JSON Array
    if (node.IsArray()) {
	if (!keep_empty_array) {
	    // 目标存在且非空数组，停止向下递归
            if (target && !(target->IsArray() && target->Empty())) {
		return false;
	    }
	} else {
	    // 目标存在，停止向下递归
            if (target) {
		return false;
            }
	}
        // 否则继续递归
        return true;
    }
    // 非叶节点，JSON Object
    else if (node.IsObject()) {
        // 目标存在且非Object，停止向下递归
        if (target && !(target->IsObject())) {
            return false;
        }
        return true;
    }
    // 叶节点
    else {
        // 目标存在，则不覆盖；否则创建
        if (!target) {
            utils_type::set(doc_, path, node);
        }
        // 已经到了叶节点
        return false;
    }
}

// 复制所有配置项并覆盖目标节点中的已有配置
bool JsonParams::update_tree(const node_type& source, const node_type& node, const JsonPath& path) {
    // 非叶节点，JSON Array
    if (node.IsArray()) {
        node_type* target = utils_type::walk_path(doc_, path);
        // 目标存在且为Array，则清空；非Array则创建节点。
        if (target) {
            if (!(target->IsArray())) {
                target->SetArray();
            } else {
                target->Clear();
            }
        }
        return true;
    }
    // 非叶节点，JSON Object
    else if (node.IsObject()) {
        node_type* target = utils_type::walk_path(doc_, path);
        // 目标存在且非Object，则创建为Object（清空）
        if (target) {
            if (!(target->IsObject())) {
                target->SetObject();
            }
        }
        return true;
    }
    // 叶节点
    else {
        // 覆盖目标
        utils_type::set(doc_, path, node);
        // 已经到了叶节点
        return false;
    }
}

void JsonParams::inherit(const JsonParams& inherited) {
    inherit(inherited, false);
}

void JsonParams::inherit(const JsonParams& inherited, const bool keep_empty_array) {
    traverse(inherited.doc_, JsonPath(), std::bind(&JsonParams::inherit_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, keep_empty_array));
}

void JsonParams::inherit(const JsonPath& path, const JsonParams& inherited, const bool keep_empty_array) {
    traverse(inherited.doc_, path, std::bind(&JsonParams::inherit_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, keep_empty_array));
}

void JsonParams::inherit(const JsonPath& path, const JsonParams& inherited) {
    inherit(path, inherited, false);
}

void JsonParams::update(const JsonParams& updated) {
    traverse(updated.doc_, JsonPath(), std::bind(&JsonParams::update_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void JsonParams::update(const JsonPath& path, const JsonParams& updated) {
    traverse(updated.doc_, path, std::bind(&JsonParams::update_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void JsonParams::update(const JsonParams& updated, const JsonPath& updated_path, bool clear_when_empty) {
    const node_type* pnode = utils_type::walk_path(updated.doc_, updated_path);
    if (pnode) {
        traverse(*pnode, JsonPath(), std::bind(&JsonParams::update_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    } else {
        if (clear_when_empty) {
            clear();
        }
    }
}

void JsonParams::update(const JsonPath& path, const JsonParams& updated, const JsonPath& updated_path) {
    const node_type* pnode = utils_type::walk_path(updated.doc_, updated_path);
    if (pnode) {
        traverse(*pnode, path, std::bind(&JsonParams::update_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    } else {
        utils_type::walk_create_path(doc_, alloc_, path, true);
    }
}

void JsonParams::update(const std::string& json) {
    JsonParams conf;
    if (conf.load_json(json)) {
        update(conf);
    }
}

void JsonParams::update(const JsonPath& path, const std::string& json) {
    JsonParams conf;
    if (conf.load_json(json)) {
        update(path, conf);
    }
}

std::shared_ptr<JsonParams> JsonParams::clone() const {
    std::shared_ptr<JsonParams> conf(new JsonParams());
    conf->doc_.CopyFrom(doc_, conf->alloc_);
    return conf;
}

std::shared_ptr<JsonParams> JsonParams::clone(const JsonPath& path) const {
    std::shared_ptr<JsonParams> conf(new JsonParams());
    const node_type* pnode = utils_type::walk_path(doc_, path);
    if (pnode) {
        conf->doc_.CopyFrom(*pnode, conf->alloc_);
    }
    return conf;
}

void JsonParams::print() const {
    traverse(doc_, JsonPath(),
        [this] (const node_type& root, const node_type& node, const JsonPath& path) -> bool {
            if (!(node.IsArray()) && !(node.IsObject())) {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                node.Accept(writer);
                std::cout << " === Configuration " << path.dump() << " = " << buffer.GetString() << std::endl;
                //INFO("Configuration %s = %s", path.dump().c_str(), buffer.GetString());
            }
            return true;
        }
    );
}

} /* namespace cpputil */
} /* namespace json */
