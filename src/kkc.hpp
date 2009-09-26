#include <list>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace kkc {

    typedef wchar_t Char;
    typedef std::basic_string<Char> String;

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

    template<class T> class Matrix {
    public:
        typedef size_t size_type;
        Matrix(size_type n_row_, size_type n_col_);
        T& at(size_type row, size_type col);
        size_type n_row, n_col;
    private:
        std::vector<T> matrix;
    };

    class Connect {
    public:
	void load(const char* filename);
	int at(connect_id_t right, connect_id_t left);
        int n_right_id();
        int n_left_id();
    private:
        std::auto_ptr<Matrix<int> > matrix;
    };

    class Context {
    public:	
	Context();
	Dict& dict() { return dict_; };
	Connect& connect() { return connect_; };
    private:
	Dict dict_;
	Connect connect_;
    };

    class Lattice {
    public:
	Lattice(size_t size);
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

        String sentence_;
    };

}

