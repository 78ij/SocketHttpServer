#pragma once

#include<string>
#include<map>
#include<sstream>
#include<cstring>
#include<limits>
#include<functional>
#include<algorithm>

using std::string;
using std::map;
using std::stringstream;
using std::pair;
using std::strtok;
using std::numeric_limits;

class http_message {
public:
	http_message() {};
	http_message(string m) : message(m) { } // 5MB
	http_message(bool isreceive) { 
		content = (char*)malloc(1024 * 1024 * 5); 
		memset(content, 0, 1024 * 1024 * 5);
	};
	~http_message() { delete content; }
	string method;
	string URL;
	string status;
	string statusdesc;
	string version;
	map<string, string> headers;
	char *content;
	int parse();
	int build();
	string message;
};