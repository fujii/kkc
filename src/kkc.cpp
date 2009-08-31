#include "kkc.hpp"
#include "encode.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

namespace kkc {

String from_utf8(std::string utf8)
{
    Char* buf;
    size_t len;
    int err;
    err = convert_utf8_to_ucs4(utf8.c_str(), reinterpret_cast<char**>(&buf), &len);
    if (err)
	return String();
    String result(buf, len/4);
    free(buf);
    return result;
}

void Dict::load(const char* filename)
{
    std::ifstream f;
    f.open(filename);
    std::string line;
    while (std::getline(f, line)) {
	std::istringstream iss(line);

	std::string col[5];
	for (int i=0; i<5; i++)
	    std::getline(iss, col[i], ',');

	String key = from_utf8(col[0]);
	connect_id_t left_id = atoi(col[1].c_str());
	connect_id_t right_id = atoi(col[2].c_str());
	int cost = atoi(col[3].c_str());
	String value = from_utf8(col[4]);

	Word word(key, left_id, right_id, cost, value);
	map[key].push_back(word);
    }
}

std::list<Word> Dict::lookup(const SubString& ss) const
{
    String key(ss.begin(), ss.length());
    std::map<String, std::list<Word> >::const_iterator iter = map.find(key);
    if (iter == map.end())
	return std::list<Word>();
    else
	return iter->second;
}

Context::Context()
{
    dict_.load("dic.csv");
}

Lattice::Lattice(size_t size)
{
}

Session::Session(Context& ctx_, String reading_)
    : ctx(ctx_)
    , reading(reading_)
    , length(reading_.length())
    , lattice(length, std::vector<std::list<Word> >(length))
    , cost(length)
    , path(length)
{
    lookup();
    search();
}

void Session::lookup()
{
    const Char *r = reading.c_str();
    for (int s=0; s<length; s++) {
	for (int e=s; e<length; e++) {
	    SubString ss(r+s, r+e+1);
	    lattice[s][e] = ctx.dict().lookup(ss);
	}
    }
}

void Session::search()
{
    for (int i=0; i<length; i++) {
	int min_cost, min_j;
	min_j = i;
	min_cost = i == 0 ? 1 : cost[i-1] + 1;
	for (int j=0; j<i; j++) {
	    std::list<Word> words = lattice[j][i];
	    if (!words.empty()) {
		int c = j == 0 ? 1 : cost[j-1] + 1;
		if (min_cost > c) {
		    min_j = j;
		    min_cost = c;
		}
	    }
	}
	cost[i] = min_cost;
	path[i] = min_j;
    }
}

String Session::sentence() const
{
    String s;
    int j;
    for (int i = length - 1; i>=0; i=j-1) {
	j = path[i];
	std::list<Word> words = lattice[j][i];
	if (words.empty()) {
	    s = reading.substr(j, i-j+1) + s;
	} else {
	    s = words.front().value + s;
	}
    }
    return s;
}

}

int main()
{
    std::locale::global(std::locale(""));
    kkc::Context ctx;
    kkc::String input;
    std::wcin >> input;
    kkc::Session session(ctx, input);
    std::wcout << session.sentence() << std::endl;
    return 0;
}
