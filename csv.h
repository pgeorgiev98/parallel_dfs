#ifndef CSV_H_INCLUDED
#define CSV_H_INCLUDED

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class CsvExporter
{
public:
	struct Modifier
	{
		enum {
			NewLine,
		} type;
	};
	static constexpr Modifier endl{Modifier::NewLine};

	template<typename T>
	CsvExporter &operator<<(const T &value)
	{
		std::ostringstream o;
		o << value;
		if (!data.empty() && data[data.size() - 1] != '\n')
			data += ',';
		data += o.str();
		return *this;
	}

	template<typename T = Modifier>
	CsvExporter &operator<<(const Modifier &value)
	{
		switch (value.type) {
			case Modifier::NewLine:
				data += '\n';
				break;
			default: {}
		}
		return *this;
	}

	bool write(std::ostream &out)
	{
		out << data;
		if (!data.empty() && data[data.size() - 1] != '\n')
			out << '\n';

		return bool(out);
	}

	bool write(const std::string &file)
	{
		std::ofstream out(file, std::ofstream::out);
		if (!out)
			return false;

		return write(out);
	}

private:
	std::string data;
};

#endif
