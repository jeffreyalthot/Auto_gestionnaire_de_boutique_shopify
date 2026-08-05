#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace elit21::platform {

enum class ComponentState {
    created,
    configured,
    running,
    stopped,
    faulted
};

struct OperationContext {
    std::string correlation_id;
    std::string payload;
    bool dry_run{true};
    std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};
};

struct OperationResult {
    bool success{false};
    std::string code;
    std::string message;
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> attributes;

    static OperationResult ok(std::string message = "completed");
    static OperationResult failure(std::string code, std::string message);
};

struct ComponentSnapshot {
    std::string name;
    std::string role;
    ComponentState state{ComponentState::created};
    std::uint64_t processed{0};
    std::uint64_t accepted{0};
    std::uint64_t rejected{0};
    std::uint64_t failures{0};
    std::string last_code;
    std::string last_message;
};

class ManagedComponent {
public:
    ManagedComponent(std::string name, std::string role);
    virtual ~ManagedComponent() = default;

    ManagedComponent(const ManagedComponent&) = delete;
    ManagedComponent& operator=(const ManagedComponent&) = delete;

    bool configure(std::unordered_map<std::string, std::string> settings);
    bool start();
    void stop() noexcept;
    OperationResult tick(const OperationContext& context);
    [[nodiscard]] ComponentSnapshot snapshot() const;
    [[nodiscard]] std::optional<std::string> setting(std::string_view key) const;
    [[nodiscard]] bool isRunning() const noexcept;

    virtual OperationResult execute(const OperationContext& context) = 0;

protected:
    [[nodiscard]] const std::string& componentName() const noexcept { return name_; }
    [[nodiscard]] const std::string& componentRole() const noexcept { return role_; }

private:
    std::string name_;
    std::string role_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::string> settings_;
    std::atomic<ComponentState> state_{ComponentState::created};
    std::atomic<std::uint64_t> processed_{0};
    std::atomic<std::uint64_t> accepted_{0};
    std::atomic<std::uint64_t> rejected_{0};
    std::atomic<std::uint64_t> failures_{0};
    std::string last_code_;
    std::string last_message_;
};

[[nodiscard]] std::uint64_t stableHash(std::string_view value) noexcept;
[[nodiscard]] std::string componentStateName(ComponentState state);

} // namespace elit21::platform
