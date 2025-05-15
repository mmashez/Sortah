// paths.h
#ifndef paths_h
#define paths_h

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace paths {

    inline json config;
    inline std::string edits_folder;
    inline std::string videos_folder;
    inline std::string audios_path;
    inline std::string software_path;
    inline std::string scripts_path;
    inline bool isAfterFX;

    // Load config from config.json
    inline void load_config(const std::string& file = "config.json") {
        std::ifstream config_file(file);
        if (!config_file) {
            std::cerr << "failed to open config file: " << file << std::endl;
            return;
        }
        config_file >> config;
        edits_folder  = config["editsFolder"];
        videos_folder = config["videosFolder"];
        audios_path   = config["audiosPath"];
        software_path = config["softwarePath"];
        scripts_path  = config["startupScriptsPath"];
        isAfterFX     = config["isAfterFX"];
    }

    inline bool make_directories(const fs::path& dir) {
        try {
            return fs::create_directories(dir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "error creating directory '" << dir << "': " << e.what() << std::endl;
            return false;
        }
    }

    inline bool ensure_dir(const fs::path& dir) {
        if (!fs::exists(dir)) {
            make_directories(dir);
            return false;
        }
        return true;
    }

    inline std::vector<fs::path> list_files(const fs::path& dir) {
        std::vector<fs::path> files;
        if (fs::exists(dir) && fs::is_directory(dir)) {
            for (auto const& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file()) files.push_back(entry.path());
            }
        }
        return files;
    }

    inline bool copy_file(const fs::path& src, const fs::path& dst) {
        try {
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
            return true;
        } catch (const fs::filesystem_error& e) {
            if (std::string(e.what()).find("file already exists") != std::string::npos) return true;
            std::cerr << "error copying '" << src << "' to '" << dst << "': " << e.what() << std::endl;
            return false;
        }
    }

    inline fs::path edit_path(const std::string& edit_name) {
        return fs::path(edits_folder) / edit_name;
    }

    inline fs::path edit_videos_path(const std::string& edit_name) {
        return edit_path(edit_name) / "videos";
    }

    inline fs::path edit_audios_path(const std::string& edit_name) {
        return edit_path(edit_name) / "audios";
    }

    inline void prepare_edit_folders(const std::string& edit_name) {
        ensure_dir(edit_path(edit_name));
        ensure_dir(edit_videos_path(edit_name));
        ensure_dir(edit_audios_path(edit_name));
    }

    inline std::vector<std::string> get_video_list() {
        std::vector<std::string> out;
        for (auto& p : list_files(videos_folder)) out.push_back(p.string());
        return out;
    }

    inline std::vector<std::string> get_audio_list() {
        std::vector<std::string> out;
        for (auto& p : list_files(audios_path)) out.push_back(p.string());
        return out;
    }

} // namespace paths

#endif // paths_h
