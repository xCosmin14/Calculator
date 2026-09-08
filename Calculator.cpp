#include "Calculator.h"

#include <QHash>
#include <array>

#include <cmath>

inline constexpr double PI = 3.14159265;
inline constexpr double E  = 2.71828;

//token calculator transforms tokens into simple, memory contiguous structs
//eliminating the need for (Q)strings.

//Normal string comparisons skip lots of CPU cycles, which produce cache misses
//and idle time that is not used, due to searching through uncontinuous memory zones

//A vector of structs is a contiguous memory zone and a search is made in
//a single processor cycle

Token Calculator::createToken(const QString &op) {
    if (op.length() == 1 && op[0].isDigit()) {
        static constexpr std::array<std::string_view, 10> digits = {
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
        };
        return { TokenType::Number, OpType::None, digits[op[0].digitValue()] };
    }

    static const QHash<QString, Token> tokenMap = {
        { ".",    { TokenType::Number,     OpType::None,     "." } },
        { "π",    { TokenType::Constant,   OpType::None,     "π", PI } },
        { "e",    { TokenType::Constant,   OpType::None,     "e", E } },
        { "(",    { TokenType::OpenParen,  OpType::None,     "(" } },
        { ")",    { TokenType::CloseParen, OpType::None,     ")" } },
        { "+",    { TokenType::Operator,   OpType::Add,      "+" } },
        { "-",    { TokenType::Operator,   OpType::Subtract, "-" } },
        { "×",    { TokenType::Operator,   OpType::Multiply, "×" } },
        { "/",    { TokenType::Operator,   OpType::Divide,   "/" } },
        { "√",    { TokenType::Operator,   OpType::Sqrt,     "√" } },
        { "pow",  { TokenType::Operator,   OpType::Pow,      "pow" } },
        { "fact", { TokenType::Operator,   OpType::Fact,     "fact" } },
        { "sin",  { TokenType::Operator,   OpType::Sin,      "sin" } },
        { "cos",  { TokenType::Operator,   OpType::Cos,      "cos" } },
        { "tan",  { TokenType::Operator,   OpType::Tan,      "tan" } },
        { "ctg",  { TokenType::Operator,   OpType::Ctg,      "ctg" } },
        { "log",  { TokenType::Operator,   OpType::Log,      "log" } },
        { "ln",   { TokenType::Operator,   OpType::Ln,       "ln" } },
        { "lg",   { TokenType::Operator,   OpType::Lg,       "lg" } }
    };

    return tokenMap.value(op, { TokenType::Number, OpType::None, "" });
}

bool Calculator::addOp(const QString &op) {
    const bool isDot = (op.size() == 1 && op[0] == QLatin1Char('.'));
    const bool isDigit = (op.size() == 1 && op[0].isDigit());

    if (isDot || isDigit) {
        const char c = op[0].toLatin1();

        if (!tokens.empty() && tokens.back().type == TokenType::Number) {
            Token &back = tokens.back();

            if (isDot) {
                if (back.dynamicSymbol.find('.') != std::string::npos)
                    return false;
                back.dynamicSymbol.push_back(c);
                back.decimalScale = 1.0;
                return true;
            }

            back.dynamicSymbol.push_back(c);

            if (back.decimalScale == 0.0) {
                back.value = back.value * 10.0 + static_cast<double>(c - '0');
            } else {
                back.decimalScale *= 0.1;
                back.value += static_cast<double>(c - '0') * back.decimalScale;
            }
        } else {
            Token numToken;
            numToken.type = TokenType::Number;
            numToken.dynamicSymbol.push_back(c);
            numToken.value = isDot ? 0.0 : static_cast<double>(c - '0');
            if (isDot) numToken.decimalScale = 1.0; // next digit starts the fractional part
            tokens.push_back(std::move(numToken));
        }
        return true;
    }

    tokens.push_back(createToken(op));
    return true;
}

bool Calculator::addParanthese(bool open) {
    if (open) {
        if (!tokens.empty()) {
            const Token& lastToken = tokens.back();

            if (lastToken.type != TokenType::Operator && lastToken.type != TokenType::OpenParen)
                tokens.push_back(createToken("×"));
        }

        tokens.push_back(createToken("("));
        openParenthesesCount++;
        return true;
    } else {
        if (openParenthesesCount == 0) return false;
        if (!tokens.empty() && tokens.back().type == TokenType::OpenParen) return false;

        tokens.push_back(createToken(")"));
        openParenthesesCount--;
        return true;
    }
}

QString Calculator::deleteLast() {
    if (tokens.empty()) return "";

    Token &lastToken = tokens.back();

    if (lastToken.type == TokenType::Number && lastToken.dynamicSymbol.length() > 1) {
        char removedChar = lastToken.dynamicSymbol.back();
        lastToken.dynamicSymbol.pop_back();
        lastToken.value = std::stod(lastToken.dynamicSymbol);

        const auto dotPos = lastToken.dynamicSymbol.find('.');
        if (dotPos == std::string::npos) {
            lastToken.decimalScale = 0.0;
        } else {
            const auto fractionalDigits = lastToken.dynamicSymbol.size() - dotPos - 1;
            double scale = 1.0;
            for (std::size_t i = 0; i <= fractionalDigits; ++i) scale *= 0.1;
            lastToken.decimalScale = scale;
        }

        return QString(removedChar);
    }

    if (lastToken.type == TokenType::OpenParen) openParenthesesCount--;
    else if (lastToken.type == TokenType::CloseParen) openParenthesesCount++;

    std::string sym = lastToken.dynamicSymbol.empty() ? std::string(lastToken.symbol) : lastToken.dynamicSymbol;

    tokens.pop_back();
    return QString::fromStdString(sym);
}

int Calculator::numberToCutBy(const QString &token) const {
    return ::numberToCutBy(token.toStdString());
}

void Calculator::clear() {
    tokens.clear();
    openParenthesesCount = 0;
}

float Calculator::calculate() {
    std::stack<double, std::vector<double>> resultStack;

    while (!output.empty()) {
        Token t = std::move(output.front());
        output.pop();

        if (t.type == TokenType::Number || t.type == TokenType::Constant)
            resultStack.push(t.value);
        else if (t.type == TokenType::Operator) {
            if (isUnaryOp(t.op)) {
                if (resultStack.empty()) { clear(); return 0.0f; }

                double val = resultStack.top(), res=0.0;
                resultStack.pop();

                switch (t.op) {
                    case OpType::Sqrt: res = (val >= 0) ? std::sqrt(val) : 0.0; break;
                    case OpType::Sin:  res = std::sin(val); break;
                    case OpType::Cos:  res = std::cos(val); break;
                    case OpType::Tan:  res = std::tan(val); break;
                    case OpType::Ln:   res = (val > 0) ? std::log(val) : 0.0; break;
                    case OpType::Log:  res = (val > 0) ? std::log2(val) : 0.0; break;
                    case OpType::Lg:   res = (val > 0) ? std::log10(val) : 0.0; break;
                    case OpType::Fact: res = std::tgamma(val+1); break;
                default: break;
                }
                resultStack.push(res);

            } else {
                if (resultStack.size() < 2) { clear(); return 0.0f; }

                double right = resultStack.top(); resultStack.pop();
                double left = resultStack.top();  resultStack.pop();
                double res = 0.0;

                switch (t.op) {
                    case OpType::Add:      res = left + right; break;
                    case OpType::Subtract: res = left - right; break;
                    case OpType::Multiply: res = left * right; break;
                    case OpType::Divide:   res = (right != 0) ? (left / right) : 0.0; break;
                    case OpType::Pow:      res = std::pow(left, right); break;
                    default: break;
                }
                resultStack.push(res);
            }
        }
    }

    float finalResult = resultStack.empty() ? 0.0f : static_cast<float>(resultStack.top());

    lastResult = static_cast<double>(finalResult);
    hasResult = true;

    clear();

    return finalResult;
}

float Calculator::evaluate() {
    for (Token &t : tokens) {
        if (t.type == TokenType::Number || t.type == TokenType::Constant) output.push(std::move(t));

        if (t.type == TokenType::Operator) {
            while (!operatorStack.empty() && operatorStack.top().type != TokenType::OpenParen) {
                const int topPrec = getPrecedence(operatorStack.top().op);
                const int currPrec = getPrecedence(t.op);

                if (topPrec > currPrec ||
                    (topPrec == currPrec && getAssociativity(t.op) == Associativity::Left)) {
                    output.push(std::move(operatorStack.top()));
                    operatorStack.pop();
                } else break;
            }
            operatorStack.push(std::move(t));
        }

        if (t.type == TokenType::OpenParen) operatorStack.push(std::move(t));
        if (t.type == TokenType::CloseParen) {
            while (!operatorStack.empty() && operatorStack.top().type != TokenType::OpenParen) {
                output.push(std::move(operatorStack.top()));
                operatorStack.pop();
            }

            if (!operatorStack.empty() && operatorStack.top().type == TokenType::OpenParen)
                operatorStack.pop();
        }
    }

    while (!operatorStack.empty()) {
        output.push(std::move(operatorStack.top()));
        operatorStack.pop();
    }

    if (openParenthesesCount) {
        hasResult = false;
        return 0.0f;
    }
    return calculate();
}

QString Calculator::addOperation(const QString &op, const QString &currentText) {
    enum class UICommand { None, AC, Del, Calc };
    static const QHash<QString, UICommand> cmdMap = {
        {"AC", UICommand::AC},
        {"del", UICommand::Del},
        {"calc", UICommand::Calc}
    };

    switch (cmdMap.value(op, UICommand::None)) {
    case UICommand::AC:
        clear();
        output = std::queue<Token>();
        operatorStack = std::stack<Token, std::vector<Token>>();
        hasResult = false;
        lastResult = 0.0;
        return "";

    case UICommand::Del: {
        QString deletedToken = deleteLast();
        if (deletedToken.isEmpty()) return currentText;

        int cutLength = numberToCutBy(deletedToken);
        return currentText.length() >= cutLength
                   ? currentText.left(currentText.length() - cutLength) : "";
    }

    case UICommand::Calc: {
        const float result = evaluate();
        return QString::number(result, 'g', 10);
    }

    case UICommand::None:
        break;
    }

    Token t = createToken(op);

    if (tokens.empty() && hasResult &&
        (t.type == TokenType::Operator || t.type == TokenType::OpenParen)) {
        Token seed;
        seed.type = TokenType::Number;
        seed.value = lastResult;
        seed.dynamicSymbol = QString::number(lastResult, 'g', 10).toStdString();
        tokens.push_back(std::move(seed));
    }
    hasResult = false;

    switch (t.type) {
    case TokenType::OpenParen:
    case TokenType::CloseParen: {
        size_t prevSize = tokens.size();

        if (addParanthese(t.type == TokenType::OpenParen))
            return (tokens.size() - prevSize == 2) ? currentText + " × (" : currentText + op;
        return currentText;
    }

    case TokenType::Number:
        if (addOp(op)) return currentText + op;
        return currentText;

    case TokenType::Constant:
        addOp(op);
        return currentText + op;

    case TokenType::Operator:
        addOp(op);
        return (t.op == OpType::Sqrt) ? currentText + " " + op : currentText + " " + op + " ";
    }

    return currentText;
}