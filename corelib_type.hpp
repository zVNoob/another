#pragma once

#include "object.hpp"
#include <sstream>
#include <string>

namespace corelib {

static Object *Integer(Object_Array &Args, Object_Env &Env) {
	// decimal
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		return new Object_Int(static_cast<Object_Decimal *>(Args.Value[0])->Value);
	}
	// int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		return Args.Value[0]->clone();
	// string
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		Str s = static_cast<Object_String *>(Args.Value[0])->Value;
		std::string *temp = new std::string(s.str, s.len);
		try {
			Object *t = new Object_Int(std::stoll(*temp));
			delete temp;
			return t;
		} catch (...) {
			delete temp;
			throw "Cannot convert string to int";
		}
	}
	throw (char *)"Type mismatch";
}

static Object *Decimal(Object_Array &Args, Object_Env &Env) {
	// int
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		return new Object_Decimal(static_cast<Object_Int *>(Args.Value[0])->Value);
	}
	// decimal
	if (dynamic_cast<Object_Decimal *>(Args.Value[0]))
		return Args.Value[0]->clone();
	// string
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		Str s = static_cast<Object_String *>(Args.Value[0])->Value;
		std::string *temp = new std::string(s.str, s.len);
		try {
			Object *t = new Object_Decimal(std::stold(*temp));
			delete temp;
			return t;
		} catch (...) {
			delete temp;
			throw "Cannot convert string to decimal";
		}
	}
	throw (char *)"Type mismatch";
}

static Object *String(Object_Array &Args, Object_Env &Env) {
	// string
	if (dynamic_cast<Object_String *>(Args.Value[0]))
		return Args.Value[0]->clone();
	// int
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		std::string s = std::to_string(static_cast<Object_Int *>(Args.Value[0])->Value);
		return new Object_String(Str(s.c_str(), s.length()));
	}
	// decimal
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		std::ostringstream os;
		os << static_cast<Object_Decimal *>(Args.Value[0])->Value;
		std::string temp = os.str();
		return new Object_String(Str(temp.c_str(), temp.length()));
	}
	throw (char *)"Type mismatch";
}

static void Register_Type(Object &Shared) {
	Shared.AddChild("integer", new Object_Internal_Function(Integer));
	Shared.AddChild("decimal", new Object_Internal_Function(Decimal));
	Shared.AddChild("string", new Object_Internal_Function(String));
}
}; // namespace corelib
