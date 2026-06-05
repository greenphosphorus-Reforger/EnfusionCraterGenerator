# EnfusionCraterGenerator

A one-click crater generator for Arma Reforger Workbench. Bakes thousands of randomized crater depressions directly into the terrain heightmap and surface mask with zero runtime performance cost.

## Requirements

- Arma Reforger Workbench
- Python 3.x — [download here](https://python.org)
- Dependencies install automatically on first run

## Installation

1. Drop `UTM_CraterDepthTool.c` into your addon's `Scripts/WorkbenchGame/WorldEditor/` folder — if this folder path doesn't exist, create it manually
2. That's it — everything else generates itself on first use

## Usage

**Step 1 — Export your heightmap**
In Workbench go to **Terrain Tool → Manage → Export height map → Modified**
Save the `.asc` file to:
`Documents/My Games/ArmaReforgerWorkbench/profile/export/`
If this folder doesn't exist, create it manually.

**Step 2 — Generate craters**
1. Select **Crater Generator** from the Workbench tools menu
2. Adjust settings in the panel to your liking
3. Hit **Generate Craters** and wait for completion in the log

**Step 3 — Import the heightmap**
In Workbench go to **Terrain Tool → Manage → Import height map → Modified**
Select `UTM_Heightmap_Cratered.asc` from the export folder.

**Step 4 — Import the surface mask**
1. Go to **Terrain Tool → Paint**
2. Right-click the dirt/soil material you want to use for crater texture (e.g. `Dirt_02`)
3. Select **Priority surface mask import...**
4. Navigate to the export folder and select `UTM_CraterMask.png`
5. Click **Open**

The crater depressions and dirt texture are now baked into your terrain.

<img width="287" height="737" alt="Screenshot 2026-06-04 233535" src="https://github.com/user-attachments/assets/d7bf4fbd-cec4-44ad-bb35-2562c599df5b" />
<img width="528" height="478" alt="Screenshot 2026-06-04 234003" src="https://github.com/user-attachments/assets/ccd1e984-7f2f-4d9d-a526-2df03b92f976" />

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
