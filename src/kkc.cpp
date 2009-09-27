#include "kkc.hpp"
#include "encode.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <assert.h>

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
    std::list<Word> words;
    if (iter != map.end())
	words = iter->second;
    Word unknown(key, 10, 10, 8000 * key.length(), key); // TODO: Read unk.def
    words.push_back(unknown);
    return words;
}

template<class T>
Matrix<T>::Matrix(Matrix::size_type n_row_, Matrix::size_type n_col_)
    : n_row(n_row_)
    , n_col(n_col_)
{
    matrix.resize(n_row * n_col);
}

template<class T>
T& Matrix<T>::at(Matrix::size_type row, Matrix::size_type col)
{
    return matrix.at(row * n_col + col);
}

void Connect::load(const char* filename)
{
    std::ifstream f;
    f.open(filename);
    size_t n_right_id, n_left_id;
    f >> n_right_id >> n_left_id;
    std::auto_ptr<Matrix<int> > t(new Matrix<int>(n_right_id, n_left_id));
    matrix = t;
    size_t col, row;
    int cost;
    while (f >> row >> col >> cost) {
	matrix->at(row, col) = cost;
    }
}

int Connect::at(connect_id_t right, connect_id_t left)
{
    return matrix->at(right, left);
}

int Connect::n_right_id()
{
    return matrix->n_row;
}

int Connect::n_left_id()
{
    return matrix->n_col;
}

Context::Context()
{
    dict_.load("dic.csv");
    connect_.load("matrix.def");
}

Lattice::Lattice(size_t size)
{
}

Session::Session(Context& ctx_, String reading_)
    : ctx(ctx_)
    , reading(reading_)
    , length(reading_.length())
    , lattice(length, std::vector<std::list<Word> >(length))
{
    lookup();
    search();
}

void Session::lookup()
{
    const Char *r = reading.c_str();
    for (size_t s=0; s<length; s++) {
	for (size_t e=s; e<length; e++) {
	    SubString ss(r+s, r+e+1);
	    lattice[s][e] = ctx.dict().lookup(ss);
	}
    }
}

struct Path {
    Path* prev;
    Word word;
};

void Session::search()
{
    size_t n_right_id = ctx.connect().n_right_id();
    Matrix<std::pair<int, Path> > cost_path(length, n_right_id);
    for (size_t e=0; e<length; e++) {
	for (size_t s=0; s<=e; s++) {
	    std::list<Word> words = lattice[s][e];
            assert(!words.empty());
            for (std::list<Word>::iterator iter = words.begin(); iter != words.end(); iter++) {
                Word& word = *iter;
                if (s == 0) {
                    int cost2 = ctx.connect().at(0, word.left_id);
                    int cost3 = word.cost;
                    int cost = cost2 + cost3;
                    if (cost_path.at(e, word.right_id).first == 0
                        || cost_path.at(e, word.right_id).first > cost) {
                        cost_path.at(e, word.right_id).first = cost;
                        cost_path.at(e, word.right_id).second.prev = 0;
                        cost_path.at(e, word.right_id).second.word = word;
                    }
                } else {
                    for (size_t r=0; r<n_right_id; r++) {
                        int cost1 = cost_path.at(s-1, r).first;
                        if (!cost1)
                            continue;
                        int cost2 = ctx.connect().at(r, word.left_id);
                        int cost3 = word.cost;
                        int cost = cost1 + cost2 + cost3;
                        if (cost_path.at(e, word.right_id).first == 0
                            || cost_path.at(e, word.right_id).first > cost) {
                            cost_path.at(e, word.right_id).first = cost;
                            cost_path.at(e, word.right_id).second.prev = &cost_path.at(s-1, r).second;
                            cost_path.at(e, word.right_id).second.word = word;
                        }
                    }
                }
            }
        }
    }
    int min_cost = 0;
    Path min_path;
    for (size_t r=0; r<n_right_id; r++) {
        size_t s = length;
        int cost1 = cost_path.at(s-1, r).first;
        if (!cost1)
            continue;
        int cost2 = ctx.connect().at(r, 0);
        int cost = cost1 + cost2;
        if (min_cost == 0
            || min_cost > cost) {
            min_cost = cost;
            min_path.prev = &cost_path.at(s-1, r).second;
            min_path.word = Word();
        }
    }

    assert(min_cost != 0);
    {
        String s;
        Path* p = &min_path;
        while (p) {
            s = p->word.value + s;
            p = p->prev;
        }
        sentence_ = s;
    }
}

String Session::sentence() const
{
    return sentence_;
}

}

int main()
{
    std::locale::global(std::locale(""));
    kkc::Context ctx;
    kkc::String input;
    while (std::getline(std::wcin, input)) {
	kkc::Session session(ctx, input);
	std::wcout << session.sentence() << std::endl;
    }
    return 0;
}
