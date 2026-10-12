// FileManager.h - all file input/output lives here (persistence layer).
// Files are plain text, one record per line, fields separated by '|'.
// Lines starting with '#' and blank lines are ignored when loading.
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "DynArray.h"
#include "Exceptions.h"
#include "Record.h"
#include <iostream>
#include <string>
#include <vector>

class FileManager {
public:
    static bool exists(const std::string& path);
    static std::vector<std::string> readLines(const std::string& path);     // empty if file is missing
    static void writeLines(const std::string& path, const std::string& header,
                           const std::vector<std::string>& lines);          // safe write (temp file + rename)

    // Writes every record of the array; returns number of records saved.
    template <class T>
    static int save(const std::string& path, const std::string& header, const DynArray<T>& records) {
        std::vector<std::string> lines;
        for (int i = 0; i < records.size(); ++i) {
            const Record& r = records.at(i);                // polymorphic call through the base class
            lines.push_back(r.toFileLine());
        }
        writeLines(path, header, lines);
        return records.size();
    }

    // Replaces the array contents with the file's records; returns number of corrupt lines skipped.
    template <class T>
    static int load(const std::string& path, DynArray<T>& records) {
        records.clear();
        int skipped = 0;
        std::vector<std::string> lines = readLines(path);
        for (size_t i = 0; i < lines.size(); ++i) {
            try {
                T item = T::fromFileLine(lines[i]);
                if (records.indexOfId(item.getId()) >= 0) throw FileException("duplicate ID " + std::to_string(item.getId()));
                records.add(item);
            } catch (const std::exception& e) {
                ++skipped;
                std::cerr << "  [warning] " << path << ": skipped bad record '" << lines[i] << "' (" << e.what() << ")\n";
            }
        }
        return skipped;
    }
};

#endif
