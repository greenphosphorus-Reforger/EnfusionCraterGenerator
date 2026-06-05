# EnfusionCraterGenerator

A one-click crater generator for Arma Reforger Workbench. Bakes thousands of randomized crater depressions directly into the terrain heightmap and surface mask with zero runtime performance cost.

## Requirements

- Arma Reforger Workbench
- Python 3.x — [download here](https://python.org)
- Dependencies install automatically on first run

## Installation

1. Drop `UTM_CraterDepthTool.c` into your addon's `Scripts/WorkbenchGame/WorldEditor/` folder
2. That's it — everything else generates itself on first use

## Usage

1. In Workbench open **Terrain Tool → Export height map → Modified** and save the `.asc` file to:
   `Documents/My Games/ArmaReforgerWorkbench/profile/export/`
   If this folder doesn't exist, create it manually.
2. Select **Crater Generator** from the Workbench tools menu
3. Adjust settings in the panel
4. Hit **Generate Craters** and wait for completion
5. Import `UTM_Heightmap_Cratered.asc` — Terrain Tool → Import height map → Modified
6. Import `UTM_CraterMask.png` — Terrain Tool → Import surface map → select your dirt layer

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| Count | 22000 | Total craters across the map |
| Seed | 42 | Change for a different pattern |
| Small Weight | 6 | How common small craters are |
| Medium Weight | 2 | How common medium craters are |
| Large Weight | 1 | How common large craters are |
| Small Depth | -0.33m | Depth of small craters |
| Medium Depth | -0.48m | Depth of medium craters |
| Large Depth | -0.96m | Depth of large craters |
| Small Radius | 2.0m | Radius of small craters |
| Medium Radius | 5.0m | Radius of medium craters |
| Large Radius | 9.0m | Radius of large craters |
| Mask Bloom | 1.0 | Dirt patch size multiplier. Higher = wider dirt around each crater |

## Notes

- Always back up your heightmap before importing
- The `.asc` heightmap is auto-detected from the export folder
- Remove old `.asc` files from the export folder if you have more than one
- Same seed = same pattern. Change seed for a new layout
