#ifndef FUNCS_H
#define FUNCS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include "paths.h"
#include "privileges.h"
#include "generator.h"
#include "starter.h"

namespace fs = std::filesystem;
using namespace std;
using namespace paths;

inline bool verbose = false;
inline std::string jsxPath = "";
inline vector<string> args = {""};

const vector<string> prompts = {
    "edit name (SPACES NOT SUPPORTED): ",
    "choose which videos to use: ",
    "audio name: ",
    "hii"
};

inline void setVerbose(bool v) {
    verbose = v;
}

inline void init(int argc, char* argv[], bool worker = false) {
    if (!isRunningAsAdmin() && !worker) {
        runAsAdmin(argc, argv);
        exit(0);
    }

    if (verbose && !worker) {
        cout << "initializing app, reading config.." << endl;
    }

    load_config();

    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    cout << prompts[3] << endl;
}

inline vector<string> videoListMain() {
    cout << endl << prompts[1] << endl;

    vector<string> videos = paths::get_video_list();
    for (size_t i = 0; i < videos.size(); ++i) {
        cout << "[" << i << "] " << videos[i] << endl;
    }

    return videos;
}

inline vector<int> get_selection_array(const vector<string>& videos) {
    string selection;
    cin >> selection;

    vector<int> selection_array;
    stringstream ss(selection);
    string item;
    bool isValid = true;

    while (getline(ss, item, ',')) {
        try {
            int idx = stoi(item);
            if (idx >= 0 && idx < static_cast<int>(videos.size())) {
                selection_array.push_back(idx);
            } else {
                isValid = false;
                break;
            }
        } catch (...) {
            isValid = false;
            break;
        }
    }

    if (!isValid) {
        cout << "invalid selection entered. please try again." << endl;
        cin >> selection;
        selection_array.clear();
        stringstream retry_ss(selection);
        while (getline(retry_ss, item, ',')) {
            try {
                int idx = stoi(item);
                if (idx >= 0 && idx < static_cast<int>(videos.size())) {
                    selection_array.push_back(idx);
                }
            } catch (...) {}
        }
    }

    return selection_array;
}

inline void copy_videos(const vector<int>& selection_array, const vector<string>& videos, const string& editName) {
    paths::make_directories(paths::edit_videos_path(editName));
    for (int idx : selection_array) {
        fs::path src(videos[idx]);
        fs::path dst = paths::edit_videos_path(editName) / src.filename();
        paths::copy_file(src, dst);
    }
    if (verbose) {
        cout << "copied videos: ";
        for (int idx : selection_array) {
            cout << videos[idx] << endl;
        }
    }
}

inline void worker_copy_videos(const vector<string>& videos, const string& editName) {
    paths::make_directories(paths::edit_videos_path(editName));
    for (const auto& video : videos) {
        fs::path src(fs::path(videos_folder) / video);
        fs::path dst = paths::edit_videos_path(editName) / src.filename();
        paths::copy_file(src, dst);
    }
    if (verbose) {
        cout << "copied videos: ";
        for (const auto& video : videos) {
            cout << video << endl;
        }
    }
}

inline void copy_audio(const string& audioName, const string& editName) {
    paths::make_directories(paths::edit_audios_path(editName));
    fs::path src = fs::path(audios_path) / audioName;
    fs::path dst = paths::edit_audios_path(editName) / src.filename();
    paths::copy_file(src, dst);

    if (verbose) {
        cout << "copied audio: " << src << " to " << dst << endl;
    }
}

inline void generate_jsx(const string& editName, const vector<string>& selectedVideos, const string& audioPath) {
    if (!isAfterFX) {
        cout << "skipping jsx generation, as you are not using after effects.." << endl;
        return;
    }
    JSXGenerator jsx;
    jsxPath = scripts_path + "\\" + editName + "_autogen.jsx";
    jsx.generateJSX(jsxPath, selectedVideos, audioPath);

    if (verbose) {
        cout << "generated jsx: " << jsxPath << endl;
    }
}

inline int start_software() {
    return start(string(software_path), verbose);
}

inline int handle_logic() {
    int code = 0;

    cout << prompts[0];
    string editName;
    cin >> editName;

    fs::path editPath = paths::edit_path(editName);
    bool editPathStatus = paths::ensure_dir(editPath);

    if (verbose) {
        cout << "edit name: " << editName << endl;
        cout << "edit path: " << editPath << endl;
        cout << "edit folder status check: " << editPathStatus << endl;
    }

    vector<string> videos = videoListMain();
    vector<int> selection_array = get_selection_array(videos);
    copy_videos(selection_array, videos, editName);

    cout << prompts[2];
    string audioName;
    cin >> audioName;
    if (verbose) {
        cout << "audio name: " << audioName << endl;
    }

    copy_audio(audioName, editName);

    vector<string> selectedVideos;
    for (int idx : selection_array) {
        selectedVideos.push_back(videos[idx]);
    }

    if (verbose) {
        cout << "selected videos: ";
        for (const auto& video : selectedVideos) {
            cout << video << endl;
        }
    }

    try {
        generate_jsx(editName, selectedVideos, (fs::path(audios_path) / audioName).string());
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return -1;
    }

    code = start_software();
    cin.ignore();
    return code;
}

inline int worker() {
    int code = 0;

    if (args.size() < 4) {
        cerr << "usage: worker.exe \"EditName\" \"Video1.mp4,Video2.mp4\" \"Audio.mp3\"" << endl;
        return -1;
    }

    string editName  = args[1];
    string videos    = args[2];
    string audioName = args[3];

    // Create folders
    prepare_edit_folders(editName);

    // Split video list
    vector<string> selectedVideos;
    stringstream ss(videos);
    string video;
    while (getline(ss, video, ',')) {
        selectedVideos.push_back(video);
    }

    // Proceed
    worker_copy_videos(selectedVideos, editName);
    copy_audio(audioName, editName);

    vector<string> selectedVideosJsx;
    for (const auto& video : selectedVideos) {
        selectedVideosJsx.push_back((fs::path(videos_folder) / video).string());
    }
    string audioPath = (fs::path(audios_path) / audioName).string();
    generate_jsx(editName, selectedVideosJsx, audioPath);

    code = start_software();
    return code;
}


#endif // FUNCS_H
