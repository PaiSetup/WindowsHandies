#pragma once

#include <string>
#include <vector>

class ArgumentParser {
    template <typename T>
    using ConvertFunction = T (*)(const std::string &);

public:
    ArgumentParser(int argc, char **argv) {
        for (int i = 1; i < argc; i++) {
            this->args.push_back(argv[i]);
        }
    }

    // Helper for single-name arguments
    template <typename ResultType>
    ResultType getArgumentValue(const std::string &name, const ResultType &defaultValue) {
        return getArgumentValue<ResultType>(std::vector<std::string>{name}, defaultValue);
    }

    // Lookup method, varying implementation for different types
    template <typename ResultType>
    ResultType getArgumentValue(const std::vector<std::string> &names, const ResultType &defaultValue);

private:
    // Conversion from string to various types
    template <typename Integral, typename = std::enable_if_t<std::is_integral<Integral>::value>>
    static Integral convertFunction(const std::string &arg) {
        return static_cast<Integral>(std::stoll(arg));
    }
    template <typename String, typename = std::enable_if_t<std::is_same<String, std::string>::value>>
    static std::string convertFunction(const std::string &arg) {
        return arg;
    }
    template <typename WString, typename = std::enable_if_t<std::is_same<WString, std::wstring>::value>>
    static std::wstring convertFunction(const std::string &arg) {
        std::wstring result(arg.size() + 1, L' ');
        const auto cap = result.capacity();
        size_t charsConverted{};
        const auto error = ::mbstowcs_s(&charsConverted, &result[0], result.capacity(), arg.c_str(), arg.size());
        result.resize(charsConverted - 1);
        return result;
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
