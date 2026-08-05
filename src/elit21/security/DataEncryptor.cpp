#include "elit21/security/DataEncryptor.h"

#include "elit21/json/Json.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <array>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

namespace elit21::security {
namespace {

using CipherContext = std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;

std::string hex(const unsigned char* data, std::size_t size) {
    std::ostringstream output;
    output << std::hex << std::setfill('0');
    for (std::size_t index = 0; index < size; ++index) output << std::setw(2) << static_cast<int>(data[index]);
    return output.str();
}

Result<std::vector<unsigned char>> unhex(const std::string& value) {
    if (value.size() % 2 != 0) return Result<std::vector<unsigned char>>::failure("Invalid hexadecimal envelope");
    std::vector<unsigned char> output(value.size() / 2);
    try {
        for (std::size_t index = 0; index < output.size(); ++index) {
            output[index] = static_cast<unsigned char>(std::stoul(value.substr(index * 2, 2), nullptr, 16));
        }
    } catch (...) {
        return Result<std::vector<unsigned char>>::failure("Invalid hexadecimal envelope");
    }
    return Result<std::vector<unsigned char>>::success(std::move(output));
}

std::array<unsigned char, 32> deriveKey(const std::string& secret) {
    std::array<unsigned char, 32> key{};
    SHA256(reinterpret_cast<const unsigned char*>(secret.data()), secret.size(), key.data());
    return key;
}

std::vector<std::string> split(const std::string& value, char delimiter) {
    std::vector<std::string> output;
    std::istringstream input(value);
    std::string part;
    while (std::getline(input, part, delimiter)) output.push_back(part);
    return output;
}

} // namespace

DataEncryptor::DataEncryptor()
    : platform::BusinessComponent(
          "DataEncryptor", "AES-256-GCM authenticated local data encryption",
          platform::BusinessComponentSpec{"security", "encrypt", {}, true, false, 4U * 1024U * 1024U}) {}

Result<std::string> DataEncryptor::encrypt(const std::string& plaintext,
                                           const std::string& master_secret,
                                           const std::string& associated_data) {
    if (master_secret.size() < 16) return Result<std::string>::failure("Master secret must contain at least 16 characters");
    const auto key = deriveKey(master_secret);
    std::array<unsigned char, 12> nonce{};
    std::array<unsigned char, 16> tag{};
    if (RAND_bytes(nonce.data(), static_cast<int>(nonce.size())) != 1) {
        return Result<std::string>::failure("Unable to generate encryption nonce");
    }
    CipherContext context(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!context) return Result<std::string>::failure("Unable to allocate cipher context");
    if (EVP_EncryptInit_ex(context.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(nonce.size()), nullptr) != 1 ||
        EVP_EncryptInit_ex(context.get(), nullptr, nullptr, key.data(), nonce.data()) != 1) {
        return Result<std::string>::failure("Unable to initialize AES-256-GCM");
    }
    int written = 0;
    if (!associated_data.empty() && EVP_EncryptUpdate(
            context.get(), nullptr, &written,
            reinterpret_cast<const unsigned char*>(associated_data.data()),
            static_cast<int>(associated_data.size())) != 1) {
        return Result<std::string>::failure("Unable to authenticate associated data");
    }
    std::vector<unsigned char> ciphertext(plaintext.size() + 16);
    int total = 0;
    if (EVP_EncryptUpdate(context.get(), ciphertext.data(), &written,
                          reinterpret_cast<const unsigned char*>(plaintext.data()),
                          static_cast<int>(plaintext.size())) != 1) {
        return Result<std::string>::failure("Encryption failed");
    }
    total += written;
    if (EVP_EncryptFinal_ex(context.get(), ciphertext.data() + total, &written) != 1) {
        return Result<std::string>::failure("Encryption finalization failed");
    }
    total += written;
    ciphertext.resize(static_cast<std::size_t>(total));
    if (EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_GET_TAG, static_cast<int>(tag.size()), tag.data()) != 1) {
        return Result<std::string>::failure("Unable to retrieve authentication tag");
    }
    return Result<std::string>::success(
        "v1:" + hex(nonce.data(), nonce.size()) + ":" + hex(ciphertext.data(), ciphertext.size()) +
        ":" + hex(tag.data(), tag.size()));
}

Result<std::string> DataEncryptor::decrypt(const std::string& envelope,
                                           const std::string& master_secret,
                                           const std::string& associated_data) {
    if (master_secret.size() < 16) return Result<std::string>::failure("Master secret must contain at least 16 characters");
    const auto parts = split(envelope, ':');
    if (parts.size() != 4 || parts[0] != "v1") return Result<std::string>::failure("Unsupported encrypted envelope");
    auto nonce = unhex(parts[1]);
    auto ciphertext = unhex(parts[2]);
    auto tag = unhex(parts[3]);
    if (!nonce || !ciphertext || !tag || nonce.value().size() != 12 || tag.value().size() != 16) {
        return Result<std::string>::failure("Malformed encrypted envelope");
    }
    const auto key = deriveKey(master_secret);
    CipherContext context(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!context) return Result<std::string>::failure("Unable to allocate cipher context");
    if (EVP_DecryptInit_ex(context.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(nonce.value().size()), nullptr) != 1 ||
        EVP_DecryptInit_ex(context.get(), nullptr, nullptr, key.data(), nonce.value().data()) != 1) {
        return Result<std::string>::failure("Unable to initialize AES-256-GCM");
    }
    int written = 0;
    if (!associated_data.empty() && EVP_DecryptUpdate(
            context.get(), nullptr, &written,
            reinterpret_cast<const unsigned char*>(associated_data.data()),
            static_cast<int>(associated_data.size())) != 1) {
        return Result<std::string>::failure("Unable to authenticate associated data");
    }
    std::vector<unsigned char> plaintext(ciphertext.value().size() + 16);
    int total = 0;
    if (EVP_DecryptUpdate(context.get(), plaintext.data(), &written,
                          ciphertext.value().data(), static_cast<int>(ciphertext.value().size())) != 1) {
        return Result<std::string>::failure("Decryption failed");
    }
    total += written;
    if (EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_SET_TAG,
                            static_cast<int>(tag.value().size()), tag.value().data()) != 1) {
        return Result<std::string>::failure("Unable to set authentication tag");
    }
    if (EVP_DecryptFinal_ex(context.get(), plaintext.data() + total, &written) != 1) {
        return Result<std::string>::failure("Authentication failed: ciphertext, key or associated data is invalid");
    }
    total += written;
    return Result<std::string>::success(std::string(reinterpret_cast<char*>(plaintext.data()), static_cast<std::size_t>(total)));
}

platform::OperationResult DataEncryptor::execute(const platform::OperationContext& context) {
    auto parsed = Json::parse(context.payload);
    if (!parsed) return platform::OperationResult::failure("INVALID_JSON", parsed.error());
    const auto secret = parsed.value().getString("master_secret");
    const auto plaintext = parsed.value().getString("plaintext");
    auto encrypted = encrypt(plaintext, secret, parsed.value().getString("associated_data", "ELIT21"));
    if (!encrypted) return platform::OperationResult::failure("ENCRYPTION_FAILED", encrypted.error());
    auto result = platform::OperationResult::ok("AES-256-GCM envelope created");
    result.attributes["envelope"] = encrypted.value();
    result.metrics["plaintext_bytes"] = static_cast<double>(plaintext.size());
    return result;
}

} // namespace elit21::security
