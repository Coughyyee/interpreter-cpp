module;
#include <string>

export module SourceUtils;

/// <summary>
///	Return the whole line from source code based on line postfix. Utilised in error handling + debugging. 
/// </summary>
/// <param name="source">Source code file</param>
/// <param name="line">Line postfix</param>
/// <returns>Whole line from offset</returns>
export std::string get_line_from_source(const std::string& source, size_t line) {
	size_t current_line = 1;
	size_t start = 0;

	for (size_t i = 0; i < source.size(); ++i) {
		if (current_line == line) {
			start = i;
			break;
		}

		if (source[i] == '\n') {
			++current_line;
		}
	}

	size_t end = start;
	while (end < source.size() && source[end] != '\n') {
		++end;
	}

	return source.substr(start, end - start);
}
