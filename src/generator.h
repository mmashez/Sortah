#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

class JSXGenerator {
public:
    void generateJSX(const std::string& path,
                     const std::vector<std::string>& videoPaths,
                     const std::string& audioPath) {
        std::ofstream file(path);
        if (!file.is_open()) return;

        file << "// AUTO-GENERATED CODE BY EDIT ORGANIZER @MMASHEZ. DO NOT EDIT.\n\n";

        file << "var scriptFile = new File($.fileName);\n";

        file << "app.beginUndoGroup(\"Auto Import\");\n\n";

        file << "var importOptions;\n";
        // videos
        for (const auto& video : videoPaths) {
            file << "importOptions = new ImportOptions(File(\"" << escapePath(video) << "\"));\n";
            file << "app.project.importFile(importOptions);\n";
        }

        // audio(s)
        file << "\nimportOptions = new ImportOptions(File(\"" << escapePath(audioPath) << "\"));\n";
        file << "app.project.importFile(importOptions);\n";

        file << "app.endUndoGroup();\n\n";

        // alert
        file << "alert('have fun!! @mmashez');\n";
        file << "if (scriptFile.exists) { scriptFile.remove(); }\n";

        file.close();
    }

private:
    std::string escapePath(const std::string& rawPath) {
        std::string escaped = rawPath;
        for (size_t i = 0; i < escaped.length(); ++i) {
            if (escaped[i] == '\\') escaped.insert(i++, "\\");
        }
        return escaped;
    }
};

#endif // GENERATOR_H
