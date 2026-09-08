#pragma once

#include <QObject>

#include <QString>
#include <vector>
#include <stack>
#include <queue>
#include <string_view>
#include <array>
#include <cstddef>

enum class TokenType {
    Number, Operator,
    OpenParen, CloseParen,
    Constant
};

enum class OpType {
    None,
    Add,  Subtract,
    Multiply, Divide,
    Sqrt, Pow,
    Fact,
    Sin, Cos, Tan, Ctg,
    Log, Ln, Lg
};

enum class Associativity { Left, Right };

struct Token {
    TokenType type = TokenType::Number;
    OpType op = OpType::None;
    std::string_view symbol = "";
    double value = 0.0, decimalScale = 0.0;
    std::string dynamicSymbol = "";
};

constexpr int numberToCutBy(std::string_view token) {
    if (token.empty()) return 0;
    if (token == "(" || token == ")") return 1;

    if ((token[0] >= '0' && token[0] <= '9') || token == "." || token == "π" || token == "e")
        return static_cast<int>(token.length());

    return static_cast<int>(token.length()) + 2;
}

// Lookup tables indexed by OpType
// A table lookup is a single load, no branches/jump-table indirection and no
// risk of branch misprediction, unlike a switch over 14 non-contiguous cases.
inline constexpr std::array<unsigned char, 15> kPrecedenceTable = {
    0, // None
    1, 1,          // Add, Subtract
    2, 2,          // Multiply, Divide
    4,             // Sqrt
    3,             // Pow
    4,             // Fact
    4, 4, 4, 4,    // Sin, Cos, Tan, Ctg
    4, 4, 4        // Log, Ln, Lg
};

inline constexpr std::array<bool, 15> kUnaryTable = {
    false,               // None
    false, false,        // Add, Subtract
    false, false,        // Multiply, Divide
    true,                // Sqrt
    false,               // Pow
    true,                // Fact
    true, true, true, true, // Sin, Cos, Tan, Ctg
    true, true, true     // Log, Ln, Lg
};

constexpr unsigned short int getPrecedence(OpType op) {
    return kPrecedenceTable[static_cast<std::size_t>(op)];
}

constexpr bool isUnaryOp(OpType op) {
    return kUnaryTable[static_cast<std::size_t>(op)];
}

constexpr Associativity getAssociativity(OpType op) {
    return (op == OpType::Pow) ? Associativity::Right : Associativity::Left;
}

class Calculator : public QObject {
    Q_OBJECT

public:
    Calculator() {
        tokens.reserve(64);
        std::vector<Token> reserved;
        reserved.reserve(32);
        operatorStack = std::stack<Token, std::vector<Token>>(std::move(reserved));
    }
    ~Calculator() {
        tokens.clear();
        openParenthesesCount = 0;
    }

    std::vector<Token> tokens;
    std::stack<Token, std::vector<Token>> operatorStack;
    std::queue<Token> output;

    int openParenthesesCount = 0;
    double lastResult = 0.0;
    bool hasResult = false;

    static Token createToken(const QString &op);

    Q_INVOKABLE bool addOp(const QString &op);
    Q_INVOKABLE QString addOperation(const QString &op, const QString &currentText);
    Q_INVOKABLE bool addParanthese(bool open);

    Q_INVOKABLE QString deleteLast();
    Q_INVOKABLE int numberToCutBy(const QString &token) const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE float evaluate();
    float calculate();
};