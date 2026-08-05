#pragma once

#include <set>
#include <string>
#include <vector>

namespace elit21::shopify {

class ShopifyScopes {
public:
    ShopifyScopes() = default;
    explicit ShopifyScopes(std::vector<std::string> scopes);

    void add(std::string scope);
    void remove(const std::string& scope);
    [[nodiscard]] bool contains(const std::string& scope) const;
    [[nodiscard]] bool containsAll(const ShopifyScopes& required) const;
    [[nodiscard]] ShopifyScopes missingFrom(const ShopifyScopes& granted) const;
    [[nodiscard]] ShopifyScopes intersection(const ShopifyScopes& other) const;
    [[nodiscard]] std::string commaSeparated() const;
    [[nodiscard]] std::vector<std::string> sorted() const;
    [[nodiscard]] const std::set<std::string>& values() const noexcept { return scopes_; }
    [[nodiscard]] bool empty() const noexcept { return scopes_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return scopes_.size(); }

    static ShopifyScopes parse(const std::string& comma_separated);
    static ShopifyScopes autonomousManagerDefaults();
    static ShopifyScopes privacyMandatory();

private:
    static bool validScope(const std::string& scope);
    std::set<std::string> scopes_;
};

} // namespace elit21::shopify
