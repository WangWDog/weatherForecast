# Weather Forecast Application

This project provides both a command line interface (CLI) and a Qt based graphical interface (GUI) for querying weather information.

## Building

```bash
cmake -S . -B build
cmake --build build
```

Qt 6 and libcurl development packages are required. On Debian/Ubuntu you can install them with:

```bash
sudo apt-get install qt6-base-dev libcurl4-openssl-dev
```

If `configs/configKey.json` is present it will be copied to the build directory. The file is ignored by Git because it usually contains API keys.

## Running

After building you will get two executables inside the `build` directory:

- `weather_cli` – the traditional command line application
- `weather_gui` – a simple window that wraps the CLI

### Switching between CLI and GUI

Use `weather_cli` when you prefer terminal commands. Launch `weather_gui` for a windowed interface that internally runs the same commands.

Both executables use the same configuration files located under `build/configs`.
