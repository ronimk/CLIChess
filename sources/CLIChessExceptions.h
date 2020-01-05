#pragma once
#include <exception>
#include <string>

class Exception {
private:
	std::string explanation;

public:
	Exception(std::string _explanation) { explanation = _explanation; }
	std::string what() const { return explanation; }
};

class ParseException : public Exception {

public:
	ParseException(std::string _explanation) : Exception(_explanation) {}
};

class SquareValidationException : public Exception {

public:
	SquareValidationException(std::string _explanation) : Exception(_explanation) {}
};

class IllegalMoveException : public Exception {

public:
	IllegalMoveException(std::string _explanation) : Exception(_explanation) {}
};

class BoardAccessException : public Exception {
public:
	BoardAccessException(std::string _explanation) : Exception(_explanation) {}
};