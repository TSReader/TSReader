# TSReaderPro - MPEG Transport Stream Analyzer

**Version 2.8.53h-memorial**

TSReader is a Windows application for analysing MPEG Transport Streams.
Originally developed circa 2004-2008 with Visual C++ 6.0 by Rod Hewitt
KG6TTD (G6TTD), this codebase has been migrated to build with modern
Visual Studio (2022 / 2026) using CMake.

> **In memory of Rod Hewitt KG6TTD (G6TTD)** — passed away March 2025.
> This project continues his work under a memorial build. The running
> application shows a splash screen at startup with memorial text and
> a link to donate via PayPal to help cover hosting costs.

The codebase originally shipped in three editions (Lite, Standard, Pro).
This modernisation project focuses exclusively on the **Pro edition** —
the top-tier version with all features, decoders, and plugins enabled.

## Prerequisites

| Software | Version | Notes |
|----------|---------|-------|
| Visual Studio | 2022 (v17) or 2026 (v18) | Community edition is fine. Install the **Desktop development with C++** workload |
| CMake | 4.3.0+ | Must be on PATH |
| Git | Any recent | For cloning the repo |
| Inno Setup 6 | 6.7+ | Only needed for building the installer |

## Quick Start

```cmd
git clone git@github.com:TSReader/TSReader.git
cd TSReader

:: Generate the Visual Studio solution (Win32/x86 is required)
cmake -G "Visual Studio 18 2026" -A Win32 -B build

:: Build Release
cmake --build build --config Release
```

For Visual Studio 2022, use:

```cmd
cmake -G "Visual Studio 17 2022" -A Win32 -B build
```

> **Note:** The `-A Win32` flag is required. TSReader is a 32-bit (x86) application.

## Build Output

After a successful build, the following files will be in `build\Release\`:

| File | Description |
|------|-------------|
| `TSReaderPro.exe` | Main application (~1.3 MB) |
| `TSReader_SourceHelper.dll` | Helper library — **built from `TSReader_SourceHelper/` source** alongside the exe |
| `libfaad2.dll` | AAC audio decoder (pre-built, auto-copied) |
| `PEGRPCS.DLL` | Charting library (pre-built, auto-copied) |

## Running

TSReader requires additional runtime files from the source tree. The easiest options:

1. **Use the installer** (recommended) — see *Building the Installer* below.
2. **Run from source directory** — copy the build output to the repo root and run from there.

When the app starts you'll see the memorial splash screen (photo, tribute
text, GitHub and PayPal links, plus a **Continue** button). Click
**Continue** to dismiss it and the normal source-selection / tune flow
begins.

### Required Runtime Files

TSReader loads these from its working directory:

- `*.bmp`, `*.png` — UI icons and status images (plus `rod_splash.png` for the memorial splash)
- `*.ini` — Configuration files
- `*.lst` — Source preset lists
- `Sources\` folder — **Source plugin DLLs** (loaded dynamically via LoadLibrary)
- `Forwarders\` folder — Stream forwarder plugin DLLs
- `Satellites\` folder — Satellite transponder configs

> **Important:** Without the `Sources\` folder, TSReader cannot open any transport streams. At minimum, `TSReader_File.dll` must be present for file-based input.

## Building the Installer

```cmd
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer.iss
```

Output: `installer_output\TSReaderPro_Setup.exe` (~10 MB)

The installer packages everything needed: executable, DLLs (including
the built-from-source `TSReader_SourceHelper.dll`), bitmaps, configs,
source plugins from both `Sources/` and `Sources_Archive/` (flattened
into one `{app}\Sources` at install time), forwarders, satellite
data, and the memorial splash image. The default install location is
`C:\Program Files (x86)\TSReaderPro` (following Windows guidelines for
32-bit applications); the user can change this during setup.
Runtime-writable files (`*.lst`, `*.ini`, `Satellites\*.ini`) are
installed with `Users:modify` ACLs so non-admin users can persist
tune-dialog changes in place.

## Recent Changes

- **Version bumped** to `2.8.53h-memorial` (`FILEVERSION` 2,8,53,6).
  `TSReader_SourceHelper.dll` is now built from the in-repo source as
  a first-class CMake target, with 14 stub functions filling in the
  ordinals (49-63) that prebuilt 2019-2022 source plugins import but
  Rod's pre-2008 source predates. The host and SourceHelper now share
  the same `VARIABLES` struct layout regardless of `MAX_SOURCE_MODULES`,
  so UDP / TCP / satellite tune-dialog edits actually reach `v->ss`
  instead of overshooting into the enlarged `sourcemodules[]` region.
  Resolves `KNOWN_ISSUES` BA-1 (UDP data flow broken in 2.8.53f/g).
- **Version bumped** to `2.8.53g-memorial` (`FILEVERSION` 2,8,53,5).
  Fixes a NULL-pointer fault when a user cancels the current source's
  tune dialog and then picks a different source from the resulting
  source-selection dialog. The cancel-and-reselect path rebound the
  host's function pointers but skipped calling the new plugin's
  `TSReader_Init`, so the plugin's static `ss` was still NULL when its
  `TSReader_TuneDialog` ran — `ss->fDontTune` then crashed at offset
  0x1851 in `TSReader_UDPMulticast.dll v2.6.0.41` (and any other 2008-
  vintage source plugin that follows the same `ss = pss;` pattern).
- **Version bumped** to `2.8.53f-memorial` (`FILEVERSION` 2,8,53,4).
  Installer now grants Users `modify` on runtime-writable config files
  (`*.lst`, `BISS.ini`, `diseqcU.ini`, `dvbt.ini`, `S2emu.ini`,
  `Satellites\*.ini`) so non-elevated users can persist tune-dialog
  changes under `C:\Program Files (x86)\TSReaderPro\`. Previously the
  underlying `CreateFile(GENERIC_WRITE)` failed silently and dialog
  changes appeared to vanish on next launch.
- **Version bumped** to `2.8.53e-memorial` (`FILEVERSION` 2,8,53,3).
  Includes raising `MAX_SOURCE_MODULES` from 128 to 512 so the now
  333-strong combined plugin set in `Sources/` fits — the 128 cap
  produced "Too many source modules" at startup.
- **Version bumped** to `2.8.53d-memorial` (`FILEVERSION` 2,8,53,2).
  Marks the upgrade of the bundled `TSReader_SourceHelper.dll` from
  the 2008 build to a 2018 build, which is what makes the BDA source
  plugins added in `Sources_Archive/` actually loadable. Without that
  newer SourceHelper, startup aborted with "ordinal 63 could not be
  located in TSReader_BDASource_TBS5220DVBC.dll" — that ordinal lives
  in SourceHelper, not in the plugin.
- **Version bumped** to `2.8.53c-memorial` (exe `FILEVERSION`/
  `PRODUCTVERSION` 2,8,53,1 in `TSReader.rc`; installer `AppVersion`).
- **Memorial splash screen** at startup with Rod's photo, tribute text,
  GitHub link, PayPal donate link, and a Continue button.
- **Cancel in source tune dialog** now returns to the source selection
  dialog rather than exiting the application.
- **Version bumped** to `2.8.53b-memorial` (exe version info and installer).
- **URLs updated** throughout the codebase:
  - `coolstf.com` → `tsreader.co.uk`
  - `rod@coolstf.com` → `support@tsreader.co.uk`
  - GitHub references updated to `github.com/TSReader/TSReader`
- **Purchase link removed** (Digital River checkout URL is dead).
- **Installer** now installs to `C:\Program Files (x86)\TSReaderPro` by
  default with user-selectable directory and admin elevation.
- **Sources_Archive/** added: 218 additional source-plugin DLLs from
  Rod's posthumous archive (2019–2022 builds, including SAT>IP, IPTV,
  SRT, HDHomeRun DVB-T/C/ISDB-T and an extensive set of TBS BDA tuners).
  These are additive — no filename overlap with the existing `Sources/`
  set — and the installer copies both folders into `{app}\Sources` so
  TSReaderPro sees them as one flat plugin directory at runtime.

## Project Structure

```
TSReader/
├── CMakeLists.txt          # Main build file
├── installer.iss           # Inno Setup installer script
├── splash.c                # Memorial splash screen module
├── rod_splash.png          # Memorial photo
├── *.c / *.h               # Core TSReader source
├── stubs/                  # SDK replacement implementations
│   ├── isource_impl.c      # ImgSource SDK → stb_image + GDI
│   └── stb_image*.h        # stb single-header image libraries
├── include/                # Shared headers
├── h264/                   # H.264 decoder (built from source)
├── vc1/                    # VC-1 decoder (built from source)
├── libmad-0.15.1b/         # MPEG audio decoder (built from source)
├── a52dec-0.7.4/           # AC-3 audio decoder (built from source)
├── libmpeg2/               # MPEG-2 video decoder (pre-built .lib)
├── TSReader_SourceHelper/  # Helper DLL — built from source as a
│                           #   CMake target. Provides tune dialogs and
│                           #   SI/PSI helpers shared with all source
│                           #   plugins. Ordinals 1-48 are full
│                           #   implementations; 49-63 are __cdecl
│                           #   stubs covering 2008-2018 additions Rod's
│                           #   source predates.
├── Sources/                # Source plugin DLLs (pre-built)
├── Sources_Archive/        # Additional source plugins from Rod's
│                           #   posthumous archive (merged into
│                           #   {app}\Sources by the installer)
├── Forwarders/             # Forwarder plugin DLLs (pre-built)
└── Satellites/             # Satellite config INI files
```

## Troubleshooting

**CMake can't find a compiler** — Run from a Developer Command Prompt, or ensure VS C++ workload is installed.

**Missing DLLs at runtime** — Check that `libfaad2.dll`, `PEGRPCS.DLL`, and `TSReader_SourceHelper.dll` are alongside the exe. The build copies these automatically.

**No source plugins** — Ensure the `Sources\` subfolder with plugin DLLs is in the same directory as the executable.

**Splash photo not showing** — Ensure `rod_splash.png` is in the install directory (next to `TSReaderPro.exe`). The splash falls back to text-only if the image cannot be loaded.

**Clean rebuild:**

```cmd
rmdir /s /q build
cmake -G "Visual Studio 18 2026" -A Win32 -B build
cmake --build build --config Release
```
