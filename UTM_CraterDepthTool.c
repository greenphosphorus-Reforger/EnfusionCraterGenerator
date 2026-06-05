#ifdef WORKBENCH

[WorkbenchToolAttribute(
	name: "Crater Generator",
	description: "Generates randomized crater depressions baked directly into the terrain heightmap and surface mask.",
	awesomeFontCode: 0xF6D3
)]
class UTM_CraterDepthTool : WorldEditorTool
{
	[Attribute("22000", UIWidgets.Slider, "Total number of craters to scatter across the map.", "1000 50000 1", category: "Craters")]
	protected int m_iCount;

	[Attribute("42", UIWidgets.Slider, "Random seed. Same seed = same pattern. Change this for a different layout.", "0 9999 1", category: "Craters")]
	protected int m_iSeed;

	[Attribute("6.0", UIWidgets.Slider, "How common small craters are relative to other sizes. Higher = more small craters.", "0.1 10 0.1", category: "Craters")]
	protected float m_fSmallWeight;

	[Attribute("2.0", UIWidgets.Slider, "How common medium craters are relative to other sizes.", "0.1 10 0.1", category: "Craters")]
	protected float m_fMediumWeight;

	[Attribute("1.0", UIWidgets.Slider, "How common large craters are relative to other sizes. Lower = rarer large craters.", "0.1 10 0.1", category: "Craters")]
	protected float m_fLargeWeight;

	[Attribute("-0.33", UIWidgets.Slider, "Depth of small craters in metres. Negative value digs into terrain.", "-5 0 0.01", category: "Craters")]
	protected float m_fSmallDepth;

	[Attribute("-0.48", UIWidgets.Slider, "Depth of medium craters in metres. Negative value digs into terrain.", "-5 0 0.01", category: "Craters")]
	protected float m_fMediumDepth;

	[Attribute("-0.96", UIWidgets.Slider, "Depth of large craters in metres. Negative value digs into terrain.", "-5 0 0.01", category: "Craters")]
	protected float m_fLargeDepth;

	[Attribute("2.0", UIWidgets.Slider, "Radius of small craters in world metres.", "0.5 20 0.1", category: "Craters")]
	protected float m_fSmallRadius;

	[Attribute("5.0", UIWidgets.Slider, "Radius of medium craters in world metres.", "0.5 20 0.1", category: "Craters")]
	protected float m_fMediumRadius;

	[Attribute("9.0", UIWidgets.Slider, "Radius of large craters in world metres.", "0.5 20 0.1", category: "Craters")]
	protected float m_fLargeRadius;

	[Attribute("1.0", UIWidgets.Slider, "Multiplier for the surface mask dirt patch size. 1.0 = matches crater exactly. Higher = wider dirt patch around each crater.", "0.5 3.0 0.1", category: "Craters")]
	protected float m_fMaskBloom;

	//------------------------------------------------------------------------------------------------
	protected void WriteGeneratorScript()
	{
		string profilePath;
		Workbench.GetAbsolutePath("$profile:", profilePath);
		string scriptPath = profilePath + "/export/UTM_CraterGen.py";

		if (FileIO.FileExists(scriptPath))
			return;

		FileHandle fh = FileIO.OpenFile(scriptPath, FileMode.WRITE);
		if (!fh)
		{
			Print("[UTM Crater Tool] ERROR: Could not write UTM_CraterGen.py to " + scriptPath, LogLevel.ERROR);
			return;
		}

		fh.WriteLine("\"\"\"");
		fh.WriteLine("UTM Crater Generator - All-in-one");
		fh.WriteLine("Run standalone (uses CONTROLS defaults) or from Workbench via UTM_CraterDepthTool.c:");
		fh.WriteLine("  python UTM_CraterGen.py [--count N] [--seed N] [--small-weight F] ...");
		fh.WriteLine("");
		fh.WriteLine("Outputs:");
		fh.WriteLine("  UTM_CraterMask.png       - surface mask, import as Priority Surface Mask in Workbench");
		fh.WriteLine("  UTM_Heightmap_Cratered.asc - modified heightmap, import as Modified heightmap in Workbench");
		fh.WriteLine("  UTM_Craters.csv          - crater positions (backup/reference)");
		fh.WriteLine("\"\"\"");
		fh.WriteLine("");
		fh.WriteLine("import argparse, csv, math, os, random, sys, time");
		fh.WriteLine("");
		fh.WriteLine("try:");
		fh.WriteLine("    import numpy as np");
		fh.WriteLine("    from PIL import Image");
		fh.WriteLine("except ImportError:");
		fh.WriteLine("    sys.exit(");
		fh.WriteLine("        \"\\n[UTM Crater Tool] Missing dependencies.\\n\"");
		fh.WriteLine("        \"Open PowerShell and run:\\n\"");
		fh.WriteLine("        \"    pip install pillow numpy\\n\"");
		fh.WriteLine("        \"Then hit Generate Craters again.\\n\"");
		fh.WriteLine("    )");
		fh.WriteLine("");
		fh.WriteLine("# ===============================================================================");
		fh.WriteLine("# CONTROLS - defaults used when running standalone (no CLI args)");
		fh.WriteLine("# ===============================================================================");
		fh.WriteLine("");
		fh.WriteLine("TOTAL_CRATERS   = 22000   # total craters across the map");
		fh.WriteLine("");
		fh.WriteLine("# Rarity weights - higher number = more common");
		fh.WriteLine("# e.g. 6/1/1 means small appears ~6x more than large");
		fh.WriteLine("SMALL_WEIGHT    = 6");
		fh.WriteLine("MEDIUM_WEIGHT   = 2");
		fh.WriteLine("LARGE_WEIGHT    = 1");
		fh.WriteLine("");
		fh.WriteLine("# Crater radii in world metres");
		fh.WriteLine("SMALL_RADIUS_M  = 2.0");
		fh.WriteLine("MEDIUM_RADIUS_M = 5.0");
		fh.WriteLine("LARGE_RADIUS_M  = 9.0");
		fh.WriteLine("MASK_BLOOM      = 1.0");
		fh.WriteLine("");
		fh.WriteLine("# Crater depths in metres (negative = depression into terrain)");
		fh.WriteLine("SMALL_DEPTH     = -0.33");
		fh.WriteLine("MEDIUM_DEPTH    = -0.48");
		fh.WriteLine("LARGE_DEPTH     = -0.96");
		fh.WriteLine("");
		fh.WriteLine("# Minimum edge-to-edge gap between craters in world metres");
		fh.WriteLine("SMALL_GAP_M     = 3.0");
		fh.WriteLine("MEDIUM_GAP_M    = 5.0");
		fh.WriteLine("LARGE_GAP_M     = 7.0");
		fh.WriteLine("");
		fh.WriteLine("# Random seed - change this to get a different pattern, keep it the same to reproduce");
		fh.WriteLine("SEED            = 42");
		fh.WriteLine("");
		fh.WriteLine("# Paths - export folder is created automatically if it doesn't exist");
		fh.WriteLine("EXPORT_DIR      = r\"C:\\Users\\delan\\Documents\\My Games\\ArmaReforgerWorkbench\\profile\\export\"");
		fh.WriteLine("HEIGHTMAP_IN    = r\"Chasiv Yar_modified.asc\"   # relative to EXPORT_DIR");
		fh.WriteLine("");
		fh.WriteLine("# ===============================================================================");
		fh.WriteLine("# END OF CONTROLS - do not edit below this line");
		fh.WriteLine("# ===============================================================================");
		fh.WriteLine("");
		fh.WriteLine("def _parse_args():");
		fh.WriteLine("    p = argparse.ArgumentParser(description=\"UTM Crater Generator\")");
		fh.WriteLine("    p.add_argument(\"--count\",         type=int,   default=TOTAL_CRATERS)");
		fh.WriteLine("    p.add_argument(\"--seed\",          type=int,   default=SEED)");
		fh.WriteLine("    p.add_argument(\"--small-weight\",  type=float, default=SMALL_WEIGHT)");
		fh.WriteLine("    p.add_argument(\"--medium-weight\", type=float, default=MEDIUM_WEIGHT)");
		fh.WriteLine("    p.add_argument(\"--large-weight\",  type=float, default=LARGE_WEIGHT)");
		fh.WriteLine("    p.add_argument(\"--small-depth\",   type=float, default=SMALL_DEPTH)");
		fh.WriteLine("    p.add_argument(\"--medium-depth\",  type=float, default=MEDIUM_DEPTH)");
		fh.WriteLine("    p.add_argument(\"--large-depth\",   type=float, default=LARGE_DEPTH)");
		fh.WriteLine("    p.add_argument(\"--small-radius\",  type=float, default=SMALL_RADIUS_M)");
		fh.WriteLine("    p.add_argument(\"--medium-radius\", type=float, default=MEDIUM_RADIUS_M)");
		fh.WriteLine("    p.add_argument(\"--large-radius\",  type=float, default=LARGE_RADIUS_M)");
		fh.WriteLine("    p.add_argument(\"--mask-bloom\",    type=float, default=MASK_BLOOM)");
		fh.WriteLine("    return p.parse_args()");
		fh.WriteLine("");
		fh.WriteLine("_args = _parse_args()");
		fh.WriteLine("TOTAL_CRATERS = _args.count");
		fh.WriteLine("SEED          = _args.seed");
		fh.WriteLine("SMALL_WEIGHT  = _args.small_weight");
		fh.WriteLine("MEDIUM_WEIGHT = _args.medium_weight");
		fh.WriteLine("LARGE_WEIGHT  = _args.large_weight");
		fh.WriteLine("SMALL_DEPTH   = _args.small_depth");
		fh.WriteLine("MEDIUM_DEPTH  = _args.medium_depth");
		fh.WriteLine("LARGE_DEPTH   = _args.large_depth");
		fh.WriteLine("SMALL_RADIUS_M  = _args.small_radius");
		fh.WriteLine("MEDIUM_RADIUS_M = _args.medium_radius");
		fh.WriteLine("LARGE_RADIUS_M  = _args.large_radius");
		fh.WriteLine("MASK_BLOOM      = _args.mask_bloom");
		fh.WriteLine("");
		fh.WriteLine("os.makedirs(EXPORT_DIR, exist_ok=True)");
		fh.WriteLine("");
		fh.WriteLine("_asc_files = [f for f in os.listdir(EXPORT_DIR) if f.lower().endswith(\".asc\") and \"cratered\" not in f.lower()]");
		fh.WriteLine("if not _asc_files:");
		fh.WriteLine("    sys.exit(");
		fh.WriteLine("        \"\\n[UTM Crater Tool] No heightmap found.\\n\"");
		fh.WriteLine("        \"In Workbench: Terrain Tool > Export height map > Modified\\n\"");
		fh.WriteLine("        \"Save the .asc file to: \" + EXPORT_DIR + \"\\n\"");
		fh.WriteLine("        \"Then hit Generate Craters again.\\n\"");
		fh.WriteLine("    )");
		fh.WriteLine("if len(_asc_files) > 1:");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Multiple .asc files found, using: {_asc_files[0]}\")");
		fh.WriteLine("HEIGHTMAP_IN = _asc_files[0]");
		fh.WriteLine("print(f\"[UTM Crater Tool] Heightmap: {HEIGHTMAP_IN}\")");
		fh.WriteLine("");
		fh.WriteLine("MASK_SIZE   = 4537      # surface mask resolution (pixels)");
		fh.WriteLine("HM_SIZE     = 2305      # heightmap resolution (cells)");
		fh.WriteLine("MAP_METRES  = 4608.0    # world size in metres");
		fh.WriteLine("CELL_SIZE   = MAP_METRES / (HM_SIZE - 1)   # metres per heightmap cell (~2m)");
		fh.WriteLine("M_PER_PX    = MAP_METRES / MASK_SIZE       # metres per mask pixel (~1.016m)");
		fh.WriteLine("");
		fh.WriteLine("# Convert world metres to mask pixels and heightmap cells");
		fh.WriteLine("def m_to_px(m):   return m / M_PER_PX");
		fh.WriteLine("def m_to_hm(m):   return m / CELL_SIZE");
		fh.WriteLine("");
		fh.WriteLine("CRATER_TYPES = [");
		fh.WriteLine("    {");
		fh.WriteLine("        \"name\":     \"small\",");
		fh.WriteLine("        \"weight\":   SMALL_WEIGHT,");
		fh.WriteLine("        \"r_px\":     m_to_px(SMALL_RADIUS_M),");
		fh.WriteLine("        \"r_hm\":     m_to_hm(SMALL_RADIUS_M),");
		fh.WriteLine("        \"depth\":    SMALL_DEPTH,");
		fh.WriteLine("        \"gap_px\":   m_to_px(SMALL_GAP_M),");
		fh.WriteLine("    },");
		fh.WriteLine("    {");
		fh.WriteLine("        \"name\":     \"medium\",");
		fh.WriteLine("        \"weight\":   MEDIUM_WEIGHT,");
		fh.WriteLine("        \"r_px\":     m_to_px(MEDIUM_RADIUS_M),");
		fh.WriteLine("        \"r_hm\":     m_to_hm(MEDIUM_RADIUS_M),");
		fh.WriteLine("        \"depth\":    MEDIUM_DEPTH,");
		fh.WriteLine("        \"gap_px\":   m_to_px(MEDIUM_GAP_M),");
		fh.WriteLine("    },");
		fh.WriteLine("    {");
		fh.WriteLine("        \"name\":     \"large\",");
		fh.WriteLine("        \"weight\":   LARGE_WEIGHT,");
		fh.WriteLine("        \"r_px\":     m_to_px(LARGE_RADIUS_M),");
		fh.WriteLine("        \"r_hm\":     m_to_hm(LARGE_RADIUS_M),");
		fh.WriteLine("        \"depth\":    LARGE_DEPTH,");
		fh.WriteLine("        \"gap_px\":   m_to_px(LARGE_GAP_M),");
		fh.WriteLine("    },");
		fh.WriteLine("]");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def weighted_choice(types, rng):");
		fh.WriteLine("    total = sum(t[\"weight\"] for t in types)");
		fh.WriteLine("    r = rng.random() * total");
		fh.WriteLine("    acc = 0");
		fh.WriteLine("    for t in types:");
		fh.WriteLine("        acc += t[\"weight\"]");
		fh.WriteLine("        if r <= acc:");
		fh.WriteLine("            return t");
		fh.WriteLine("    return types[-1]");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def place_craters(count, types, rng, size=MASK_SIZE):");
		fh.WriteLine("    max_r   = max(t[\"r_px\"]   for t in types)");
		fh.WriteLine("    max_gap = max(t[\"gap_px\"] for t in types)");
		fh.WriteLine("    cell    = int(max_r * 2 + max_gap) + 4");
		fh.WriteLine("    grid    = {}");
		fh.WriteLine("    placed  = []");
		fh.WriteLine("    max_att = count * 50");
		fh.WriteLine("");
		fh.WriteLine("    def neighbours(cx, cy):");
		fh.WriteLine("        gx, gy = int(cx) // cell, int(cy) // cell");
		fh.WriteLine("        for dx in range(-3, 4):");
		fh.WriteLine("            for dy in range(-3, 4):");
		fh.WriteLine("                k = (gx+dx, gy+dy)");
		fh.WriteLine("                if k in grid:");
		fh.WriteLine("                    yield grid[k]");
		fh.WriteLine("");
		fh.WriteLine("    attempts = 0");
		fh.WriteLine("    while len(placed) < count and attempts < max_att:");
		fh.WriteLine("        attempts += 1");
		fh.WriteLine("        t  = weighted_choice(types, rng)");
		fh.WriteLine("        r  = t[\"r_px\"]");
		fh.WriteLine("        ir = int(math.ceil(r)) + 2");
		fh.WriteLine("        cx = rng.uniform(ir+1, size-ir-2)");
		fh.WriteLine("        cy = rng.uniform(ir+1, size-ir-2)");
		fh.WriteLine("        extra = rng.uniform(0, t[\"gap_px\"] * 3)");
		fh.WriteLine("        req   = r + t[\"gap_px\"] + extra");
		fh.WriteLine("        ok = True");
		fh.WriteLine("        for nb in neighbours(cx, cy):");
		fh.WriteLine("            dist = math.sqrt((cx-nb[\"cx\"])**2 + (cy-nb[\"cy\"])**2)");
		fh.WriteLine("            if dist < req + nb[\"r_px\"] + nb[\"gap_px\"]:");
		fh.WriteLine("                ok = False");
		fh.WriteLine("                break");
		fh.WriteLine("        if ok:");
		fh.WriteLine("            entry = {\"cx\": cx, \"cy\": cy, \"name\": t[\"name\"],");
		fh.WriteLine("                     \"r_px\": r, \"r_hm\": t[\"r_hm\"],");
		fh.WriteLine("                     \"depth\": t[\"depth\"], \"gap_px\": t[\"gap_px\"]}");
		fh.WriteLine("            placed.append(entry)");
		fh.WriteLine("            grid[(int(cx)//cell, int(cy)//cell)] = entry");
		fh.WriteLine("    return placed");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def build_circle_kernel(r_px, rng):");
		fh.WriteLine("    ir   = int(math.ceil(r_px)) + 2");
		fh.WriteLine("    size = 2 * ir + 1");
		fh.WriteLine("    y, x = np.ogrid[-ir:ir+1, -ir:ir+1]");
		fh.WriteLine("    dist = np.sqrt(x*x + y*y).astype(np.float32)");
		fh.WriteLine("    fade_start = max(0.0, r_px - 1.5)");
		fh.WriteLine("    fade_end   = r_px + 1.5");
		fh.WriteLine("    kern = np.zeros((size, size), dtype=np.float32)");
		fh.WriteLine("    kern[dist <= fade_start] = 255.0");
		fh.WriteLine("    rim = (dist > fade_start) & (dist <= fade_end)");
		fh.WriteLine("    t   = (dist[rim] - fade_start) / (fade_end - fade_start)");
		fh.WriteLine("    kern[rim] = 255.0 * 0.5 * (1.0 + np.cos(np.pi * t))");
		fh.WriteLine("    fringe = (dist > fade_end) & (dist <= fade_end + 1.5)");
		fh.WriteLine("    for py, px in np.argwhere(fringe):");
		fh.WriteLine("        if rng.random() < 0.5:");
		fh.WriteLine("            kern[py, px] = rng.randint(1, 12)");
		fh.WriteLine("    return kern");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def render_mask(craters, rng, mask_bloom=1.0, size=MASK_SIZE):");
		fh.WriteLine("    arr     = np.zeros((size, size), dtype=np.float32)");
		fh.WriteLine("    kernels = {}");
		fh.WriteLine("    for c in craters:");
		fh.WriteLine("        rp = round(c[\"r_px\"] * mask_bloom, 4)");
		fh.WriteLine("        if rp not in kernels:");
		fh.WriteLine("            kernels[rp] = build_circle_kernel(c[\"r_px\"] * mask_bloom, rng)");
		fh.WriteLine("    for c in craters:");
		fh.WriteLine("        kern = kernels[round(c[\"r_px\"] * mask_bloom, 4)]");
		fh.WriteLine("        cx, cy = int(round(c[\"cx\"])), int(round(c[\"cy\"]))");
		fh.WriteLine("        ir = kern.shape[0] // 2");
		fh.WriteLine("        ky0 = max(0, ir - cy);           kx0 = max(0, ir - cx)");
		fh.WriteLine("        ky1 = kern.shape[0] - max(0, cy+ir+1-size)");
		fh.WriteLine("        kx1 = kern.shape[1] - max(0, cx+ir+1-size)");
		fh.WriteLine("        ay0 = max(0, cy-ir);             ax0 = max(0, cx-ir)");
		fh.WriteLine("        ay1 = min(size, cy+ir+1);        ax1 = min(size, cx+ir+1)");
		fh.WriteLine("        arr[ay0:ay1, ax0:ax1] = np.maximum(");
		fh.WriteLine("            arr[ay0:ay1, ax0:ax1], kern[ky0:ky1, kx0:kx1])");
		fh.WriteLine("    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8), mode='L')");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def read_asc(path):");
		fh.WriteLine("    header = {}");
		fh.WriteLine("    with open(path, \"r\") as f:");
		fh.WriteLine("        for _ in range(6):");
		fh.WriteLine("            key, val = f.readline().split()");
		fh.WriteLine("            header[key.lower()] = float(val)");
		fh.WriteLine("        ncols = int(header[\"ncols\"])");
		fh.WriteLine("        nrows = int(header[\"nrows\"])");
		fh.WriteLine("        print(f\"[UTM Crater Tool] Loading {nrows}x{ncols} heightmap...\", end=\" \", flush=True)");
		fh.WriteLine("        t = time.time()");
		fh.WriteLine("        data = np.fromstring(f.read(), sep=\" \", dtype=np.float32)");
		fh.WriteLine("    data = data.reshape((nrows, ncols))");
		fh.WriteLine("    print(f\"done in {time.time()-t:.1f}s\")");
		fh.WriteLine("    return header, data");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def write_asc(path, header, data):");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Writing ASC...\", end=\" \", flush=True)");
		fh.WriteLine("    t = time.time()");
		fh.WriteLine("    with open(path, \"w\") as f:");
		fh.WriteLine("        f.write(f\"ncols         {int(header['ncols'])}\\n\")");
		fh.WriteLine("        f.write(f\"nrows         {int(header['nrows'])}\\n\")");
		fh.WriteLine("        f.write(f\"xllcorner     {int(header['xllcorner'])}\\n\")");
		fh.WriteLine("        f.write(f\"yllcorner     {int(header['yllcorner'])}\\n\")");
		fh.WriteLine("        f.write(f\"cellsize      {int(header['cellsize'])}\\n\")");
		fh.WriteLine("        f.write(f\"nodata_value  {int(header['nodata_value'])}\\n\")");
		fh.WriteLine("        for row in data:");
		fh.WriteLine("            f.write(\" \".join(f\"{v:.3f}\" for v in row) + \"\\n\")");
		fh.WriteLine("    print(f\"done in {time.time()-t:.1f}s\")");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def apply_craters_to_hm(hm, craters):");
		fh.WriteLine("    nrows, ncols = hm.shape");
		fh.WriteLine("    scale = HM_SIZE / MASK_SIZE");
		fh.WriteLine("    placed = 0");
		fh.WriteLine("    for c in craters:");
		fh.WriteLine("        hm_col = int(round(c[\"cx\"] * scale))");
		fh.WriteLine("        hm_row = int(round(c[\"cy\"] * scale))");
		fh.WriteLine("        r_hm   = c[\"r_hm\"]");
		fh.WriteLine("        depth  = c[\"depth\"]");
		fh.WriteLine("        ir     = max(1, int(math.ceil(r_hm)))");
		fh.WriteLine("        for dr in range(-ir, ir+1):");
		fh.WriteLine("            for dc in range(-ir, ir+1):");
		fh.WriteLine("                row = hm_row + dr");
		fh.WriteLine("                col = hm_col + dc");
		fh.WriteLine("                if row < 0 or row >= nrows or col < 0 or col >= ncols:");
		fh.WriteLine("                    continue");
		fh.WriteLine("                dist = math.sqrt(dr*dr + dc*dc)");
		fh.WriteLine("                if dist > r_hm:");
		fh.WriteLine("                    continue");
		fh.WriteLine("                t = dist / r_hm");
		fh.WriteLine("                weight = 0.5 * (1.0 + math.cos(math.pi * t))");
		fh.WriteLine("                hm[row, col] += depth * weight");
		fh.WriteLine("        placed += 1");
		fh.WriteLine("        if placed % 2000 == 0:");
		fh.WriteLine("            print(f\"[UTM Crater Tool] {placed}/{len(craters)} craters applied...\")");
		fh.WriteLine("    return placed");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("def main():");
		fh.WriteLine("    hm_path   = os.path.join(EXPORT_DIR, HEIGHTMAP_IN)");
		fh.WriteLine("    mask_path = os.path.join(EXPORT_DIR, \"UTM_CraterMask.png\")");
		fh.WriteLine("    csv_path  = os.path.join(EXPORT_DIR, \"UTM_Craters.csv\")");
		fh.WriteLine("    out_path  = os.path.join(EXPORT_DIR, \"UTM_Heightmap_Cratered.asc\")");
		fh.WriteLine("");
		fh.WriteLine("    rng = random.Random(SEED)");
		fh.WriteLine("");
		fh.WriteLine("    total_w = SMALL_WEIGHT + MEDIUM_WEIGHT + LARGE_WEIGHT");
		fh.WriteLine("    print(\"[UTM Crater Tool] \" + \"=\" * 42)");
		fh.WriteLine("    print(\"[UTM Crater Tool] UTM Crater Generator\")");
		fh.WriteLine("    print(\"[UTM Crater Tool] \" + \"=\" * 42)");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Seed          : {SEED}\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Total craters : {TOTAL_CRATERS}\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Small   r={SMALL_RADIUS_M}m  depth={SMALL_DEPTH}m  weight={SMALL_WEIGHT}  (~{SMALL_WEIGHT/total_w*100:.0f}%)\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Medium  r={MEDIUM_RADIUS_M}m  depth={MEDIUM_DEPTH}m  weight={MEDIUM_WEIGHT}  (~{MEDIUM_WEIGHT/total_w*100:.0f}%)\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Large   r={LARGE_RADIUS_M}m  depth={LARGE_DEPTH}m  weight={LARGE_WEIGHT}  (~{LARGE_WEIGHT/total_w*100:.0f}%)\")");
		fh.WriteLine("    print()");
		fh.WriteLine("");
		fh.WriteLine("    # -- Step 1: Place craters");
		fh.WriteLine("    print(\"[UTM Crater Tool] Step 1/4 - Placing craters...\")");
		fh.WriteLine("    t0 = time.time()");
		fh.WriteLine("    craters = place_craters(TOTAL_CRATERS, CRATER_TYPES, rng)");
		fh.WriteLine("    counts = {t[\"name\"]: sum(1 for c in craters if c[\"name\"]==t[\"name\"]) for t in CRATER_TYPES}");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Placed {len(craters)}/{TOTAL_CRATERS} in {time.time()-t0:.1f}s\")");
		fh.WriteLine("    for name, n in counts.items():");
		fh.WriteLine("        print(f\"[UTM Crater Tool]     {name:8s}: {n}  ({n/len(craters)*100:.1f}%)\")");
		fh.WriteLine("");
		fh.WriteLine("    # -- Step 2: Save CSV");
		fh.WriteLine("    print(\"\\n[UTM Crater Tool] Step 2/4 - Saving CSV...\")");
		fh.WriteLine("    with open(csv_path, \"w\", newline=\"\") as f:");
		fh.WriteLine("        w = csv.writer(f)");
		fh.WriteLine("        w.writerow([\"pixel_x\",\"pixel_y\",\"size_class\",\"radius_px\"])");
		fh.WriteLine("        for c in craters:");
		fh.WriteLine("            w.writerow([f\"{c['cx']:.2f}\", f\"{c['cy']:.2f}\", c[\"name\"], f\"{c['r_px']:.4f}\"])");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Saved {csv_path}\")");
		fh.WriteLine("");
		fh.WriteLine("    # -- Step 3: Render surface mask PNG");
		fh.WriteLine("    print(\"\\n[UTM Crater Tool] Step 3/4 - Rendering surface mask PNG...\")");
		fh.WriteLine("    t1 = time.time()");
		fh.WriteLine("    img = render_mask(craters, rng, mask_bloom=MASK_BLOOM)");
		fh.WriteLine("    img.save(mask_path, \"PNG\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Done in {time.time()-t1:.1f}s\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Saved {mask_path}\")");
		fh.WriteLine("");
		fh.WriteLine("    # -- Step 4: Apply to heightmap");
		fh.WriteLine("    print(\"\\n[UTM Crater Tool] Step 4/4 - Applying craters to heightmap...\")");
		fh.WriteLine("    header, hm = read_asc(hm_path)");
		fh.WriteLine("    t2 = time.time()");
		fh.WriteLine("    placed = apply_craters_to_hm(hm, craters)");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   {placed} craters applied in {time.time()-t2:.1f}s\")");
		fh.WriteLine("    write_asc(out_path, header, hm)");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Saved {out_path}\")");
		fh.WriteLine("");
		fh.WriteLine("    print(\"\\n[UTM Crater Tool] DONE.\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Import these two files into Workbench:\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Heightmap:     {os.path.join(EXPORT_DIR, 'UTM_Heightmap_Cratered.asc')}\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool]   Surface mask:  {os.path.join(EXPORT_DIR, 'UTM_CraterMask.png')}\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Heightmap: Terrain Tool > Import height map > Modified\")");
		fh.WriteLine("    print(f\"[UTM Crater Tool] Surface mask: Terrain Tool > Import surface map > select your dirt layer\")");
		fh.WriteLine("");
		fh.WriteLine("");
		fh.WriteLine("if __name__ == \"__main__\":");
		fh.WriteLine("    main()");

		fh.Close();
		Print("[UTM Crater Tool] Written UTM_CraterGen.py to " + scriptPath, LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Generate Craters")]
	protected void GenerateCraters()
	{
		string profilePath;
		Workbench.GetAbsolutePath("$profile:", profilePath);
		string exportPath  = profilePath + "/export";
		string scriptPath  = exportPath  + "/UTM_CraterGen.py";

		// 1. Ensure export folder exists
		FileIO.MakeDirectory(exportPath);

		// 2. Self-extract UTM_CraterGen.py if absent
		WriteGeneratorScript();

		// 3. Check Python is available
		ProcessHandle pyCheck = Workbench.RunProcess("python --version");
		if (!pyCheck)
		{
			Print("[UTM Crater Tool] ERROR: Python not found. Download from https://python.org — install it, then hit Generate Craters again.", LogLevel.ERROR);
			return;
		}
		Workbench.WaitProcess(pyCheck, 5000);
		Workbench.FinishProcess(pyCheck);

		// 4. Auto-install dependencies
		Print("[UTM Crater Tool] Checking dependencies...", LogLevel.NORMAL);
		ProcessHandle pipHandle = Workbench.RunProcess("python -m pip install pillow numpy --quiet");
		if (pipHandle)
		{
			Workbench.WaitProcess(pipHandle, 60000);
			Workbench.FinishProcess(pipHandle);
		}

		// 5. Check script exists
		if (!FileIO.FileExists(scriptPath))
		{
			Print("[UTM Crater Tool] ERROR: UTM_CraterGen.py not found. Place it in your Workbench export folder then try again.", LogLevel.ERROR);
			Print("[UTM Crater Tool] Expected path: " + scriptPath, LogLevel.ERROR);
			return;
		}

		// 6. Launch
		string cmd = "python \"" + scriptPath + "\"";
		cmd += " --count "        + m_iCount;
		cmd += " --seed "         + m_iSeed;
		cmd += " --small-weight " + m_fSmallWeight;
		cmd += " --medium-weight "+ m_fMediumWeight;
		cmd += " --large-weight " + m_fLargeWeight;
		cmd += " --small-depth "  + m_fSmallDepth;
		cmd += " --medium-depth " + m_fMediumDepth;
		cmd += " --large-depth "  + m_fLargeDepth;
		cmd += " --small-radius " + m_fSmallRadius;
		cmd += " --medium-radius "+ m_fMediumRadius;
		cmd += " --large-radius " + m_fLargeRadius;
		cmd += " --mask-bloom "   + m_fMaskBloom;

		Print("[UTM Crater Tool] Launching... watch this log for progress.", LogLevel.NORMAL);
		Print("[UTM Crater Tool] " + cmd, LogLevel.NORMAL);
		Workbench.RunProcess(cmd);
	}
}

#endif // WORKBENCH
