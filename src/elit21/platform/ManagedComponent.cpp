#include "elit21/platform/ManagedComponent.h"

#include <utility>

namespace elit21::platform {

OperationResult OperationResult::ok(std::string message) {
    OperationResult result;
    result.success = true;
    result.code = "OK";
    result.message = std::move(message);
    return result;
}

OperationResult OperationResult::failure(std::string code, std::string message) {
    OperationResult result;
    result.success = false;
    result.code = std::move(code);
    result.message = std::move(message);
    return result;
}

ManagedComponent::ManagedComponent(std::string name, std::string role)
    : name_(std::move(name)), role_(std::move(role)) {}

bool ManagedComponent::configure(std::unordered_map<std::string, std::string> settings) {
    const auto current = state_.load();
    if (current == ComponentState::running) {
        return false;
    }
    {
        std::scoped_lock lock(mutex_);
        settings_ = std::move(settings);
        last_code_ = "CONFIGURED";
        last_message_ = "Configuration accepted";
    }
    state_.store(ComponentState::configured);
    return true;
}

bool ManagedComponent::start() {
    const auto current = state_.load();
    if (current == ComponentState::running) {
        return true;
    }
    if (current == ComponentState::faulted) {
        return false;
    }
    state_.store(ComponentState::running);
    std::scoped_lock lock(mutex_);
    last_code_ = "STARTED";
    last_message_ = "Component running";
    return true;
}

void ManagedComponent::stop() noexcept {
    state_.store(ComponentState::stopped);
    std::scoped_lock lock(mutex_);
    last_code_ = "STOPPED";
    last_message_ = "Component stopped";
}

OperationResult ManagedComponent::tick(const OperationContext& context) {
    ++processed_;
    if (state_.load() != ComponentState::running) {
        ++rejected_;
        auto result = OperationResult::failure("NOT_RUNNING", "Component must be started before execution");
        std::scoped_lock lock(mutex_);
        last_code_ = result.code;
        last_message_ = result.message;
        return result;
    }

    try {
        auto result = execute(context);
        if (result.success) {
            ++accepted_;
        } else {
            ++rejected_;
        }
        std::scoped_lock lock(mutex_);
        last_code_ = result.code;
        last_message_ = result.message;
        return result;
    } catch (const std::exception& error) {
        ++failures_;
        state_.store(ComponentState::faulted);
        auto result = OperationResult::failure("EXCEPTION", error.what());
        std::scoped_lock lock(mutex_);
        last_code_ = result.code;
        last_message_ = result.message;
        return result;
    } catch (...) {
        ++failures_;
        state_.store(ComponentState::faulted);
        auto result = OperationResult::failure("UNKNOWN_EXCEPTION", "Unknown component exception");
        std::scoped_lock lock(mutex_);
        last_code_ = result.code;
        last_message_ = result.message;
        return result;
    }
}

ComponentSnapshot ManagedComponent::snapshot() const {
    ComponentSnapshot output;
    output.name = name_;
    output.role = role_;
    output.state = state_.load();
    output.processed = processed_.load();
    output.accepted = accepted_.load();
    output.rejected = rejected_.load();
    output.failures = failures_.load();
    std::scoped_lock lock(mutex_);
    output.last_code = last_code_;
    output.last_message = last_message_;
    return output;
}

std::optional<std::string> ManagedComponent::setting(std::string_view key) const {
    std::scoped_lock lock(mutex_);
    const auto iterator = settings_.find(std::string(key));
    if (iterator == settings_.end()) {
        return std::nullopt;
    }
    return iterator->second;
}

bool ManagedComponent::isRunning() const noexcept {
    return state_.load() == ComponentState::running;
}

std::uint64_t stableHash(std::string_view value) noexcept {
    std::uint64_t hash = 1469598103934665603ULL;
    for (const unsigned char byte : value) {
        hash ^= static_cast<std::uint64_t>(byte);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::string componentStateName(ComponentState state) {
    switch (state) {
        case ComponentState::created: return "created";
        case ComponentState::configured: return "configured";
        case ComponentState::running: return "running";
        case ComponentState::stopped: return "stopped";
        case ComponentState::faulted: return "faulted";
    }
    return "unknown";
}

} // namespace elit21::platform
