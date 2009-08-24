#include "kkc.hpp"
#include "encode.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

namespace kkc {

String String::from_utf8(std::string utf8)
{
    Char* buf;
    size_t len;
    int err;
    err = convert_utf8_to_ucs4(utf8.c_str(), reinterpret_cast<char**>(&buf), &len);
    if (err)
	return String();
    String result(buf, len/4);
    return result;
}

void Dict::load(const char* filename)
{
    std::ifstream f;
    f.open(filename);
    std::string line;
    while (std::getline(f, line)) {
	std::istringstream iss(line);

	std::string col[4];
	for (int i=0; i<4; i++)
	    std::getline(iss, col[i], ',');

	String key = String::from_utf8(col[0]);
	connect_id_t left_id = atoi(col[1].c_str());
	connect_id_t right_id = atoi(col[2].c_str());
	int cost = atoi(col[3].c_str());
	String value = String::from_utf8(col[4]);

	Word word = {key, left_id, right_id, cost, value};
	map[key] = word;
    }
}

Context::Context()
{
    dict.load("dic.csv");
}


}

int main()
{
    kkc::Context cxt;
    std::cout << "Hello world!" << std::endl;
    return 0;
}
