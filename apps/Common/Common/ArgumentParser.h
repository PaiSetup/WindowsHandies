#pragma once

#include <string>
#include <vector>
#include <filesystem>

class ArgumentParser {
    template <typename T>
    using ConvertFunction = T (*)(const std::string &);

public:
    ArgumentParser(int argc, char **argv) {
        for (int i = 1; i < argc; i++) {
            this->args.push_back(argv[i]);
        }
    }

    // Helpers for single-name arguments
    template <typename ResultType>
    ResultType getArgumentValue(const std::string &name, const ResultType &defaultValue) {
        return getArgumentValue<ResultType>(std::vector<std::string>{name}, defaultValue);
    }
    template <typename ResultType>
    std::vector<ResultType> getArgumentValues(const std::string &name) {
        return getArgumentValues<ResultType>(std::vector<std::string>{name});
    }

    // Lookup methods, varying implementation for different types
    template <typename ResultType>
    ResultType getArgumentValue(const std::vector<std::string> &names, const ResultType &defaultValue);
    template <typename ResultType>
    std::vector<ResultType> getArgumentValues(const std::vector<std::string> &names);

private:
    // Conversion from string to various types

    template <typename ResultType>
    static ResultType convertFunction(const std::string &arg) {
        if constexpr (std::is_same_v<ResultType, wchar_t>) {
            return static_cast<ResultType>(arg[0]);
        }
        if constexpr (std::is_integral_v<ResultType>) {
            return static_cast<ResultType>(std::stoll(arg));
        }
        if constexpr (std::is_same_v<ResultType, std::string>) {
            return arg;
        }
        if constexpr (std::is_same_v<ResultType, std::wstring>) {
            std::wstring result(arg.size() + 1, L' ');
            const auto cap = result.capacity();
            size_t charsConverted{};
            const auto error = ::mbstowcs_s(&charsConverted, &result[0], result.capacity(), arg.c_str(), arg.size());
            result.resize(charsConverted - 1);
            return result;
        }
        if constexpr (std::is_same_v<ResultType, std::filesystem::path>) {
            return std::filesystem::path(arg);
        }
    }

    // Arguments captures
    std::vector<std::string> args;
};

template <>
inline bool ArgumentParser::getArgumentValue<bool>(const std::vector<std::string> &names, const bool &defaultValue) {
    for (auto &name : names) {
        auto nameIt = std::find(this->args.begin(), this->args.end(), name);
        if (nameIt != this->args.end()) {
            return true;
        }
    }
    return defaultValue;
}

template <typename ResultType>
inline ResultType ArgumentParser::getArgumentValue(const std::vector<std::string> &names, const ResultType &defaultValue) {
    for (auto &name : names) {
        auto nameIt = std::find(this->args.begin(), this->args.end(), name);
        if (nameIt == this->args.end())
            continue;

        auto valueIt = nameIt + 1;
        if (valueIt == this->args.end())
            continue;

        return ArgumentParser::convertFunction<ResultType>(*valueIt);
    }
    return defaultValue;
}

template <typename ResultType>
inline std::vector<ResultType> ArgumentParser::getArgumentValues(const std::vector<std::string> &names) {
    std::vector<ResultType> result{};
    for (auto &name : names) {
        auto nameIt = this->args.begin();
        while (true) {
            nameIt = std::find(nameIt, this->args.end(), name);
            if (nameIt == this->args.end()) {
                break;
            }

            auto valueIt = nameIt + 1;
            if (valueIt == this->args.end()) {
                break;
            }

            result.push_back(ArgumentParser::convertFunction<ResultType>(*valueIt));
            nameIt += 2;
        }
    }
    return result;
}
