# Extended Demon Stars

Complete Geode mod project.

## What it does

Only Demon-rated levels receive the extended displayed star count.

Normal levels, including normal 8–9 star Insane levels, are untouched.

### Demon scale

- Easy Demon: 100–199
- Medium Demon: 200–399
- Hard Demon: 400–699
- Insane Demon: 700–999
- Extreme Demon: 1000+

The Nightmare is the 100-star starting point.

Actual Geometry Dash rewards are not changed.

## Difficulty weighting

Length > Frame Perfects > Timing > Precision > Visibility > Awkwardness > Game Mode.

Extreme Demons additionally use AREDL placement with an exponential top-end curve.

## Build locally

Install the current Geode SDK/CLI, then:

```bash
geode build
```

The current Geode documentation recommends `geode new` for creating projects and `geode build` for building them.

## GitHub

Create a repository named:

`extended-demon-stars`

Upload this project to it.

The GitHub Actions workflow will build platform packages automatically.

## Important

`mod.json` and `src/DataManager.cpp` contain `YOUR_USERNAME`. Replace it with your GitHub username before the first GitHub build.

The AREDL updater is intentionally isolated in `tools/update_data.py`, so API schema changes can be fixed there without changing the Geode mod.
