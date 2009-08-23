#include <list>
#include <string>
#include <map>

namespace kkc {
    
    class Char {
	unsigned int c;
    };
    class String : std::basic_string<Char> {
    public:
	String() : std::basic_string<Char>() {};
	String(const Char* p, size_t len) : std::basic_string<Char>(p, len) {};
	static String from_utf8(std::string);
    };

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

    struct Word {
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
	std::map<String, Word> map;
    };

    class Context {
    public:	
	Context();
    private:
	Dict dict;
    };

    class Matrix {
    };

    class Session {
    public:
	Session(String reading_) : reading(reading_) {};
    private:
	const String reading;
    };

}

