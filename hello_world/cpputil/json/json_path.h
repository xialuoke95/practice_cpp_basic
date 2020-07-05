#pragma once

#include <vector>
#include <deque>
#include <limits>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace cpputil {
namespace json {

class JsonPath {
public:
    // A single node in a json path, either a number or a string.
    // When n == STR_TYPE, only the string is available;
    // or else only the number is available.
    class JsonPathNode {
    public:
        JsonPathNode(std::string str) : str_(std::move(str)), num_(INVALID) {
            if (str_.empty()) {
                throw std::range_error("Json path string is empty!");
            }
        }

        JsonPathNode(const char* c_str) : str_(c_str), num_(INVALID) {
            if (str_.empty()) {
                throw std::range_error("Json path string is empty!");
            }
        }

        JsonPathNode(int num) : num_(num) {
            if (num_ == INVALID) {
                throw std::range_error("Json path index out of range!");
            }
        }

        ~JsonPathNode() = default;

        bool is_number() const {
            return num_ != INVALID;
        }

        bool is_string() const {
            return num_ == INVALID;
        }

        int get_number() const {
            return num_;
        }

        const std::string& get_string() const {
            return str_;
        }

    private:
        static const int INVALID = std::numeric_limits<int>::max();

        std::string str_;
        int num_;
    };

    // A simple wrapped iterator.
    template <typename Iter, typename Value>
    struct JsonPathNodeIterator {
        Iter iter;

        JsonPathNodeIterator(Iter i) : iter(i) {
        }

        JsonPathNodeIterator(const JsonPathNodeIterator&) = default;
        JsonPathNodeIterator& operator= (const JsonPathNodeIterator&) = default;

        bool operator== (const JsonPathNodeIterator& other) {
            return iter == other.iter;
        }

        bool operator!= (const JsonPathNodeIterator& other) {
            return iter != other.iter;
        }

        JsonPathNodeIterator& operator++ () {
            ++iter;
            return *this;
        }

        Value operator* () {
            return *iter;
        }
    };

    typedef JsonPathNode value_type;
    typedef JsonPathNodeIterator<std::deque<JsonPathNode>::iterator, JsonPathNode&> iterator;
    typedef JsonPathNodeIterator<std::deque<JsonPathNode>::const_iterator, const JsonPathNode&> const_iterator;

    // path separator
    static const char SEP = '.';
    static const char BRK_L = '[';
    static const char BRK_R = ']';

    JsonPath() = default;

    JsonPath(const std::vector<JsonPathNode>& nodes);

    JsonPath(std::initializer_list<JsonPathNode> nodes);

    JsonPath(const JsonPath& other) = default;

    JsonPath(JsonPath&& other) = default;

    JsonPath& operator= (const JsonPath& other) = default;

    JsonPath& operator= (JsonPath&& other) = default;

    ~JsonPath() = default;

    iterator begin() {
        return iterator(nodes_.begin());
    }

    iterator end() {
        return iterator(nodes_.end());
    }

    const_iterator cbegin() const {
        return const_iterator(nodes_.cbegin());
    }

    const_iterator cend() const {
        return const_iterator(nodes_.cend());
    }

    value_type& front() {
        return nodes_.front();
    }

    void pop() {
        nodes_.pop_front();
    }

    bool empty() const {
        return nodes_.empty();
    }

    std::string dump() const;

    JsonPath operator/ (const JsonPath& other) const;

    JsonPath operator/ (const JsonPathNode& other) const;

    JsonPath & operator/=(const JsonPath & other);

    JsonPath & operator/=(const JsonPathNode & other);

private:
    std::deque<JsonPathNode> nodes_;

    void parse_from_string(std::istream& is);

    void dump_to_string(std::ostream& os) const;
};

} /* namespace cpputil */
} /* namespace json */
