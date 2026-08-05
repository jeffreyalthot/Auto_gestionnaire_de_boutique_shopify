#pragma once
#include <cstddef>
#include <string>
namespace elit21::tests::mocks {
class MockClock {
public:
    void record(std::string operation) { last_operation_ = std::move(operation); ++calls_; }
    [[nodiscard]] std::size_t calls() const noexcept { return calls_; }
    [[nodiscard]] const std::string& lastOperation() const noexcept { return last_operation_; }
private:
    std::size_t calls_{0};
    std::string last_operation_;
};
}
