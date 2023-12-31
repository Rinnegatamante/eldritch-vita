# PSVita Readme
Port of Eldritch for PSVITA based on official source code release with modifications from [usineur](https://github.com/usineur/Eldritch) and [ptitSeb](https://github.com/ptitSeb/Eldritch) that add GLES2 support and other minor bugfixes.
Thanks to CatoTheYounger for testing the homebrew.

Notes on compilation:
vitaGL needs to be compiled with HAVE_GLSL_SUPPORT=1.

## Installation Instructions

- Install libshacccg.suprx (The fastest way to install it is by running this homebrew: https://vitadb.rinnegatamante.it/#/info/997)
- Install the vpk of this port.
- Gather your Eldritch data files (the game can be purchased in several stores, for example Steam)
- Copy eldritch-audio.cpk, eldritch-base.cpk, eldritch-meshes.cpk, eldritch-textures.cpk and eldritch-world.cpk to ux0:data/Eldritch
- Put prefs.cfg in ux0:data/Eldritch (this has a preset controls binding and optimal settings for performances).


# Original Readme

Thanks for downloading this distribution of Eldritch's source code!

Please note that this is an unsupported, unmaintained release;
documentation is limited to this readme file and I will not be
integrating changes. Questions and bug reports are welcome at
david.pittman@gmail.com, but I cannot promise I will be able to
respond.

This release has been updated from the 2014 release to include
the new audio engine (a modified version of SoLoud) as well as
the content for the Steam version of the game, without any
dependencies on Steam's libraries.

It will be necessary for OS X and Linux users to download SDL 2.0
to build Eldritch. For Windows users, the library files included
here should suffice.

IDE project files are provided for Visual Studio 2015, Code::Blocks,
and Xcode, but it may be more desirable to rebuild your own project
file.

This distribution does not include most of the game's content, but
configuration, script, and room files are included and are licensed
the same as the source code. (All game content not included here is
not covered by these license terms and may not be redistributed.)

For Windows users, precompiled Release binaries have been
provided for immediate access to the game's debug tools and level
editor. To access the editor, press Tab during gameplay.

The game's .cpk files (not included here; you must purchase a copy
of Eldritch to get them) can be unpacked with the FilePacker.exe tool.
Deleting, renaming, or moving the .cpk files will force the game to
load from loose files instead.

Configuration/script files (.config extension) are used to define
most aspects of the game: procedural generation parameters, entity
composition, AI behaviors, etc. They can be parsed at runtime or
precompiled with ConfigCompiler.exe.

The source code for the tools is included, but they have not been
ported to OS X or Linux.

Happy coding!
David
