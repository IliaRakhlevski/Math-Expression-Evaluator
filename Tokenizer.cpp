#include <cctype>
#include "Tokenizer.h"
using namespace std;



Tokenizer::Tokenizer() : tokens_preced_rules ({

		{ TokenType::NUMBER, { TokenType::PLUS, TokenType::MINUS, TokenType::MULT, TokenType::DIV, TokenType::RPAREN } },
		{ TokenType::LPAREN, { TokenType::NUMBER, TokenType::MINUS, TokenType::LPAREN} },
		{ TokenType::RPAREN, { TokenType::PLUS, TokenType::MINUS, TokenType::MULT, TokenType::DIV, TokenType::RPAREN } },
		{ TokenType::PLUS,   { TokenType::NUMBER, TokenType::LPAREN} },
		{ TokenType::MINUS,  { TokenType::NUMBER, TokenType::LPAREN} },
		{ TokenType::MULT,   { TokenType::NUMBER, TokenType::LPAREN} },
		{ TokenType::DIV,    { TokenType::NUMBER, TokenType::LPAREN} },
}) {}


TokenRetValue Tokenizer::tokenize(const std::string& expression)
{
	Token token;
	std::string str_num;
	int num_paren = 0;

	auto flush_number = [&]() -> TokenRetValue {
		if (str_num.empty()) return TokenRetValue::SUCCESS;

		try {
			size_t pos = 0;
			double num = std::stod(str_num, &pos);
			if (pos != str_num.size()) 
				return TokenRetValue::INVALID_NUMBER;
			tokens.push_back(num);
			str_num.clear();
			return TokenRetValue::SUCCESS;
		}
		catch (...) {
			return TokenRetValue::INVALID_NUMBER;
		}
	};

	tokens.clear();

	for (size_t i = 0; i < expression.length(); i++)
	{
		char c = expression[i];

		if (std::isdigit(static_cast<unsigned char>(c)) || c == '.')
		{
			str_num.push_back(c);
				continue;
		}	

		auto r = flush_number();
		if (r != TokenRetValue::SUCCESS) 
			return r;

		switch (c)
		{
			case ' ':
				break;
			case '+':	
				token = TokenOperator::PLUS;
				tokens.push_back(token);
				break;
			case '-':
				token = TokenOperator::MINUS;
				tokens.push_back(token);
				break;
			case '*':
				token = TokenOperator::MULT;
				tokens.push_back(token);
				break;
			case '/':
				token = TokenOperator::DIV;
				tokens.push_back(token);
				break;
			case '(':
				token = TokenParen::LPAREN;
				tokens.push_back(token);
				num_paren++;
				break;
			case ')':
				token = TokenParen::RPAREN;
				tokens.push_back(token);
				num_paren--;
				if (num_paren < 0)
					return TokenRetValue::UNBALANCED_PARENTHESIS;
				break;
			default:
				return TokenRetValue::INVALID_CHAR;
		};
	}

	auto r = flush_number();
	if (r != TokenRetValue::SUCCESS) return r;

	if(num_paren != 0)
		return TokenRetValue::UNBALANCED_PARENTHESIS;

	return TokenRetValue::SUCCESS;
}


TokenType Tokenizer::get_token_type(const Token& token)
{
	TokenType type = TokenType::NUMBER;

	switch (token.index()) {
		case 0: // double
		{
			type = TokenType::NUMBER;
			break;
		}
		case 1: // TokenOperator
		{
			TokenOperator op = get<1>(token);
			switch (op)
			{
			case TokenOperator::PLUS:
				type = TokenType::PLUS;
				break;
			case TokenOperator::MINUS:
				type = TokenType::MINUS;
				break;
			case TokenOperator::MULT:
				type = TokenType::MULT;
				break;
			case TokenOperator::DIV:
				type = TokenType::DIV;
				break;
			case TokenOperator::UNARY_MINUS:
				type = TokenType::UNARY_MINUS;
				break;
			};
			break;
		}
		case 2: // TokenParen
		{
			TokenParen par = get<2>(token);
			switch (par)
			{
			case TokenParen::LPAREN:
				type = TokenType::LPAREN;
				break;
			case TokenParen::RPAREN:
				type = TokenType::RPAREN;
				break;
			};
			break;
		}
	}

	return type;
}


bool Tokenizer::check_precedence(TokenType a, TokenType b)
{
	bool found = false;
	auto it_a = tokens_preced_rules.find(a);
	if (it_a == tokens_preced_rules.end()) 
		return false;
	vector<TokenType> vec = it_a->second;

	auto it_b = find(vec.begin(), vec.end(), b);
	if (it_b != vec.end())
		found = true;

	return found;
}


TokenRetValue Tokenizer::validate()
{
	if(tokens.size() == 0)
		return TokenRetValue::MATH_EXP_NOT_TOKENIZED;

	if (tokens.size() > 0)
	{
		// check first token
		if (!(get_token_type(tokens[0]) == TokenType::NUMBER ||
				get_token_type(tokens[0]) == TokenType::LPAREN || 
				get_token_type(tokens[0]) == TokenType::MINUS))
					return TokenRetValue::INVALID_MATH_EXP;

		// check last token
		if (!(get_token_type(tokens[tokens.size()-1]) == TokenType::NUMBER ||
				get_token_type(tokens[tokens.size()-1]) == TokenType::RPAREN))
					return TokenRetValue::INVALID_MATH_EXP;
	}

	if (tokens.size() > 1)
	{
		for (size_t i = 0; i < tokens.size() - 1; i++)
		{
			bool is_correct_preced = check_precedence(get_token_type(tokens[i]), get_token_type(tokens[i + 1]));
			if (!is_correct_preced)
				return TokenRetValue::INVALID_MATH_EXP;
		}
	}
	
	return TokenRetValue::SUCCESS;
}


void Tokenizer::update_unary_minus()
{
	if (tokens.size() < 2)
		return;

	if (get_token_type(tokens[0]) == TokenType::MINUS)
		tokens[0] = TokenOperator::UNARY_MINUS;

	for (size_t i = 0; i < tokens.size() - 1; i++)
	{
		if(get_token_type(tokens[i])==TokenType::LPAREN && get_token_type(tokens[i+1])==TokenType::MINUS)
			tokens[i+1] = TokenOperator::UNARY_MINUS;
	}
}


std::vector<Token> Tokenizer::get_tokens() const
{
	return tokens;
}