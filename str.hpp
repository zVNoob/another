#pragma once

#include <cstring>
struct Str {
	char *str;
	long len;
	Str(char *str, unsigned long len) : str(str), len(len) {}
	Str(const char *str, unsigned long len) : str(const_cast<char *>(str)), len(len) {}
	Str(const char *str) : str(const_cast<char *>(str)), len(strlen(str)) {}
	void operator<<(int i) {
		len -= i;
		if (len < 1)
			len = 0;
	}
	void operator>>(int i) {
		len -= i;
		str += i;
		if (len < 1)
			len = 0;
	}
	Str clone() {
		char *new_str = new char[len];
		memcpy(new_str, str, len);
		return Str(new_str, len);
	}
	struct STD_Str_LESS {
		bool operator()(const Str &a, const Str &b) const {
			int c = (a.len < b.len) ? a.len : b.len;
			for (int i = 0; i < c; i++) {
				if (a.str[i] != b.str[i])
					return a.str[i] < b.str[i];
			}
			return a.len < b.len;
		}
	};
};
