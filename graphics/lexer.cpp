#include <string>
#include <vector>

void lex(const std::string source,
	std::vector<std::pair<float, float>> &vector)
{
	std::string lexeme;
	bool secondLine = false;
	std::size_t i = 0;
	for (char ch : source) {
		if (ch != '\n' && ch != ',') {
			lexeme.push_back(ch);
		} else {
			if (ch != '\n') {
				if (secondLine) {
					vector.at(i).second = std::stof(lexeme);
					++i;
				} else {
					vector.push_back(std::make_pair(std::stof(lexeme), 0));
				}
			} else {
				if (secondLine == false)
					vector.push_back(std::make_pair(std::stof(lexeme), 0));
				secondLine = true;
			}
			vector.at(i).second = std::stof(lexeme);
			lexeme = "";
		}
	}
}
