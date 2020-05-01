#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED

#include <variant>

template<typename V, typename E>
class Result
{
public:
	Result(const V &v) : data(v) {}
	Result(const E &e) : data(e) {}

	operator bool() const
	{
		return std::holds_alternative<V>(data);
	}

	V &operator *()
	{
		return std::get<V>(data);
	}

	const V &operator *() const
	{
		return std::get<V>(data);
	}

	const E &error() const
	{
		return std::get<E>(data);
	}

private:
	std::variant<V, E> data;
};

#endif
