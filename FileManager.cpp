#include "FileManager.h"
#include "Validator.h"
#include <cstdio>
#include <fstream>

bool FileManager::exists(const std::string& path) {
    std::ifstream in(path.c_str());
    return in.good();
}

std::vector<std::string> FileManager::readLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream in(path.c_str());
    if (!in) return lines;                                   // first run: no file yet -> no records
    std::string s;
    while (std::getline(in, s)) {
        s = Validator::trim(s);
        if (s.empty() || s[0] == '#') continue;
        lines.push_back(s);
    }
    return lines;
}

void FileManager::writeLines(const std::string& path, const std::string& header,
                             const std::vector<std::string>& lines) {
    std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp.c_str());
        if (!out) throw FileException("Cannot open '" + tmp + "' for writing (does the Data folder exist?).");
        out << "# " << header << "\n";
        for (size_t i = 0; i < lines.size(); ++i) out << lines[i] << "\n";
        out.flush();
        if (!out) throw FileException("Write error while saving '" + path + "'.");
    }
    std::remove(path.c_str());                               // required on Windows before rename
    if (std::rename(tmp.c_str(), path.c_str()) != 0)
        throw FileException("Could not replace '" + path + "'.");
}
