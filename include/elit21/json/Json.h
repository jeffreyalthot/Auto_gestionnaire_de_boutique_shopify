#pragma once
#include "elit21/core/Result.h"
#include <json-c/json.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace elit21 {
class Json {
public:
    Json();
    explicit Json(json_object* object, bool take_ownership=true);
    Json(const Json& other);
    Json(Json&& other) noexcept;
    Json& operator=(const Json& other);
    Json& operator=(Json&& other) noexcept;
    ~Json();
    static Result<Json> parse(const std::string& text);
    static Result<Json> parseFile(const std::string& path);
    static Json object();
    static Json array();
    bool isNull() const; bool isObject() const; bool isArray() const;
    bool isString() const; bool isNumber() const; bool isBool() const;
    bool contains(const std::string& key) const;
    Json get(const std::string& key) const;
    Json at(std::size_t index) const;
    std::size_t size() const;
    std::string stringValue(const std::string& fallback={}) const;
    std::string scalarStringValue(const std::string& fallback={}) const;
    double numberValue(double fallback=0.0) const;
    std::int64_t int64Value(std::int64_t fallback=0) const;
    int intValue(int fallback=0) const;
    bool boolValue(bool fallback=false) const;
    std::string getString(const std::string& key,const std::string& fallback={}) const;
    std::string getScalarString(const std::string& key,const std::string& fallback={}) const;
    double getNumber(const std::string& key,double fallback=0.0) const;
    std::int64_t getInt64(const std::string& key,std::int64_t fallback=0) const;
    int getInt(const std::string& key,int fallback=0) const;
    bool getBool(const std::string& key,bool fallback=false) const;
    void set(const std::string& key,const std::string& value);
    void set(const std::string& key,const char* value);
    void set(const std::string& key,double value);
    void set(const std::string& key,std::int64_t value);
    void set(const std::string& key,int value);
    void set(const std::string& key,bool value);
    void set(const std::string& key,const Json& value);
    void push(const Json& value);
    std::string dump(bool pretty=false) const;
    json_object* raw() const { return object_; }
private:
    json_object* object_{nullptr};
};
}
