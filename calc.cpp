#include "calc.h"

#include <stack>
#include <stdexcept>

int Calculator::Calculate(const std::string& expression) {
  Calculator calculator;
  std::vector<Token> polish_tokens = StringToVectorOfToken(expression);
  polish_tokens = ConvertToPolishNotation(polish_tokens);
  for (const auto& token : polish_tokens) {
    calculator.Process(token);
  }
  return calculator.stack_values_.top();
}

std::vector<Calculator::Token> Calculator::StringToVectorOfToken
    (const std::string& str_expr) {
  std::vector<Calculator::Token> vec_expr;
  std::string cur_str_token;
  for (char symbol : str_expr) {
    if (isspace(symbol)) {
      continue;
    }
    if (cur_str_token.empty()
        || (isdigit(cur_str_token.back()) && isdigit(symbol))
        || (isalpha(cur_str_token.back()) && isalpha(symbol))) {
      cur_str_token += symbol;
    } else {
      vec_expr.emplace_back(cur_str_token);
      cur_str_token = symbol;
    }
  }
  vec_expr.emplace_back(cur_str_token);
  return vec_expr;
}
std::vector<Calculator::Token> Calculator::ConvertToPolishNotation
    (const std::vector<Token>& tokens) {
  std::vector<Calculator::Token> polish_tokens;
  std::stack<Token> stack;
  for (size_t i = 0; i < tokens.size(); ++i) {
    const Token& cur_token = tokens[i];
    switch (cur_token.token_type) {
      case Token::TokenType::kConstant: {
        polish_tokens.push_back(cur_token);
        break;
      }
      case Token::TokenType::kUnaryMinus:
      case Token::TokenType::kAbs:
      case Token::TokenType::kOpeningBrace: {
        stack.push(cur_token);
        break;
      }
      case Token::TokenType::kClosingBrace: {
        while (stack.top().token_type != Token::TokenType::kOpeningBrace) {
          polish_tokens.push_back(stack.top());
          stack.pop();
        }
        stack.pop();
        break;
      }
      case Token::TokenType::kSubtraction: {
        if (i == 0
            || tokens[i - 1].token_type == Token::TokenType::kOpeningBrace) {
          stack.push(cur_token);
          stack.top().token_type = Token::TokenType::kUnaryMinus;
          break;
        }
      }
      case Token::TokenType::kMultiply:
      case Token::TokenType::kDivide:
      case Token::TokenType::kMod:
      case Token::TokenType::kAddition: {
        while (!stack.empty()
            && stack.top().Priority() >= cur_token.Priority()) {
          polish_tokens.push_back(stack.top());
          stack.pop();
        }
        stack.push(cur_token);
        break;
      }
      case Token::TokenType::kUnknown:
      default: {
        throw std::logic_error("Unknown token");
        break;
      }
    }
  }
  while (!stack.empty()) {
    polish_tokens.push_back(stack.top());
    stack.pop();
  }
  return polish_tokens;
}

void Calculator::Process(const Calculator::Token& token) {
  if (token.CountOperands() == 0) {
    stack_values_.push(token.value);
  } else if (token.CountOperands() == 1) {
    int cur_value = stack_values_.top();
    stack_values_.pop();
    switch (token.token_type) {
      case Token::TokenType::kUnaryMinus: {
        stack_values_.push(-cur_value);
        break;
      }
      case Token::TokenType::kAbs: {
        stack_values_.push(std::abs(cur_value));
        break;
      }
      default: {
        break;
      }
    }
  } else {
    int value_r = stack_values_.top();
    stack_values_.pop();
    int value_l = stack_values_.top();
    stack_values_.pop();
    switch (token.token_type) {
      case Token::TokenType::kUnaryMinus: {
        stack_values_.push(value_l - value_r);
        break;
      }
      case Token::TokenType::kAddition: {
        stack_values_.push(value_l + value_r);
        break;
      }
      case Token::TokenType::kSubtraction: {
        stack_values_.push(value_l - value_r);
        break;
      }
      case Token::TokenType::kMultiply: {
        stack_values_.push(value_l * value_r);
        break;
      }
      case Token::TokenType::kDivide: {
        if (value_r == 0) {
          throw std::runtime_error("Division by zero");
        }
        stack_values_.push(value_l / value_r);
        break;
      }
      case Token::TokenType::kMod: {
        if (value_r == 0) {
          throw std::runtime_error("Dovision by zero");
        }
        stack_values_.push(value_l % value_r);
        break;
      }
      default: {
        break;
      }
    }
  }
}

Calculator::Token::Token() : value(0), token_type(TokenType::kUnknown) {}

Calculator::Token::Token(const std::string& str) {
  value = 0;
  if (isdigit(str.back())) {
    value = stoi(str);
    token_type = TokenType::kConstant;
  } else if (str == "+") {
    token_type = TokenType::kAddition;
  } else if (str == "-") {
    token_type = TokenType::kSubtraction;
  } else if (str == "*") {
    token_type = TokenType::kMultiply;
  } else if (str == "/") {
    token_type = TokenType::kDivide;
  } else if (str == "%") {
    token_type = TokenType::kMod;
  } else if (str == "abs") {
    token_type = TokenType::kAbs;
  } else if (str == "(") {
    token_type = TokenType::kOpeningBrace;
  } else if (str == ")") {
    token_type = TokenType::kClosingBrace;
  } else {
    token_type = TokenType::kUnknown;
  }
}

Calculator::Token::Token(int number) {
  value = number;
  token_type = TokenType::kConstant;
}

int Calculator::Token::Priority() const {
  switch (token_type) {
    case TokenType::kUnaryMinus:
    case TokenType::kAbs: {
      return 2;
      break;
    }
    case TokenType::kMultiply:
    case TokenType::kDivide:
    case TokenType::kMod: {
      return 1;
      break;
    }
    case TokenType::kAddition:
    case TokenType::kSubtraction: {
      return 0;
    }
    default: {
      return -1;
    }
  }
}

int Calculator::Token::CountOperands() const {
  switch (token_type) {
    case TokenType::kUnaryMinus:
    case TokenType::kAbs: {
      return 1;
    }
    case TokenType::kAddition:
    case TokenType::kSubtraction:
    case TokenType::kMultiply:
    case TokenType::kDivide:
    case TokenType::kMod: {
      return 2;
    }
    default: {
      return 0;
    }
  }
}
