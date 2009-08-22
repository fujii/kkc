#include <list>

namespace kkc {
    
    class Char {
	int c;
    };
    class String {};

    class SubString {
    public:
	SubString(const Char* begin, const Char* end) : begin_(begin), end_(end) {};
	SubString(const Char* begin, const size_t length) : begin_(begin), end_(begin + length) {};
	const Char* begin() const { return begin_; }
	const Char* end() const { return end_; }
	size_t lenght() const { return end_ - begin_; }
    private:
	const Char* begin_;
	const Char* end_;
    };

    typedef unsigned int connect_id_t;

    class Word {
    public:
	String reading;
	connect_id_t left_id;
	connect_id_t right_id;
	int cost;
	String string;
    };

    class Dict {
	std::list<Word> prefix_lookup(const SubString&) const;
	std::list<Word> lookup(const SubString&) const;
    };

    class Context {
	Dict dict;
    };

    class Session {
    public:
	Session(String reading_) : reading(reading_) {};
    private:
	const String reading;
    };

}

