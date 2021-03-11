#include "../includes/modules/KittenPreprocessor.h"

#include <ratio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <regex>

namespace Kitten {
	void printWithLineNumber(string str) {
		int nLine = 0;
		const char* format = "%-4d| ";
		printf(format, nLine++);
		for (size_t i = 0; i < str.length(); i++) {
			char c = str[i];
			cout << c;
			if (c == '\n') printf(format, ++nLine);
		}
		cout << endl;
	}

	bool endsWith(string const& str, string const& ending) {
		if (str.length() >= ending.length())
			return (0 == str.compare(str.length() - ending.length(), ending.length(), ending));
		return false;
	}

	string loadText(string name) {
		ifstream f(name);
		stringstream buff;
		buff << f.rdbuf() << '\0';
		f.close();
		return buff.str();
	}

	string loadTextWithIncludes(string path, set<string>& includedSet) {
		if (includedSet.count(path))
			return "";
		includedSet.insert(path);

		string src = loadText(path);
		regex pattern("^#include +[<|\"][^\\n\">]+[>|\"] *$");

		stringstream buff;
		auto lastItr = src.begin();

		std::sregex_iterator iter(src.begin(), src.end(), pattern);
		std::sregex_iterator end;
		while (iter != end) {
			auto s = src.begin() + iter->position(0);
			auto e = s + iter->length(0) - 1;

			buff << string(lastItr, s).c_str();
			lastItr = e + 1;

			while (*s != '\"' && *s != '<' && s < e) s++;
			s++;
			while (*e != '\"' && *e != '>' && e > s) e--;

			string p(s, e);
			//cout << "including: " << p.c_str() << endl;
			buff << loadTextWithIncludes(p, includedSet).c_str() << '\n';
			++iter;
		}

		buff << string(lastItr, src.end()).c_str();

		return buff.str();
	}

	void parseAssetTag(string& ori, string& name, Tags& tags) {
		char tagBuff[32];
		char buff[256];
		int tagI = 0, buffI = 0;
		int state = 0;
		int dataI = 0;
		int data[4]{ 0, 0, 0, 0 };

		size_t lim = ori.length();
		while (ori[lim] != '.' && lim > 0) lim--;

		for (size_t i = 0; i < lim; ++i) {
			char c = ori[i];
			switch (state) {
			case 0://Reading name
				if (c == '#')
					state = 1;
				else
					buff[buffI++] = c;
				break;
			case 1://Reading tag name
				if (c == '=')
					state = 2;
				else if (isalpha(c))
					tagBuff[tagI++] = c;
				else {
					tagBuff[tagI] = '\0';
					tagI = 0;
					tags[string(tagBuff)] = ivec4{ 1, 0, 0, 0 };
					state = 0;
					--i;
				}
				break;
			case 2://Reading tag data
				if (c <= '9' && c >= '0')
					data[dataI] = data[dataI] * 10 + c - '0';
				else if (c == ',')
					dataI++;
				else {
					tagBuff[tagI] = '\0';
					tagI = 0;
					if (data[dataI])
						dataI++;
					if (dataI == 0)
						tags[string(tagBuff)] = ivec4{ 1, 0, 0, 0 };
					else if (dataI == 1)
						tags[string(tagBuff)] = ivec4{ data[0], 0, 0, 0 };
					else if (dataI == 2)
						tags[string(tagBuff)] = ivec4{ data[0], data[1], 0, 0 };
					else if (dataI == 3)
						tags[string(tagBuff)] = ivec4{ data[0], data[1], data[2], 0 };
					else if (dataI == 4)
						tags[string(tagBuff)] = ivec4{ data[0], data[1], data[2], data[3] };
					data[0] = data[1] = data[2] = data[3] = 0;
					state = 0;
					--i;
				}
				break;
			}
		}
		buff[buffI] = '\0';
		name = string(buff) + ".tex";
	}
}