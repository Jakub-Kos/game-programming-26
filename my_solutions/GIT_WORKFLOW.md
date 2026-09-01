# Git workflow for this repo

This repo has two remotes:

- `upstream` -> the course repo (`Chris-Carvelli/game-programming-26`). Read-only in practice — new
  exercises, fixes, and assets show up here over the semester.
- `origin` -> my own fork (`Jakub-Kos/game-programming-26`) on GitHub. This is where my own
  work gets pushed.

Check the current setup any time with:

```sh
git remote -v
```

## Getting new course content

Whenever new material has been pushed to the course repo:

```sh
git fetch upstream
git log upstream/main -5      # optional: preview what's new before merging it in
git pull upstream main        # fast-forwards main with the new commits
```

This should almost always be a clean fast-forward, as long as tracked course files
(`exercises/`, `examples/`, `data/`, `lib/`, etc.) haven't been edited locally — which is
exactly why exercise work happens outside of those folders instead (see below).

If `CMakeLists.txt` was also touched on both sides (e.g. it gains an `add_subdirectory(my_solutions)`
line locally, and the course adds its own new subdirectory upstream), `git pull` may fall back to a
real merge instead of a fast-forward. That's normal — resolve the conflict by keeping both added
lines and continue.

## Pushing my own work

```sh
git add <files>
git commit -m "..."
git push origin main
```

`origin` is my fork, so I always have push access there regardless of what permissions
exist on the course repo.

## Where to put my own code

- `my_solutions/` — tracked, pushed to `origin`. This is where finished/in-progress exercise
  solutions actually live so they get committed and backed up.
- `exercises_workplace/` — gitignored, **never tracked at all**, not even on my own fork. Good for
  throwaway experiments, but don't rely on anything in here surviving — nothing in it can be
  pushed without force-adding it (`git add -f`), which defeats the point of it being scratch space.
- Files tracked by the course repo (anything under `exercises/`, `examples/`, `exercises_solutions/`
  as provided upstream, `lib/`, `data/`) should stay untouched locally, so `git pull upstream main`
  keeps fast-forwarding cleanly instead of hitting merge conflicts.

## Building after changes

The CMake project globs source files per folder, so it does **not** notice new/removed/renamed
`.c`/`.cpp` files (or new data files) automatically — it needs to be reconfigured. Editing an
*existing* file's contents doesn't require this, only add/remove/rename.

- **CLion**: reload the CMake project (the banner that pops up after the file change, or
  `Tools -> CMake -> Reload CMake Project`).
- **VS Code (CMake Tools)**: `Ctrl+Shift+P -> CMake: Configure` (or `Delete Cache and Reconfigure`
  if something looks stale).
- **Command line**, from the repo root:

```sh
cmake --build build --target <target_name>   # if a build/ dir already exists and is configured
# or, from scratch:
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Executables land in `build/`, one per source file, named after the file with the extension
stripped — **except** targets under `my_solutions/`, which get a `mysol_` prefix (e.g.
`my_solutions/E00_introduction.cpp` builds to `mysol_E00_introduction`). That prefix exists so a
solution file can share a filename with the matching file in `exercises/` without CMake target
name collisions — see the comment in `my_solutions/CMakeLists.txt`.

Build (and ideally run) locally before pushing to `origin`, since nothing here runs it for you.

## Quick reference

| Task                          | Command                          |
|-------------------------------|-----------------------------------|
| Get latest course material    | `git fetch upstream && git pull upstream main` |
| Push my own work              | `git push origin main`            |
| See what remotes are set up   | `git remote -v`                   |
| Preview upstream changes first| `git fetch upstream && git log main..upstream/main` |
| Rebuild after adding/removing a file | reconfigure CMake (IDE reload, or `cmake ..` again) |
| Build one target from the CLI | `cmake --build build --target <target_name>` |
