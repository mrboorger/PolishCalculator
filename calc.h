#pragma once

#include <string>
#include <vector>
#include <stack>

class Calculator {
 public:
  static int Calculate(const std::string& expression);

 private:
  struct Token {
    enum class TokenType {
      kUnknown,
      kConstant,
      kUnaryMinus,
      kAddition,
      kMultiply,
      kDivide,
      kSubtraction,
      kAbs,
      kMod,
      kOpeningBrace,
      kClosingBrace,
    };
    int value;
    TokenType token_type;

    Token();
    explicit Token(const std::string& str);
    explicit Token(int number);

    int Priority() const;
    int CountOperands() const;
  };
  static std::vector<Token> StringToVectorOfToken(const std::string& str_expr);
  static std::vector<Token> ConvertToPolishNotation
      (const std::vector<Token>& tokens);

  std::stack<int> stack_values_;

  void Process(const Calculator::Token& token);

  friend class CalculatorTester;
};
