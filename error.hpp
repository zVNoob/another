#pragma once

#include "token.hpp"

struct Error {
	Token* Where;
	char* What;
	Error(Token* Where, char* What = 0) :
	Where(Where),What(What) {}
};
