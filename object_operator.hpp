#pragma once

#include "object.hpp"
#include "object_function.hpp"

static Object *Internal_Plus(Object_Array &Args, Object_Env &Env) { // +
	if (Args.Value.size() == 1) {
		// int,decimal -> clone
		if (dynamic_cast<Object_Int *>(Args.Value[0])) {
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value);
		}
		if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value);
		}
		// array,string -> length
		if (dynamic_cast<Object_Array *>(Args.Value[0])) {
			return new Object_Int(
				static_cast<Object_Array *>(Args.Value[0])->Value.size());
		}
		if (dynamic_cast<Object_String *>(Args.Value[0])) {
			return new Object_Int(
				static_cast<Object_String *>(Args.Value[0])->Value.len);
		}
		throw (char *)"Type mismatch";
	}
	// int + (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int + int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value +
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int + decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Int *>(Args.Value[0])->Value +
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal + (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal + int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value +
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal + decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value +
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// array + array
	if (dynamic_cast<Object_Array *>(Args.Value[0])) {
		if (dynamic_cast<Object_Array *>(Args.Value[1])) {
			Object_Array *Result = new Object_Array();
			for (auto &i : static_cast<Object_Array *>(Args.Value[0])->Value)
				Result->Value.push_back(i->clone());
			for (auto &i : static_cast<Object_Array *>(Args.Value[1])->Value)
				Result->Value.push_back(i->clone());
			return Result;
		}
		throw (char *)"Type mismatch";
	}
	// string + string
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		if (dynamic_cast<Object_String *>(Args.Value[1])) {
			char *result = new char[static_cast<Object_String *>(Args.Value[0])->Value.len + static_cast<Object_String *>(Args.Value[1])->Value.len];
			for (int i = 0; i < static_cast<Object_String *>(Args.Value[0])->Value.len; i++)
				result[i] = static_cast<Object_String *>(Args.Value[0])->Value.str[i];
			for (int i = 0; i < static_cast<Object_String *>(Args.Value[1])->Value.len; i++)
				result[i + static_cast<Object_String *>(Args.Value[0])->Value.len] = static_cast<Object_String *>(Args.Value[1])->Value.str[i];
			Object *Result = new Object_String(result);
			delete[] result;
			return Result;
		}
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Minus(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1) {
		// int -> reverse
		if (dynamic_cast<Object_Int *>(Args.Value[0]))
			return new Object_Int(-static_cast<Object_Int *>(Args.Value[0])->Value);
		// decimal -> reverse
		if (dynamic_cast<Object_Decimal *>(Args.Value[0]))
			return new Object_Decimal(-static_cast<Object_Decimal *>(Args.Value[0])->Value);
		throw (char *)"Type mismatch";
	}
	// int - (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int - int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value -
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int - decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Int *>(Args.Value[0])->Value -
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal - (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal - int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value -
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal - decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value -
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// array - int #why not?
	if (dynamic_cast<Object_Array *>(Args.Value[0])) {
		if (dynamic_cast<Object_Int *>(Args.Value[1])) {
			Object_Array *Result = new Object_Array();
			int dr = static_cast<Object_Int *>(Args.Value[1])->Value;
			for (auto &i : static_cast<Object_Array *>(Args.Value[0])->Value) {
				if (dr)
					Result->Value.push_back(i->clone());
				dr--;
			}
			return Result;
		}
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Multiply(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int * (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int * int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value *
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int * decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Int *>(Args.Value[0])->Value *
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal * (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal * int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value *
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal * decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value *
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Divide(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int / (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int / int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Decimal(
				(long double)static_cast<Object_Int *>(Args.Value[0])->Value /
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int / decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Int *>(Args.Value[0])->Value /
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal / (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal / int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value /
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal / decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Decimal(
				static_cast<Object_Decimal *>(Args.Value[0])->Value /
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Modulo(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int % int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value %
				static_cast<Object_Int *>(Args.Value[1])->Value);
	// string % string (split string based on characters)
	if (dynamic_cast<Object_String *>(Args.Value[0]))
		if (dynamic_cast<Object_String *>(Args.Value[1])) {
			Object_Array *Result = new Object_Array();
			Object_String *Source = static_cast<Object_String *>(Args.Value[0]);
			int source_start = 0;
			Object_String *Filter = static_cast<Object_String *>(Args.Value[1]);
			for (int i = 0; i < Source->Value.len; i++) {
				bool found = false;
				for (int j = 0; j < Filter->Value.len; j++)
					if (Source->Value.str[i] == Filter->Value.str[j]) {
						found = true;
						break;
					}
				if (found) {
					if (source_start != i)
						Result->Value.push_back(new Object_String(Str(Source->Value.str + source_start, i - source_start)));
					source_start = i + 1;
				}
			}
			if (source_start != Source->Value.len)
				Result->Value.push_back(new Object_String(Str(Source->Value.str + source_start, Source->Value.len - source_start)));
			return Result;
		}
	throw (char *)"Type mismatch";
}

static Object *Intenal_And(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int & int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value &
				static_cast<Object_Int *>(Args.Value[1])->Value);
	// string & string (find position of first character)
	if (dynamic_cast<Object_String *>(Args.Value[0]))
		if (dynamic_cast<Object_String *>(Args.Value[1])) {
			Object_String *Source = static_cast<Object_String *>(Args.Value[0]);
			Object_String *Substr = static_cast<Object_String *>(Args.Value[1]);
			std::string SourceStr(Source->Value.str, Source->Value.len);
			std::string SubstrStr(Substr->Value.str, Substr->Value.len);
			// TODO: Is std::string use the fastest algorithm?
			int pos = SourceStr.find(SubstrStr);
			if (pos == std::string::npos)
				pos = -1;
			return new Object_Int(pos);
		}
	throw (char *)"Type mismatch";
}

static Object *Intenal_Or(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int & int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value |
				static_cast<Object_Int *>(Args.Value[1])->Value);
	throw (char *)"Type mismatch";
}

static Object *Intenal_XOr(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int & int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value ^
				static_cast<Object_Int *>(Args.Value[1])->Value);
	throw (char *)"Type mismatch";
}

static Object *Intenal_Not(Object_Array &Args, Object_Env &Env) {
	// int
	if (dynamic_cast<Object_Int *>(Args.Value[0]))
		return new Object_Int(!static_cast<Object_Int *>(Args.Value[0])->Value);
	// array (return length)
	if (dynamic_cast<Object_Array *>(Args.Value[0]))
		return new Object_Int(static_cast<Object_Array *>(Args.Value[0])->Value.size());
	// string (return length)
	if (dynamic_cast<Object_String *>(Args.Value[0]))
		return new Object_Int(static_cast<Object_String *>(Args.Value[0])->Value.len);
	throw (char *)"Type mismatch";
}

static Object *Internal_Less(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int < (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int < int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value <
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int < decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value <
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal < (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal < int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value <
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal < decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value <
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Greater(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int > (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int > int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value >
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int > decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value >
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal > (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal > int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value >
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal > decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value >
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static Object *Internal_Equal(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw (char *)"Missing operand";
	// int == (int,decimal)
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		// int == int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value ==
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// int == decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Int *>(Args.Value[0])->Value ==
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	// decimal == (int,decimal)
	if (dynamic_cast<Object_Decimal *>(Args.Value[0])) {
		// decimal == int
		if (dynamic_cast<Object_Int *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value ==
				static_cast<Object_Int *>(Args.Value[1])->Value);
		// decimal == decimal
		if (dynamic_cast<Object_Decimal *>(Args.Value[1]))
			return new Object_Int(
				static_cast<Object_Decimal *>(Args.Value[0])->Value ==
				static_cast<Object_Decimal *>(Args.Value[1])->Value);
		throw (char *)"Type mismatch";
	}
	throw (char *)"Type mismatch";
}

static void Register_Operators(Object &Shared) {
	Shared.AddChild(Str("+"), new Object_Internal_Function(Internal_Plus));
	Shared.AddChild(Str("-"), new Object_Internal_Function(Internal_Minus));
	Shared.AddChild(Str("*"), new Object_Internal_Function(Internal_Multiply));
	Shared.AddChild(Str("/"), new Object_Internal_Function(Internal_Divide));
	Shared.AddChild(Str("%"), new Object_Internal_Function(Internal_Modulo));
	Shared.AddChild(Str("&"), new Object_Internal_Function(Intenal_And));
	Shared.AddChild(Str("|"), new Object_Internal_Function(Intenal_Or));
	Shared.AddChild(Str("^"), new Object_Internal_Function(Intenal_XOr));
	Shared.AddChild(Str("!"), new Object_Internal_Function(Intenal_Not));
	Shared.AddChild(Str("<"), new Object_Internal_Function(Internal_Less));
	Shared.AddChild(Str(">"), new Object_Internal_Function(Internal_Greater));
	Shared.AddChild(Str("~"), new Object_Internal_Function(Internal_Equal));
}
