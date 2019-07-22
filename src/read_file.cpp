#include <tsl/read_file.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

using std::ifstream;
using std::runtime_error;
using std::string;
using std::istreambuf_iterator;

namespace tsl {

string read_file(const string& filename) {
    ifstream ifs(filename);
    if (!ifs.good()) {
        throw runtime_error("Could not open file: " + filename);
    }

    string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    return content;
}

}
