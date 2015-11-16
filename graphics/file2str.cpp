/*
Copyright 2013, 2014 Kevin Robert Stravers

This file is part of TTL.

TTL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TTL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TTL.  If not, see <http://www.gnu.org/licenses/>.
*/


// Headers
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>


namespace ttl
{

	////////////////////////////////////////////////////////////
	std::string file2str(const std::string &filename)
	{
		std::fstream input_file(filename.data(), std::ios::in);
		if (!input_file.is_open())
			throw std::runtime_error("File can not be found");
		std::stringstream ss;
		ss << input_file.rdbuf();
		return ss.str();
		}

} // Namespace ttl
