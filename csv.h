#ifndef CSV_H_INCLUDED
#define CSV_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

class CsvExporter
{
public:
	template<typename T>
	void setCell(int x, int y, const T &value)
	{
		if (y >= data.size())
			data.resize(y + 1);
		auto &row = data[y];
		if (x >= row.size())
			row.resize(x + 1);

		std::ostringstream o;
		o << value;
		std::string s = o.str();
		row[x] = s;
	}

	bool write(std::ostream &out)
	{
		for (int y = 0; y < data.size(); ++y) {
			const auto &row = data[y];
			for (int x = 0; x < row.size(); ++x) {
				out << row[x];
				if (x < row.size() - 1)
					out << ',';
			}
			out << std::endl;
		}

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
	std::vector<std::vector<std::string>> data;
};

#endif
