#include "ShuntingYard.h"
using namespace std;


ShuntingYard::ShuntingYard(const Tokenizer& tokenizer) : operator_precedence_associative({
	{ TokenType::PLUS,			{Precedence::LOW, Associative::LEFT} },
	{ TokenType::MINUS,			{Precedence::LOW, Associative::LEFT} },
	{ TokenType::MULT,			{Precedence::MID, Associative::LEFT} },
	{ TokenType::DIV,			{Precedence::MID, Associative::LEFT} },
	{ TokenType::UNARY_MINUS,	{Precedence::HIGH, Associative::RIGHT} },
})
{
	tokens = tokenizer.get_tokens();
}


Precedence ShuntingYard::get_precedence(const Token& token)
{
	TokenType token_type = Tokenizer::get_token_type(token);
	auto it = operator_precedence_associative.find(token_type);
	if (it == operator_precedence_associative.end())
		return Precedence::UNKNOWN;
	pair<Precedence, Associative> data = it->second;
	return data.first;
}


Associative ShuntingYard::get_associative(const Token& token)
{
	TokenType token_type = Tokenizer::get_token_type(token);
	auto it = operator_precedence_associative.find(token_type);
	if (it == operator_precedence_associative.end())
		return Associative::UNKNOWN;
	pair<Precedence, Associative> data = it->second;
	return data.second;
}


ComparePrecedenceRes ShuntingYard::compare_precedences(const Token& token_a, const Token& token_b)
{
	ComparePrecedenceRes res = ComparePrecedenceRes::UNKNOWN;

	Precedence prec_a = get_precedence(token_a);
	if (prec_a == Precedence::UNKNOWN)
		return res;

	Precedence prec_b = get_precedence(token_b);
	if (prec_b == Precedence::UNKNOWN)
		return res;

	if (static_cast<int>(prec_a) > static_cast<int>(prec_b))
		res = ComparePrecedenceRes::GREATER;
	else if (static_cast<int>(prec_a) == static_cast<int>(prec_b))
		res = ComparePrecedenceRes::EQUAL;
	else if (static_cast<int>(prec_a) < static_cast<int>(prec_b))
		res = ComparePrecedenceRes::LESS;

	return res;
}


bool ShuntingYard::is_operator(const Token& token)
{
	bool res = false;
	TokenType token_type = Tokenizer::get_token_type(token);
	switch (token_type)
	{
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::MULT:
	case TokenType::DIV:
	case TokenType::UNARY_MINUS:
		res = true;
		break;
	}
	return res;
}


void ShuntingYard::process_operator(Token& token)
{
	while (!op_stack.empty())
	{
		Token top_token = op_stack.top();
		bool is_top_oper = is_operator(top_token);

		if (!is_top_oper) // not operator
			break;

		ComparePrecedenceRes prec_res = compare_precedences(top_token, token);
		if (prec_res == ComparePrecedenceRes::UNKNOWN) { /* TBD */ }

		Associative ass_res = get_associative(token);
		if(ass_res == Associative::UNKNOWN) { /* TBD */ }

		if ( prec_res == ComparePrecedenceRes::GREATER || (prec_res == ComparePrecedenceRes::EQUAL && ass_res == Associative::LEFT) )
		{
			rpn.push_back(top_token);
			op_stack.pop();
		}	
	}

	op_stack.push(token);
}


void ShuntingYard::process_right_paren(Token& token)
{
	while (!op_stack.empty())
	{
		Token top_token = op_stack.top();
		TokenType type = Tokenizer::get_token_type(top_token);
		if (type != TokenType::LPAREN)
		{
			rpn.push_back(top_token);
			op_stack.pop();
		}
		else
			break;
	}
	if(!op_stack.empty())
		op_stack.pop();
}


void ShuntingYard::create_rpn()
{
	if (tokens.size() == 0)
		return;

	op_stack = {};
	rpn.clear();

	for (auto token : tokens)
	{
		TokenType token_type = Tokenizer::get_token_type(token);
		switch (token_type)
		{
		case TokenType::NUMBER:
			rpn.push_back(token);
			break;
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MULT:
		case TokenType::DIV:
		case TokenType::UNARY_MINUS:
			process_operator(token);
			break;
		case TokenType::LPAREN:
			op_stack.push(token);
			break;
		case TokenType::RPAREN:
			process_right_paren(token);
			break;
		}
	}

	while (!op_stack.empty())
	{
		Token top_token = op_stack.top();
		rpn.push_back(top_token);
		op_stack.pop();
	}
}

vector<Token> ShuntingYard::get_rpn() const
{
	return rpn;
}