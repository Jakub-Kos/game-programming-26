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

## Quick reference

| Task                          | Command                          |
|-------------------------------|-----------------------------------|
| Get latest course material    | `git fetch upstream && git pull upstream main` |
| Push my own work              | `git push origin main`            |
| See what remotes are set up   | `git remote -v`                   |
| Preview upstream changes first| `git fetch upstream && git log main..upstream/main` |
