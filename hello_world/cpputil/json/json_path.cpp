#include "json_path.h"

#include <sstream>

namespace cpputil {
namespace json {

JsonPath::JsonPath(const std::vector<JsonPathNode>& nodes) {
    for (auto& i : nodes) {
        nodes_.push_back(i);
    }
}

JsonPath::JsonPath(std::initializer_list<JsonPathNode> nodes) {
    for (auto& i : nodes) {
        nodes_.push_back(i);
    }
}

std::string JsonPath::dump() const {
    std::ostringstream os;
    dump_to_string(os);
    return os.str();
}

JsonPath JsonPath::operator/ (const JsonPath& other) const {
    JsonPath merged;
    merged.nodes_ = nodes_;
    for (const auto& i : other.nodes_) {
        merged.nodes_.push_back(i);
    }
    return merged;
}

JsonPath JsonPath::operator/ (const JsonPathNode& other) const {
    JsonPath merged;
    merged.nodes_ = nodes_;
    merged.nodes_.push_back(other);
    return merged;
}

JsonPath & JsonPath::operator/=(const JsonPath & other) {
    for (const auto& i : other.nodes_) {
        nodes_.push_back(i);
    }
    return *this;
}

JsonPath & JsonPath::operator/=(const JsonPathNode & other) {
    nodes_.push_back(other);
    return *this;
}

void JsonPath::parse_from_string(std::istream& is) {
    nodes_.clear();
    int num;
    bool first = true;
    char tok = 0;
    while (is) {
        // the first element, or got a dot
        if (first || tok == SEP) {
            first = false;
            // read a string node, until get a dot or bracket (or eof)
            std::ostringstream os;
            while (true) {
                is.get(tok);
                if (is && tok != SEP && tok != BRK_L) {
                    os << tok;
                } else {
                    break;
                }
            }
            std::string str = os.str();
            if (str.empty()) {
                throw std::runtime_error("Json path invalid! (expect non-empty string)");
            }
            nodes_.push_back(JsonPathNode(std::move(str)));
        }
        // got a left bracket
        else if (tok == BRK_L) {
            // read a number node
            is >> num;
            if (is) {
                // get the right bracket
                is.get(tok);
                if (is && tok == BRK_R) {
                    nodes_.push_back(JsonPathNode(num));
                    // try get next token
                    is.get(tok);
                } else {
                    throw std::runtime_error("Json path invalid! (expect right bracket)");
                }
            } else {
                throw std::runtime_error("Json path invalid! (expect number)");
            }
        }
        // should not come here. just make sure to move forward or throw exception in all branches.
        else {
            throw std::runtime_error("Json path invalid! (unknown error)");
        }
    }
}

void JsonPath::dump_to_string(std::ostream& os) const {
    bool first = true;
    for (const auto& i : nodes_) {
        if (first) {
            // the first item must be string
            if (i.is_string()) {
                os << i.get_string();
                first = false;
            }
        } else {
            if (i.is_number()) {
                os << '[' << i.get_number() << ']';
            } else if (i.is_string()) {
                os << '.';
                os << i.get_string();
            }
        }
    }
}

} /* namespace cpputil */
} /* namespace json */
