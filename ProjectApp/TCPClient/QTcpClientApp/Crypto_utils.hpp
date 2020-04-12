#ifndef CRYPTO_UTILS_HPP
#define CRYPTO_UTILS_HPP

#include "InfInt.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

namespace crypto {

using string = std::string;

auto inf_gcd(const InfInt& x, const InfInt& y) -> InfInt {
    if (y == 0) return x;
    return inf_gcd(y, x % y);
}

auto inf_gcdExtended(const InfInt& x, const InfInt& y, InfInt& a, InfInt& b) -> InfInt {
    // Base Case
    if (x == 0) {
        a = 0;
        b = 1;
        return y;
    }

    InfInt a1, b1; // To store results of recursive call
    auto gcd = inf_gcdExtended(y % x, x, a1, b1);

    // Update x and y using results of
    // recursive call
    a = b1 - (y / x) * a1;
    b = a1;

    return gcd;
}

auto inf_pow(const InfInt& x, const InfInt& y, const InfInt& my_mod) -> InfInt {
    InfInt temp;
    if (y == 0)
        return 1;

    temp = inf_pow(x, y / 2, my_mod);
    if ((y % 2) == 0)
        return temp * temp % my_mod;
    else
        return x * temp * temp % my_mod;
}

auto inf_pow_nomod(const InfInt& x, const InfInt& y) -> InfInt {
    InfInt temp;
    if (y == 0)
        return 1;

    temp = inf_pow_nomod(x, y / 2);
    if ((y % 2) == 0)
        return temp * temp;
    else
        return x * temp * temp;
}

auto inf_log2(InfInt& n) -> InfInt {
    InfInt logValue = -1;
    while (n > 0) {
        logValue++;
        n /= 2;
    }
    return logValue;
}

auto Jacobi(const InfInt& a, const InfInt& b) -> InfInt {
    if (a == 1) return 1;
    if (a % 2 == 0) return Jacobi(a / 2, b) * inf_pow(-1, (inf_pow_nomod(b, 2) - 1) / 8, 2);
    return Jacobi(b % a, a) * inf_pow(-1, (a - 1) * (b - 1) / 4, 2);
}

auto getRandomNumber(uint32_t n) -> InfInt {
    static auto rng = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
    static auto generator = std::uniform_int_distribution<std::mt19937::result_type>{0, 9};

    string str_result;
    auto value = generator(rng);

    while (value == 0)
        value = generator(rng);

    str_result += std::to_string(value);

    for (uint32_t i{1}; i < n; ++i)
        str_result += std::to_string(generator(rng));
    auto result = InfInt{str_result};

    return result;
}

auto getNDigitRandomPrimeNumber(uint32_t n) {
    InfInt result;

    while (true) {
        auto b = getRandomNumber(n);

        if (b % 2 == 0) ++b;
        if (b % 5 == 0) b += 2;

        int cycle_size = 1;
        int i{};
        for (; i < cycle_size; ++i) {
            auto a = getRandomNumber(n - 1);
            if(inf_gcd(a, b) != 1 || Jacobi(a, b) != (inf_pow(a, (b - 1) / 2, b))) break;
        }

        if (i == cycle_size) {
            result = b;
            break;
        }
    }
    return result;
}

auto calculateD_E(const InfInt& p, const InfInt& q, const InfInt& n) {
    auto fi_n = n - (p + q) + 1;
    auto n_here = n;
    InfInt max;
    auto calculateD = [&max](const InfInt& p, const InfInt& q) -> InfInt {
        if (p > q)
            max = p;
        else
            max = q;
        auto random_num = getNDigitRandomPrimeNumber(max.numberOfDigits() + 1);
        max = random_num;
        return max;
    };

    InfInt a, b, d;
    do {
        d = calculateD(p, q);
        a = 0, b = 0;
        inf_gcdExtended(fi_n, d, a, b);
    } while (inf_log2(n_here) > b);

    return std::make_pair(d, b);
}

auto bitsetInfInt(InfInt number) {
    string str;
    for (size_t i{0}; number > 0; ++i) {
        str += (number % 2).toString();
        number /= 2;
    }

    std::reverse(std::begin(str), std::end(str));
    return str;
}

auto encryptMessage(const string& message, const InfInt& key, const InfInt& n) -> string {
    auto binary = bitsetInfInt(key);
    string result;

    InfInt c;
    for (const auto& letter : message) {
        c = 1;
        for (const auto& bit : binary) {
            c = inf_pow(c, 2, n);
            if (bit == '1')
                c = c * letter % n;
        }

        for (size_t i{c.numberOfDigits()}; i < n.numberOfDigits(); ++i)
            result += '0';

        result += c.toString();
    }
    return result;
}

auto decryptMessage(const string& message, const InfInt& key, const InfInt& n) -> string {
    auto binary = bitsetInfInt(key);
    auto message_format = std::vector<InfInt>{};
    string result;

    for (size_t i{}; i < message.length(); i += n.numberOfDigits())
        message_format.emplace_back(message.substr(i, n.numberOfDigits()));

    InfInt c;
    for (const auto& letter : message_format) {
        c = 1;
        for (const auto& bit : binary) {
            c = inf_pow(c, 2, n);
            if (bit == '1')
                c = c * letter % n;
        }
        result += static_cast<char>(c.toInt());
    }
    return result;
}

auto test_encryption(const string& message) {
    auto digits = 10;
    auto p = getNDigitRandomPrimeNumber(digits);
    auto q = getNDigitRandomPrimeNumber(digits);
    auto n = p * q;
    auto [d, e] = calculateD_E(p, q, n);
    auto encrypted_msg = encryptMessage(message, e, n);
    auto decrypted_msg = decryptMessage(encrypted_msg, d, n);

    std::cout << "p: " << p << std::endl;
    std::cout << "q: " << q << std::endl;
    std::cout << "e: " << e << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << decrypted_msg << std::endl;
}

}
#endif // CRYPTO_UTILS_HPP
