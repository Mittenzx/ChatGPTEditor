# Building the project in Visual Studio (Windows / Unreal Engine 5.5)

This document explains how to reproduce the build and how to generate a Visual Studio solution for the Adastrea project and the ChatGPTEditor plugin.

Prereqs
- Unreal Engine 5.5 installed (adjust paths below if different).
- Visual Studio 2022 with C++ workload for Game development with C++.
- Git and repo checked out locally.
- (Optional) Python support installed for Unreal if you enable Python-related plugin features.

Quick steps (recommended)
1. Ensure Adastrea.uproject is present in the repository root.
2. Set OPENAI_API_KEY in your environment if you intend to use plugin features that call the OpenAI API:
   - Windows (PowerShell): $env:OPENAI_API_KEY = "your_key_here"
   - Windows (cmd): setx OPENAI_API_KEY "your_key_here"
3. Run the helper script to generate the Visual Studio solution:
   - scripts\generate-sln.bat
   - If your engine is in a different folder, set the UE_ROOT environment variable first:
     - cmd: set UE_ROOT="C:\Program Files\Epic Games\UE_5.5"
4. Open the generated .sln in Visual Studio.
5. Set the build configuration to "Development Editor" and platform "Win64" (or your preferred editor config).
6. Build the project. If you previously encountered compile errors, verify the following fixes are present in your working copy (these are the fixes that made the build succeed on the original machine):
   - Correct include paths in game code (e.g., #include "PlayerStatsComponent.h").
   - In ChatGPTEditor plugin:
     - Removed deprecated WorkspaceMenuStructure usage.
     - Added missing includes for JSON and HTTP modules (Dom/JsonObject.h, HttpModule, and Interfaces/IHttp*).
     - Guarded Python usage with #if defined(WITH_PYTHON) && WITH_PYTHON.
     - Fixed shadowed local variable names (e.g., renamed LogPath to AuditLogPath where applicable).
     - Declared optional dependency on PythonScriptPlugin in ChatGPTEditor.uplugin if the module uses Python.
     - Stubbed advanced features (test generation, blueprint generation) as needed to avoid compile-time dependencies.
7. If you still get include or dependency errors, verify the plugin's Build.cs includes the correct module dependencies:
   - Add "Json", "JsonUtilities", "HTTP", "Http", "Python" (optional) where used.

Notes and troubleshooting
- Unreal projects normally generate a machine- and engine-specific Visual Studio solution. Avoid checking in generated .sln files unless you really need them.
- If you get SDK or toolset mismatches, make sure Visual Studio's selected platform toolset matches the engine/UBT expectations (usually the one installed with Visual Studio Game Development workload).
- If plugin compilation fails with missing module errors, ensure the ChatGPTEditor.uplugin and corresponding Build.cs declare the modules used (e.g., HTTP, Json).
- If you are working in CI, you can run UBT to compile the plugin specifically:
  - Use Engine\Binaries\DotNET\UnrealBuildTool.exe with the appropriate target, or run the Engine's Build.bat.

If you'd like, I can also:
- Make small Build.cs or uplugin edits to declare missing dependencies, or enable/disable optional Python support.

