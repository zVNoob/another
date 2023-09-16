#pragma once

#include "object.hpp"

#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace corelib {

static Object *Read(Object_Array &Args, Object_Env &Env);
static Object *Write(Object_Array &Args, Object_Env &Env);
static Object *Seek(Object_Array &Args, Object_Env &Env);

class Object_IO : public Object {
  public:
#define _IO_HANDLE int
	_IO_HANDLE handle;
	bool Closable;
	Object_IO() {
		AddChild("read", new Object_Internal_Function(Read));
		AddChild("write", new Object_Internal_Function(Write));
		AddChild("seek", new Object_Internal_Function(Seek));
	}
};
#define _IO_MAX_BUFFER 8192
static Object *Read(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0) {
		std::string Result;
		char *Buffer = new char[_IO_MAX_BUFFER];
		int len;
		_IO_HANDLE handle = static_cast<Object_IO *>(Env.This)->handle;
		while ((len = ::read(handle, Buffer, _IO_MAX_BUFFER)) != 0) {
			Result += std::string(Buffer, len);
		}
		delete[] Buffer;
		return new Object_String(Str(Result.c_str(), Result.size()));
	}
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		long long len = static_cast<Object_Int *>(Args.Value[0])->Value;
		char *Buffer = new char[len];
		_IO_HANDLE handle = static_cast<Object_IO *>(Env.This)->handle;
		len = ::read(handle, Buffer, len);
		Object_String *Result = new Object_String(Str(Buffer, len));
		delete[] Buffer;
		return Result;
	}
	throw "Type mismatch";
}
static Object *Write(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0)
		throw "Missing argument";
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		Object_String *Inp = static_cast<Object_String *>(Args.Value[0]);
		_IO_HANDLE handle = static_cast<Object_IO *>(Env.This)->handle;
		::write(handle, Inp->Value.str, Inp->Value.len);
		return 0;
	}
	throw "Type mismatch";
}

static Object *Seek(Object_Array &Args, Object_Env &Env) {
	_IO_HANDLE handle = static_cast<Object_IO *>(Env.This)->handle;
	if (Args.Value.size() == 0) {
		long long res = ::lseek64(handle, 0, SEEK_CUR);
		return new Object_Int(res);
	}
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		long long Inp = static_cast<Object_Int *>(Args.Value[0])->Value;
		if (Inp < 0) {
			long long res = ::lseek64(handle, 0, SEEK_END);
			return new Object_Int(res);
		}
		long long res = ::lseek64(handle, Inp, SEEK_CUR);
		return new Object_Int(res);
	}
	throw "Type mismatch";
}

static Object *Open(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0)
		throw "Missing argument";
	if (dynamic_cast<Object_Int *>(Args.Value[0])) {
		Object_IO *Result = new Object_IO();
		Result->handle = static_cast<Object_Int *>(Args.Value[0])->Value;
		Result->Closable = false;
		return Result;
	}
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		Str s = static_cast<Object_String *>(Args.Value[0])->Value;
		std::string s2(s.str, s.len);
		_IO_HANDLE handle = ::open(s2.c_str(), O_RDWR);
		Object_IO *Result = new Object_IO();
		Result->handle = handle;
		Result->Closable = true;
		return Result;
	}
	throw "Type mismatch";
}

static Object *Creat(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0)
		throw "Missing argument";
	int mode = 0666;
	if (dynamic_cast<Object_String *>(Args.Value[0])) {
		Str s = static_cast<Object_String *>(Args.Value[0])->Value;
		std::string temp(s.str, s.len);
		_IO_HANDLE handle = ::creat(temp.c_str(), mode);
		Object_IO *Result = new Object_IO();
		Result->handle = handle;
		Result->Closable = true;
		return Result;
	}
	throw "Type mismatch";
}

inline void Register_IO(Object &Shared) {
	Shared.AddChild("open", new Object_Internal_Function(Open));
}
} // namespace corelib
