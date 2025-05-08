#include <iostream>
#include "paths.h"
#include <vector>
#include <sstream>
#include <stdexcept>
#include "generator.h"
#include "starter.h"
#include "privileges.h"
#include <windows.h>
#include <shellapi.h>

namespace fs = std::filesystem;

using namespace std;
using namespace fs;
using namespace paths;

inline bool verbose = false;
std::string jsxPath = "";

const vector<string> prompts = {
    "edit name (SPACES NOT SUPPORTED): ",
    "choose which videos to use: ",
    "audio name: "
};

void init() {
    // if app didnt start as admin, run it as admin, then end current proccess
    if (!isRunningAsAdmin()) {
        runAsAdmin();

        exit(0);
    }
    if (verbose) {
        cout << "initializing app, reading config.." << endl;
    }
    // Load JSON config
    load_config();

    cout << "hii" << endl;
}

vector<string> videoListMain() {
    cout << endl << prompts[1] << endl;
    
    vector<string> videos = paths::get_video_list();

    int numeration_temp = -1;
    for (const string& video : videos) {
        numeration_temp++;
        cout << "[" << numeration_temp << "] " << video << endl;
    }

    return videos;
}

vector<int> get_selection_array(const vector<string>& videos) {
    string selection;
    cin >> selection;

    vector<int> selection_array;
    stringstream ss(selection);
    string item;
    bool isValid = true;

    while (getline(ss, item, ',')) {
        try {
            int selectionInt = stoi(item);
            if (selectionInt >= 0 && selectionInt <= videos.size()-1) {
                selection_array.push_back(selectionInt);
            } else {
                isValid = false;
                break;
            }
        } catch (invalid_argument&) {
            isValid = false;
            break;
        }
    }

    if (!isValid) {
        cout << "invalid selection entered. please try again." << endl;
        cin >> selection;
        selection_array.clear();
        ss = stringstream(selection);
        while (getline(ss, item, ',')) {
            try {
                int selectionInt = stoi(item);
                if (selectionInt >= 0 && selectionInt <= videos.size()-1) {
                    selection_array.push_back(selectionInt);
                }
            } catch (...) {}
        }
    }

    return selection_array;
}

void copy_videos(const vector<int>& selection_array, const vector<string>& videos, const string& editName) {
    paths::make_directories(paths::edit_videos_path(editName));
    for (int index : selection_array) {
        fs::path src(videos[index]);
        fs::path dst = paths::edit_videos_path(editName) / src.filename();
        paths::copy_file(src, dst);
    }
    if (verbose) {
        cout << "copied videos: ";
        for (int index : selection_array) {
            cout << videos[index] << endl;
        }
    }
}

void copy_audio(const string& audioName, const string& editName) {
    paths::make_directories(paths::edit_audios_path(editName));
    fs::path audioSrc = paths::audios_path + "\\" + audioName;
    fs::path audioDst = paths::edit_audios_path(editName) / fs::path(audioName).filename();
    paths::copy_file(audioSrc, audioDst);

    if (verbose) {
        cout << "copied audio: " << audioSrc << " to " << audioDst << endl;
    }
}

void generate_jsx(const string& editName, const vector<string>& selectedVideos, const string& audioPath) {
    if (!isAfterFX) {
        cout << "skipping jsx generation, as you are not using after effects.." << endl;
    }
    JSXGenerator jsx;
    jsxPath = scripts_path + "\\" + editName + "_autogen.jsx";
    jsx.generateJSX(jsxPath, selectedVideos, audioPath);

    if (verbose) {
        cout << "generated jsx: " << jsxPath << endl;
    }
}

int start_software(const string& jsxPath) {
    return start(string(software_path), verbose);
}


int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-verbose") {
            verbose = true;
            cout << "verbose mode enabled!" << endl;
        }
    }

    init();

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
    for (int index : selection_array) {
        selectedVideos.push_back(videos[index]);
    }
    if (verbose) {
        cout << "selected videos: ";
        for (string video : selectedVideos) {
            cout << video << endl;
        }
    }
    generate_jsx(editName, selectedVideos, string(audios_path) + "\\" + audioName);
    start_software(editPath.string());

    // wait for ANY user input to exit
    cin.ignore();
    return 0;

}
