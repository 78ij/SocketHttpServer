#include "http_message.h"
#include <iostream>

string& ltrim(string &str) {
	string::iterator p = std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(isspace)));
	str.erase(str.begin(), p);
	return str;
}

string& rtrim(string &str) {
	string::reverse_iterator p = find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(isspace)));
	str.erase(p.base(), str.end());
	return str;
}

string& trim(string &str) {
	ltrim(rtrim(str));
	return str;
}
int http_message::parse() {
	stringstream ss(message);
	ss >> method;
	ss >> URL;
	ss >> version;
	char tmp[1000];
	ss.get();
	ss.get();
	ss.getline(&tmp[0], 1000);
	std::pair<string, string> header;
	string key, value;
	while ((strcmp(tmp,"\r")) && (strcmp(tmp,""))) {
		key = strtok(tmp, ":");
		value = strtok(NULL, ":");
		trim(key);
		trim(value);
		header = make_pair(key, value);
		headers.insert(header);
		ss.getline(&tmp[0], 1000);
	}
	ss.readsome(content, numeric_limits<int>::max());
	return 0;
}

int http_message::build() {
	string str;
	str = version + " " + status + " " + statusdesc + "\r\n";
	for (auto header : headers) {
		str = str + header.first + ": " + header.second + "\r\n";
	}
	str = str + "\n";
	if(content)
		str = str + content;
	message = str;
	return 0;
}
