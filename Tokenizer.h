#pragma once
#include <variant>
#include <vector>
#include <string>
#include <map>


enum class TokenParen { LPAREN, RPAREN };

enum class TokenOperator { PLUS, MINUS, MULT, DIV, UNARY_MINUS};

enum class TokenType { NUMBER, PLUS, MINUS, MULT, DIV, LPAREN, RPAREN, UNARY_MINUS };



enum class TokenRetValue { SUCCESS, INVALID_CHAR, INVALID_NUMBER, UNBALANCED_PARENTHESIS, INVALID_MATH_EXP, MATH_EXP_NOT_TOKENIZED};



using Token = std::variant<double, TokenOperator, TokenParen>;

class Tokenizer
{
	std::vector<Token> tokens;
	std::map<TokenType, std::vector<TokenType>> tokens_preced_rules;

public:

	Tokenizer();

	TokenRetValue tokenize(const std::string& expression);

	TokenRetValue validate();

	void update_unary_minus();

	std::vector<Token> get_tokens()const;

	static TokenType get_token_type(const Token& token);


private:
	
	bool check_precedence(TokenType a, TokenType b);	

};

