#include <list>
#include <string>
#include <map>
#include <vector>

namespace kkc {

    class Char {
    public:
	bool operator< (const Char& x) const { return c < x.c; }
    private:
	unsigned int c;
    };

    class String : public std::basic_string<Char> {
    public:
	String() : std::basic_string<Char>() {};
	String(std::wstring w) : std::basic_string<Char>((Char*) w.c_str(), w.length()) {};
	String(const Char* p, size_t len) : std::basic_string<Char>(p, len) {};
	String(std::basic_string<Char> b) : std::basic_string<Char>(b) {};
	static String from_utf8(std::string);
	operator std::wstring() const { return std::wstring((wchar_t*) c_str(), length());};
    };

    class SubString {
    public:
	SubString(const Char* begin, const Char* end) : begin_(begin), end_(end) {};
	SubString(const Char* begin, const size_t length) : begin_(begin), end_(begin + length) {};
	const Char* begin() const { return begin_; }
	const Char* end() const { return end_; }
	size_t length() const { return end_ - begin_; }
    private:
	const Char* begin_;
	const Char* end_;
    };

    typedef unsigned int connect_id_t;

    class Word {
    public:
	Word() {}
	Word(const Word& x) : key(x.key), left_id(x.left_id), right_id(x.right_id), cost(x.cost), value(x.value) {}
	Word(String key_, connect_id_t left_id_, connect_id_t right_id_, int cost_, String value_)
	    : key(key_), left_id(left_id_), right_id(right_id_), cost(cost_), value(value_) {}
	String key;
	connect_id_t left_id;
	connect_id_t right_id;
	int cost;
	String value;
    };

    class Dict {
    public:
	void load(const char* filename);
	std::list<Word> prefix_lookup(const SubString&) const;
	std::list<Word> lookup(const SubString&) const;
    private:
	std::map<String, std::list<Word> > map;
    };

    class Context {
    public:	
	Context();
	Dict& dict() { return dict_; };
    private:
	Dict dict_;
    };

    class Lattice {
    public:
	Lattice(size_t size);
    };

    class Matrix {
    };

    class Session {
    public:
	Session(Context& ctx, String reading_);
	String sentence() const;
    private:
	void lookup();
	void search();
	Context& ctx;
	const String reading;
	size_t length;
	std::vector<std::vector<std::list<Word> > > lattice;
	std::vector<int> cost;
	std::vector<int> path;
    };

}

