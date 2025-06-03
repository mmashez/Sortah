#include <iostream>
#include <string>
#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

string formatForJsx(string path) {
    // replace "\\" or "\" with "/"
    for (size_t pos = 0; (pos = path.find('\\', pos)) != string::npos; ++pos) {
        path.replace(pos, 1, "/");
    }
    return path;
}

inline int worker() {
    try {
        // load config
        string cfg_filename = "config.json";
        cout << "[debug] loading config from: " << fs::absolute(cfg_filename) << endl;
        if (!fs::exists(cfg_filename)) {
            cerr << "[error] config file not found: " << cfg_filename << endl;
            exit(-1);
        }

        // load JSON config

        json config;
        ifstream config_file(cfg_filename);
        config_file >> config;

        cout << "[debug] config contents:\n" << config.dump(4) << endl;

        // print out worker version
        cout << "version: " <<  config["worker-ver"] << endl;

        // check required string keys
        vector<string> required_keys = {
            "editsPath",
            "videosPath",
            "audiosPath",
            "softwarePath",
            "ae_startupScriptsPath"
        };
        for (auto& key : required_keys) {
            if (!config.contains(key) || config[key].is_null() || !config[key].is_string()) {
                cerr << "[error] config key missing or invalid: " << key << endl;
                exit(-1);
            }
        }

        // grab paths
        string edits_folder  = config["editsPath"].get<string>();
        string videos_folder = config["videosPath"].get<string>();
        string audios_path   = config["audiosPath"].get<string>();
        string software_path = config["softwarePath"].get<string>();
        string scripts_path  = config["ae_startupScriptsPath"].get<string>();

        // check temp files
        vector<string> temps = {
            "TMP_SELECTED_VIDS.sortahtemp",
            "TMP_SELECTED_AUDIOS.sortahtemp",
            "TMP_EDIT_NAME.sortahtemp"
        };
        for (auto& f : temps) {
            if (!fs::exists(f)) {
                cerr << "[error] required file missing: " << f << endl;
                cout << "run this through the GUI, not manually." << endl;
                exit(-1);
            }
        }

        // read selected videos
        vector<string> selected_videos;
        {
            ifstream in("TMP_SELECTED_VIDS.sortahtemp");
            string l;
            while (getline(in, l)) if (!l.empty()) selected_videos.push_back(l);
        }

        // read selected audios
        vector<string> selected_audios;
        {
            ifstream in("TMP_SELECTED_AUDIOS.sortahtemp");
            string l;
            while (getline(in, l)) if (!l.empty()) selected_audios.push_back(l);
        }

        // read edit name
        string edit_name;
        {
            ifstream in("TMP_EDIT_NAME.sortahtemp");
            in >> edit_name;
        }
        if (edit_name.empty()) {
            cerr << "[error] edit name is empty." << endl;
            exit(-1);
        }

        // create folder structure
        vector<fs::path> folders = {
            fs::path(edits_folder)  / edit_name,
            fs::path(edits_folder)  / edit_name / "footages" / "videos",
            fs::path(edits_folder)  / edit_name / "footages" / "audios"
        };
        for (auto& p : folders) {
            if (!fs::exists(p) && !fs::create_directories(p)) {
                cerr << "[error] failed to create: " << p << endl;
                exit(-1);
            }
        }
        cout << "[info] folders created.\n";

        // copy videos
        for (auto& fn : selected_videos) {
            fs::path src = fs::path(videos_folder) / fn;
            fs::path dst = fs::path(edits_folder) / edit_name / "footages" / "videos" / fn;
            if (!fs::copy_file(src, dst, fs::copy_options::overwrite_existing)) {
                cerr << "[error] failed to copy video: " << fn << endl;
                exit(-1);
            }
        }

        // copy audios
        for (auto& fn : selected_audios) {
            fs::path src = fs::path(audios_path) / fn;
            fs::path dst = fs::path(edits_folder) / edit_name / "footages" / "audios" / fn;
            if (!fs::copy_file(src, dst, fs::copy_options::overwrite_existing)) {
                cerr << "[error] failed to copy audio: " << fn << endl;
                exit(-1);
            }
        }
        cout << "[info] files copied.\n";

        // generate JSX & launch AE if enabled
        if (config["isAfterFX"].get<bool>()) {
            fs::path jsx_path = fs::path(scripts_path) / (edit_name + "_sortah_autogen.jsx");
            ofstream jsx(jsx_path);
            if (!jsx.is_open()) {
                cerr << "[error] couldn't write jsx: " << jsx_path << endl;
                exit(-1);
            }
            jsx << "// AUTO-GENERATED CODE BY SORTAH\n\n"
                << "var scriptFile = new File($.fileName);\n"
                << "app.beginUndoGroup(\"Auto Import\");\n\n";
            for (auto& fn : selected_videos)
                jsx << "app.project.importFile(new ImportOptions(File(\"" << formatForJsx(edits_folder) << "/" << edit_name << "/footages/videos/" << fn << "\")));\n";
            for (auto& fn : selected_audios)
                jsx << "app.project.importFile(new ImportOptions(File(\"" << formatForJsx(edits_folder) << "/" << edit_name << "/footages/audios/" << fn << "\")));\n";
            jsx << "\napp.endUndoGroup();\n"
                << "alert('done! - sortah');\n"
                << "if (scriptFile.exists) scriptFile.remove();\n";
            jsx.close();

            cout << "[info] jsx written to " << jsx_path << "\n"
                 << "[info] launching After Effects...\n";
            system(software_path.c_str());
        } else {
            cout << "[info] skipping JSX (isAfterFX=false)\n";
        }

    } catch (const exception& e) {
        cerr << "[fatal error] " << e.what() << endl;
        exit(-1);
    }

    cout << "[done] all operations completed." << endl;
    return 0;
}