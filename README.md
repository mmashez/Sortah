# sortah.

sortah is a minimalist file organizer built to help content creators manage their project files through fully customizable settings defined in `config.json`.  
it is primarily targeted at Adobe After Effects® CS4 (v9.0, 2008 or higher) users.

> ⚠️ **platform support:**  
> currently supports **Windows only**.  
> as noted in `note.txt`, the minimum requirement is **Windows XP SP2+**, as this tool relies on PowerShell to launch your editing software, specified in `config.json`.

---

there is also a **worker** application for the **sortah organizer!**
use **worker.exe** (e.g. worker.exe "editname" "clip1.mkv,clip2.mkv" "audio.mp3")

### 📄 disclaimer

**Adobe After Effects®** is a registered trademark of **Adobe Inc.**  
this project is not affiliated with, endorsed by, or in any way officially connected to Adobe Inc.  
this project utilizes Adobe After Effects' startup scripting system for its auto-import feature, which then self-deletes the script (see `generator.h`).

this project is licensed under the **MIT License**. to learn more, click [here](https://choosealicense.com/).

throughout this project, the terms **“we,” “us,”** or **“this project”** refer to either the **sortah** project itself or its developers.

credits to nlohmann for the [json library](https://github.com/nlohmann/json), which is under the MIT license.
