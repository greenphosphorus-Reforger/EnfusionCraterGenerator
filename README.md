# EnfusionCraterGenerator

A one-click crater generator for Arma Reforger Workbench. Bakes thousands of randomized crater depressions directly into the terrain heightmap and surface mask with zero runtime performance cost. Works on any Enfusion map regardless of size.

## Requirements

- Arma Reforger Workbench
- Windows x64 PC
- Internet connection on first run (Python installs automatically)

## Installation

1. Drop `UTM_CraterDepthTool.c` into your addon's `Scripts/WorkbenchGame/WorldEditor/` folder — if this folder path doesn't exist, create it manually
2. That's it — everything else generates itself on first use
3. If the Crater Generator tool is not appearing in Workbench, close Workbench completely and reopen it

## Usage

**Step 1 — Export your heightmap**
In Workbench go to **Terrain Tool → Manage → Export height map → Modified**
Save the `.asc` file to:
`Documents/My Games/ArmaReforgerWorkbench/profile/export/`
If this folder doesn't exist, create it manually.

**Step 2 — Set Mask Size**
Go to **Terrain Tool → Info & Diags → Surface Map → Total** and note the first number (e.g. 4537).
Open the **Crater Generator** tool panel and enter that number into the **Mask Size** field.
You only need to do this once per map.

**Step 3 — Generate craters**
1. Select **Crater Generator** from the Workbench tools menu
2. Adjust settings in the panel to your liking
3. Hit **Generate Craters** and watch the log for progress
4. Python and all dependencies install automatically on first run — this may take a minute

**Step 4 — Import the heightmap**
In Workbench go to **Terrain Tool → Manage → Import height map → Modified**
Select `UTM_Heightmap_Cratered.asc` from the export folder.

**Step 5 — Import the surface mask**
1. Go to **Terrain Tool → Paint**
2. Right-click the dirt/soil material you want to use for crater texture (e.g. `Dirt_02`)
3. Select **Priority surface mask import...**
4. Navigate to `Documents/My Games/ArmaReforgerWorkbench/profile/export/` and select `UTM_CraterMask.png`
5. Click **Open**

The crater depressions and dirt texture are now baked into your terrain at no performance cost.

<img width="287" height="737" alt="Screenshot 2026-06-04 233535" src="https://github.com/user-attachments/assets/d7bf4fbd-cec4-44ad-bb35-2562c599df5b" />
<img width="528" height="478" alt="Screenshot 2026-06-04 234003" src="https://github.com/user-attachments/assets/ccd1e984-7f2f-4d9d-a526-2df03b92f976" />

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| Count | 22000 | Total craters across the map. Up to 200,000 supported |
| Seed | 42 | Change for a different pattern. Same seed = same layout |
| Small Weight | 6 | How common small craters are relative to other sizes |
| Medium Weight | 2 | How common medium craters are relative to other sizes |
| Large Weight | 1 | How common large craters are. Lower = rarer |
| Small Depth | -0.33m | Depth of small craters in metres |
| Medium Depth | -0.48m | Depth of medium craters in metres |
| Large Depth | -0.96m | Depth of large craters in metres |
| Small Radius | 2.0m | Radius of small craters in world metres |
| Medium Radius | 5.0m | Radius of medium craters in world metres |
| Large Radius | 9.0m | Radius of large craters in world metres |
| Mask Bloom | 1.0 | Dirt patch size multiplier. Higher = wider dirt around each crater |
| Mask Size | 0 | Surface mask resolution. Check Terrain Tool > Info & Diags > Surface Map > Total |

## Notes

- Always back up your heightmap before importing
- Python installs automatically if not found — no manual install required
- The `.asc` heightmap is auto-detected from the export folder
- Remove old `.asc` files from the export folder if you have more than one
- Same seed = same pattern. Change seed for a new layout
- Works on any Enfusion map — map dimensions are read automatically from the active world
