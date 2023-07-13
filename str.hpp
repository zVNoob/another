#pragma once

struct Str {
	char* str;
	int len;
	void operator<<(int i) {
		len -= i;
		if (len<1) len = 0;
	}
	void operator>>(int i) {
		len -= i;
		str += i;
		if (len<1) len = 0;
	}
	struct STD_Str_LESS {
		bool operator()(Str a,Str b) {
			int c = (a.len < b.len)? a.len : b.len;
			for (int i = 1;i < c;i++) {
				if (a.str[i] != b.str[i])
					return a.str[i] < b.str[i];
			}
			return a.len < b.len;
		}
	};
};
