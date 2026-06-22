#!/usr/bin/env python3
# extract_organisms.py — ferramenta de DESENVOLVIMENTO (não usada em runtime)
# Extrai os 18 organismos (vírus/bactérias) de Assets/Sprites/UI/virus_bacterias.png
# como PNGs transparentes, removendo SÓ o fundo branco EXTERNO (flood fill a partir
# das bordas), preservando detalhes brancos internos, centro escuro e o glow.
#
# Dependências: Pillow, numpy, scipy.
# Uso:  python3 tools/extract_organisms.py
import os, sys
import numpy as np
from PIL import Image
from scipy import ndimage

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC  = os.path.join(ROOT, "Assets/Sprites/UI/virus_bacterias.png")
OUT  = os.path.join(ROOT, "Assets/Sprites/UI/Menu/Organisms")
HDR  = os.path.join(ROOT, "Assets/@models/menu_organisms.h")
PREVIEW = os.path.join(ROOT, "tools/_organisms_preview.png")
EXPECTED = 18
os.makedirs(OUT, exist_ok=True)

if not os.path.exists(SRC):
    print("ERRO: imagem original ausente:", SRC); sys.exit(1)

im = Image.open(SRC).convert("RGB")
rgb = np.array(im).astype(np.int32)
H, W = rgb.shape[:2]
mn = rgb.min(axis=2); mx = rgb.max(axis=2)

# 1) Candidato a "branco" = claro e pouco saturado (não pega o glow colorido).
white = (mn > 210) & ((mx - mn) < 28)

# 2) Fundo EXTERNO = regiões brancas conectadas às bordas (flood fill).
wl, wn = ndimage.label(white)
border = set(np.unique(np.concatenate([wl[0, :], wl[-1, :], wl[:, 0], wl[:, -1]])))
border.discard(0)
bg = np.isin(wl, list(border))     # branco externo
fg = ~bg                            # organismo (inclui brancos internos)

# 3) Rotula organismos (dilata p/ unir glow/espículas do MESMO organismo).
fgd = ndimage.binary_dilation(fg, iterations=2)
lbl, n = ndimage.label(fgd)
comps = []
for i in range(1, n + 1):
    ys, xs = np.where(lbl == i)
    area = len(xs)
    if area < 800:            # descarta ruído/pontos soltos
        continue
    comps.append((i, xs.min(), ys.min(), xs.max(), ys.max(), area))
# ordena por linha depois coluna (leitura natural)
comps.sort(key=lambda c: (round((c[2]) / 120.0), c[1]))

print(f"Organismos detectados: {len(comps)} (esperado {EXPECTED})")

# Alpha suave global (feather ~1px) a partir do fg.
fgf = ndimage.gaussian_filter(fg.astype(np.float32), 0.8)
alpha_full = np.clip((fgf - 0.25) / 0.55, 0.0, 1.0)

def classify(w, h):
    ar = max(w, h) / max(1.0, min(w, h))
    return "bacteria" if ar >= 1.55 else "virus"

results = []
for idx, (lab, x0, y0, x1, y1, area) in enumerate(comps):
    pad = 10
    cx0 = max(0, x0 - pad); cy0 = max(0, y0 - pad)
    cx1 = min(W - 1, x1 + pad); cy1 = min(H - 1, y1 + pad)
    # máscara só deste organismo (evita vizinhos): label dilatado == lab
    region = (lbl[cy0:cy1+1, cx0:cx1+1] == lab)
    a = alpha_full[cy0:cy1+1, cx0:cx1+1] * region
    cr = rgb[cy0:cy1+1, cx0:cx1+1, :3]
    # recorta de novo ao conteúdo real (com alpha) + padding garantido
    ys, xs = np.where(a > 0.04)
    if len(xs) == 0:
        continue
    bx0 = max(0, xs.min() - 6); by0 = max(0, ys.min() - 6)
    bx1 = min(a.shape[1]-1, xs.max() + 6); by1 = min(a.shape[0]-1, ys.max() + 6)
    cr = cr[by0:by1+1, bx0:bx1+1]; a = a[by0:by1+1, bx0:bx1+1]
    out = np.zeros((a.shape[0], a.shape[1], 4), np.uint8)
    out[:, :, :3] = np.clip(cr, 0, 255).astype(np.uint8)
    out[:, :, 3] = np.clip(a * 255.0, 0, 255).astype(np.uint8)
    spr = Image.fromarray(out, "RGBA")
    # cor predominante (entre pixels coloridos, ignorando branco/escuro)
    col = cr[(a > 0.5)]
    colored = col[(col.max(axis=1) - col.min(axis=1)) > 30] if len(col) else np.array([[0,0,0]])
    mc = colored.mean(axis=0) if len(colored) else np.array([0,0,0])
    hue = "green" if (mc[1] >= mc[0] and mc[1] >= mc[2]) else ("blue" if mc[2] >= mc[0] else "purple")
    typ = classify(spr.width, spr.height)
    fn = f"organism_{idx:02d}.png"
    spr.save(os.path.join(OUT, fn))
    results.append((fn, spr.width, spr.height, typ, hue))
    print(f"  + {fn}  bbox=({x0},{y0})-({x1},{y1}) size={spr.width}x{spr.height} {typ}/{hue}")

if len(results) != EXPECTED:
    print(f"!!! ATENCAO: extraídos {len(results)} != {EXPECTED}. Ajuste o limiar/area.")
else:
    print("OK: 18 organismos extraídos.")

# Header C gerado (catálogo de organismos do menu).
with open(HDR, "w") as f:
    f.write("// GERADO por tools/extract_organisms.py — NÃO editar à mão.\n")
    f.write("#ifndef MENU_ORGANISMS_H\n#define MENU_ORGANISMS_H\n\n")
    f.write(f"#define MENU_ORGANISM_COUNT {len(results)}\n\n")
    f.write("typedef struct MenuOrganismDef { const char *path; int w, h; const char *type; const char *color; } MenuOrganismDef;\n\n")
    f.write("static const MenuOrganismDef MENU_ORGANISMS[MENU_ORGANISM_COUNT] = {\n")
    for (fn, w, h, typ, hue) in results:
        f.write(f'    {{ "Assets/Sprites/UI/Menu/Organisms/{fn}", {w}, {h}, "{typ}", "{hue}" }},\n')
    f.write("};\n\n#endif\n")
print("header:", os.path.relpath(HDR, ROOT))

# Imagem de conferência (xadrez).
cols = 6
rows = (len(results) + cols - 1) // cols
cell = 220
def checker(w, h, s=14):
    img = Image.new("RGBA", (w, h)); px = img.load()
    for y in range(h):
        for x in range(w):
            px[x, y] = (50, 54, 64, 255) if (x//s + y//s) % 2 == 0 else (90, 96, 110, 255)
    return img
sheet = checker(cols*cell, rows*cell)
for i, (fn, w, h, typ, hue) in enumerate(results):
    s = Image.open(os.path.join(OUT, fn)).convert("RGBA")
    sc = min((cell-30)/s.width, (cell-44)/s.height)
    s2 = s.resize((max(1,int(s.width*sc)), max(1,int(s.height*sc))))
    cx = (i % cols)*cell; cy = (i//cols)*cell
    sheet.alpha_composite(s2, (cx+(cell-s2.width)//2, cy+30))
from PIL import ImageDraw
d = ImageDraw.Draw(sheet)
for i, (fn, w, h, typ, hue) in enumerate(results):
    cx = (i % cols)*cell; cy = (i//cols)*cell
    d.text((cx+6, cy+6), f"{i:02d} {typ[:4]}/{hue}", fill=(255,255,0,255))
sheet.convert("RGB").save(PREVIEW)
print("preview:", os.path.relpath(PREVIEW, ROOT))
