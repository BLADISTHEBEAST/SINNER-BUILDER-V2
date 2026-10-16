# SINNER BUILDER V2 — Switch Architecture

A JUCE-based Windows VST3 software instrument source project for FL Studio. The project includes a GitHub Actions workflow to compile the VST3 on a Windows build runner. **This ZIP is not itself a compiled plugin.**

## Engines
- **SOUL** — smoother, softer tones with gentle movement.
- **MACHINE** — metallic, inharmonic harmonics for robotic and mechanical tones.
- **BEAST** — denser harmonics and stronger saturation for aggressive bass sounds.

Only one engine is active at a time. The Engine choice appears in the parameter editor and can be automated from FL Studio.

## Controls
Engine, Tune, Tone, Metal, Noise, Drive, Motion, Attack, Decay, Sustain, Release, Output.

## Easiest build route: GitHub Actions (no compiler installation on your PC)
This project contains `.github/workflows/build-windows-vst3.yml`. It asks a Windows runner to download JUCE, compile SINNER BUILDER, and publish the resulting VST3 folder as a downloadable workflow artifact.

1. Sign in to GitHub and create a **new empty repository** (for example, `sinner-builder-v2`). Do not initialize it with a README or license.
2. Extract this ZIP on your computer.
3. Upload the **contents** of the extracted folder—including the hidden `.github` folder—to the new repository. Ensure `.github/workflows/build-windows-vst3.yml` exists in the repository. If GitHub's web uploader hides dot-folders or is awkward, use GitHub Desktop to add the extracted folder as a repository and push it.
4. Open the repository's **Actions** tab. If prompted, enable Actions.
5. The workflow runs on the first push to `main`. You can also select **Build SINNER BUILDER VST3** and choose **Run workflow**.
6. Wait for the workflow to finish. Open its successful run and download the artifact named `SINNER-BUILDER-V2-Windows-VST3`.
7. Extract that artifact. It should contain `SINNER BUILDER.vst3` (a plugin bundle folder). Do not rename the `.vst3` folder.
8. Copy the `.vst3` folder to `%CommonProgramFiles%\\VST3` (usually `C:\\Program Files\\Common Files\\VST3`).
9. Open FL Studio → Plugin Manager / Manage plugins → scan for plugins, then load SINNER BUILDER as an instrument.

GitHub may require you to sign in and accept its terms. The workflow uses GitHub-hosted Windows build infrastructure; this chat cannot start the build in your account or retrieve its artifact without repository access. Check that the Actions run succeeds before treating the plugin as built.

## Alternative local build (Windows)
Requirements: Visual Studio 2022 Community with **Desktop development with C++**, CMake 3.22+, and JUCE 8.0.6. Place the JUCE folder beside `CMakeLists.txt` and name it `JUCE`, then run in Developer PowerShell for VS 2022:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --target SINNER_BUILDER_VST3
```

Expected output is similar to:

`build/SINNER_BUILDER_artefacts/Release/VST3/SINNER BUILDER.vst3`

Exact paths can vary by JUCE version and generator.

## Test checklist in FL Studio
- Load it as an instrument and play notes from the piano roll or MIDI keyboard.
- Switch between SOUL, MACHINE, and BEAST and confirm each has a distinct tone.
- Try short notes, sustained notes, chords, and automation.
- Save and reopen the project to verify parameters restore.
- Keep output conservative while auditioning.

## Current limitations
This is a source-level prototype. The interface is a simple JUCE generic parameter editor with a Blade X-style header; it is not yet the planned custom knob interface. The synthesis algorithms are initial versions and need listening tests. No compiled binary is included in this package.

## License note
SINNER BUILDER is a Blade X / BEAST INTEL project prototype. JUCE is a third-party framework with its own licensing terms; review them before distributing a commercial build.
