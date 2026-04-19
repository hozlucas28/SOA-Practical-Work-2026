<h1 align="center">
    SOA Practical Work [2026]
</h1>

<p align="center">
    <strong>Practical work repository for the Advanced Operating Systems (SOA) subject</strong>
    <br>
    <strong>- <a href="https://www.unlam.edu.ar/">UNLaM</a> (National University of La Matanza) -</strong>
</p>

<p align="center">
    <a href="#summary">Summary</a> •
    <a href="#installation">Installation</a> •
    <a href="#diagrams">Diagrams</a>
    <br>
    <a href="#application-structure">Application structure</a> •
    <a href="#development-team">Development team</a> •
    <a href="#license">License</a> •
    <a href="#acknowledgments">Acknowledgments</a>
</p>

<p align="center">
    <img src="docs/statics/billboard.png" alt="Billboard" width="800">
</p>

## Summary

This repository contains our practical work for the Advanced Operating Systems (SOA) subject at the [National University of La Matanza (UNLaM)](https://www.unlam.edu.ar/). It consists of an embedded system based on an ESP32, which has two operating modes (activated by push buttons):

- **Stock Mode:** Reports the current stock and alerts if the quantity on the shelves is below the established minimum. It uses an LCD screen to show the existing stock or the shortage, LED lights to visually indicate which shelf needs restocking, and weight sensors to calculate the stock on each shelf.

- **Security Mode:** Detects variations in the weight of the shelves. It employs an LCD screen to report on which shelf the alteration was detected, a buzzer as an audible alarm, LED lights to visually indicate the affected shelf, and weight sensors to register these variations.

> [!NOTE]
> Security mode has a higher priority than stock mode, so if both modes are activated at the same time, the system will execute security mode.

### Features

- Code conventions and standards
- Design and development of a finite state machine (FSM) to act according to the operating mode.
- Handling of sensors and actuators on an ESP32.
- Integration between an embedded system and a mobile application developed in Android.
- Real-time monitoring of stock and security alerts.

## Installation

- Clone the repository.
- Install [Visual Studio Code](https://code.visualstudio.com/), [Docker](https://www.docker.com/), and [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) (VSCode extension).
- Open the repository folder in Visual Studio Code.
- Reopen the project in a Dev Container, pressing `F1` and selecting `Dev Containers: Rebuild and Reopen in Container`.
- Wait for the container to be built and started.
- Press `F1` and select `Wokwi: Request a new License` option to get a free license for build the project.
- When you have the license, press `F1` and select `PlatformIO: Build` to build the source code.
- After the build is finished, press `F1` and select `Wokwi: Start Simulator` to run the project.
- That's it! You should see the project running in the Wokwi simulator.

> [!NOTE]
> When rebuilding the DevContainer, you might encounter an error such as `The container already exists or is in use`. Don't worry, it's a known bug, simply ignore it and rebuild it again.

> [!IMPORTANT]
> The first time you build the DevContainer, PlatformIO extension will request you to restart Visual Studio Code to finish the installation. Please do so, otherwise you won't be able to build the project.

## Application structure

```bash
SOA-Practical-Work-2026/
│
├── .devcontainer/        # DevContainer configuration files.
│
├── .github/
│   ├── workflows/        # GitHub Actions (CI/CD) workflows.
│   │
│   └── CODEOWNERS        # File to assign Pull Request and Issue reviewers.
│
├── docs/                 # Documentation files, such as diagrams, images, and other statics.
│
├── scripts/
│   └── health-check.sh   # Script to check that the necessary tools are installed.
│
├── src/                  # Source code of the project (`.h` and `.ino` files).
│
├── .editorconfig         # Configuration to standardize code style across different IDEs.
├── .gitattributes        # Configuration for Git attributes.
├── .gitignore            # List of all files and folders that Git should ignore.
├── .oxfmtrc              # Oxfmt configuration (code formatter).
├── cspell.json           # Code Spell Checker extension configuration.
├── diagram.json          # Circuit diagram of the project, exported from Wokwi.
├── lefthook.yaml         # Lefthook configuration (Git hooks manager).
├── LICENSE               # Project license.
├── platformio.ini        # PlatformIO configuration file.
├── README.md             # This file.
├── wokwi.toml            # Wokwi configuration file.
└── zizmor.yml            # Zizmor configuration (linter for GitHub Actions workflows).
```

## Development team

- [Casas Lautaro](https://github.com/lautarocasas)
- [Farias Maira](https://github.com/maifarias)
- [Hoz Lucas](https://github.com/hozlucas28)
- [Massa Valentin](https://github.com/ValentinMassa)

## License

This repository is under the [MIT license](LICENSE). For more information about what is permitted with the contents of this repository, visit [choosealicense.com](https://choosealicense.com/licenses/).

## Acknowledgments

We would like to thank the teachers from the [UNLaM](https://www.unlam.edu.ar/) Advanced Operating Systems (SOA) subject for their support and guidance.
