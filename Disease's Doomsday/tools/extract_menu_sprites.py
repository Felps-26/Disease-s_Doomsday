#!/usr/bin/env python3
# extract_menu_sprites.py  —  ferramenta de DESENVOLVIMENTO (não usada em runtime)
# Recorta sprites transparentes da arte original do menu
# (Assets/Sprites/UI/menu_reference.png), removendo apenas o fundo azul-marinho/
# preto e preservando contornos, cores e glow (alpha suave por luminância).
# Cada elemento é isolado pelo seu COMPONENTE CONEXO (sem contaminar vizinhos,
# sem retângulo escuro em volta). Também fatia o título em glifos para animação.
#
# Uso:  python3 tools/extract_menu_sprites.py
import os
import numpy as np
from PIL import Image
from scipy import ndimage

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC  = os.path.join(ROOT, "Assets/Sprites/UI/menu_reference.png")
OUT  = os.path.join(ROOT, "Assets/Sprites/UI/Menu")
GLY  = os.path.join(OUT, "glyphs")
os.makedirs(OUT, exist_ok=True)
os.makedirs(GLY, exist_ok=True)

im = Image.open(SRC).convert("RGBA")
rgb = np.array(im).astype(np.float32)
luma = 0.299 * rgb[:, :, 0] + 0.587 * rgb[:, :, 1] + 0.114 * rgb[:, :, 2]

# Alpha suave (chroma/luma key): fundo navy (~24) -> 0 ; neon/glow -> 0..1.
LO, HI = 30.0, 140.0
soft = np.clip((luma - LO) / (HI - LO), 0.0, 1.0)

# Binário p/ rotular componentes; dilata p/ unir o glow do MESMO elemento.
binary = luma > 55
binm = ndimage.binary_dilation(binary, iterations=4)
lbl, n = ndimage.label(binm)

def labels_at(points):
    s = set()
    for (x, y) in points:
        v = int(lbl[y, x])
        if v != 0:
            s.add(v)
    return s

def save_element(name, seeds, glow_iters=6, hue_shift=None, mirror=False):
    sel = labels_at(seeds)
    if not sel:
        print("  ! sem componente para", name, seeds); return None
    region = np.isin(lbl, list(sel))
    region = ndimage.binary_dilation(region, iterations=glow_iters)  # inclui glow proprio
    a = (soft * region).astype(np.float32)
    ys, xs = np.where(a > 0.02)
    if len(xs) == 0:
        print("  ! vazio", name); return None
    pad = 6
    x0 = max(0, xs.min() - pad); x1 = min(im.width - 1, xs.max() + pad)
    y0 = max(0, ys.min() - pad); y1 = min(im.height - 1, ys.max() + pad)
    crop_rgb = rgb[y0:y1+1, x0:x1+1, :3].copy()
    crop_a = a[y0:y1+1, x0:x1+1]
    out = np.zeros((crop_a.shape[0], crop_a.shape[1], 4), np.uint8)
    out[:, :, :3] = np.clip(crop_rgb, 0, 255).astype(np.uint8)
    out[:, :, 3] = np.clip(crop_a * 255.0, 0, 255).astype(np.uint8)
    spr = Image.fromarray(out, "RGBA")
    if mirror:
        spr = spr.transpose(Image.FLIP_LEFT_RIGHT)
    if hue_shift is not None:
        import colorsys
        arr = np.array(spr).astype(np.float32)
        r, g, b = arr[:, :, 0]/255, arr[:, :, 1]/255, arr[:, :, 2]/255
        mx = np.maximum(np.maximum(r, g), b); mn = np.minimum(np.minimum(r, g), b)
        # leve variação: aqui só escala canais (variação moderada de cor)
        arr[:, :, 0] = np.clip(arr[:, :, 0]*hue_shift[0], 0, 255)
        arr[:, :, 1] = np.clip(arr[:, :, 1]*hue_shift[1], 0, 255)
        arr[:, :, 2] = np.clip(arr[:, :, 2]*hue_shift[2], 0, 255)
        spr = Image.fromarray(arr.astype(np.uint8), "RGBA")
    path = os.path.join(OUT, name + ".png")
    spr.save(path)
    print(f"  + {name}.png  {spr.size}")
    return spr

print("Extraindo elementos...")
# Seeds (x,y) dentro de cada elemento bem isolado (não cortado pelas bordas).
save_element("syringe_cyan",       [(127, 500)], glow_iters=5)
save_element("biohazard_red",      [(856, 511)], glow_iters=5)
save_element("bacteria_green_01",  [(55, 376), (91, 414)], glow_iters=6)
save_element("bacteria_green_02",  [(55, 376), (91, 414)], glow_iters=6, mirror=True, hue_shift=(0.8,1.05,0.85))
save_element("virus_purple_01",    [(147, 361)], glow_iters=6)
save_element("virus_cyan_01",      [(874, 369)], glow_iters=6)
save_element("virus_green_01",     [(944, 620)], glow_iters=6)
save_element("virus_cyan_02",      [(801, 665)], glow_iters=6)
save_element("bacteria_purple_01", [(872, 659)], glow_iters=6)

# ---------------------------------------------------------------------------
# TÍTULO: recorta a região verde e fatia em GLIFOS (2 linhas) p/ animação.
# Gera glyphs/gNN.png + um header C com a posição original de cada glifo.
# ---------------------------------------------------------------------------
print("Fatiando o titulo em glifos...")
TBOX = (238, 389, 748, 569)  # bbox do titulo (comp #0)
tx0, ty0, tx1, ty1 = TBOX
# alpha do titulo (verde) isolado por cor (G dominante).
g_dom = (rgb[:, :, 1] > 90) & (rgb[:, :, 1] > rgb[:, :, 2] + 25) & (rgb[:, :, 0] < rgb[:, :, 1] + 30)
talpha = (soft * g_dom)[ty0:ty1+1, tx0:tx1+1]
# Para detectar os GAPS entre letras usamos só os NÚCLEOS brilhantes (sem glow),
# senão o brilho une letras vizinhas e o corte falha.
core = ((luma > 112) & g_dom).astype(np.float32)
tcore = core[ty0:ty1+1, tx0:tx1+1]
H, W = talpha.shape
colsum = (tcore > 0.5).sum(axis=0)
rowsum = (tcore > 0.5).sum(axis=1)
# separa as 2 linhas pela maior faixa de linhas vazias
empty_rows = np.where(rowsum < 2)[0]
split = H // 2
if len(empty_rows):
    # maior gap contíguo de linhas vazias perto do meio
    best = split
    runs = np.split(empty_rows, np.where(np.diff(empty_rows) > 1)[0]+1)
    bestlen = 0
    for run in runs:
        mid = (run[0]+run[-1])//2
        if len(run) > bestlen and 0.25*H < mid < 0.75*H:
            bestlen = len(run); best = mid
    split = best
lines = [(0, split), (split, H)]

glyphs = []  # (filename, x, y, w, h, line)
gi = 0
for li, (ra, rb) in enumerate(lines):
    sub = talpha[ra:rb, :]
    csub = tcore[ra:rb, :]
    cs = (csub > 0.5).sum(axis=0)
    # acha colunas de glifos (segmentos não vazios separados por gaps de núcleo)
    inside = cs > 0
    x = 0
    while x < W:
        if not inside[x]:
            x += 1; continue
        xs = x
        while x < W and inside[x]:
            x += 1
        xe = x  # [xs,xe)
        if xe - xs < 4:
            continue
        # bbox vertical do glifo
        gsub = sub[:, xs:xe]
        yr = np.where((gsub > 0.05).any(axis=1))[0]
        if len(yr) == 0:
            continue
        ya, yb = yr.min(), yr.max()
        gx0 = tx0 + xs; gy0 = ty0 + ra + ya
        gw = xe - xs; gh = yb - ya + 1
        # recorta do original com alpha verde
        cr = rgb[gy0:gy0+gh, gx0:gx0+gw, :3]
        ca = (soft * g_dom)[gy0:gy0+gh, gx0:gx0+gw]
        o = np.zeros((gh, gw, 4), np.uint8)
        o[:, :, :3] = np.clip(cr, 0, 255).astype(np.uint8)
        o[:, :, 3] = np.clip(ca*255, 0, 255).astype(np.uint8)
        fn = f"g{gi:02d}.png"
        Image.fromarray(o, "RGBA").save(os.path.join(GLY, fn))
        glyphs.append((fn, gx0 - tx0, gy0 - ty0, gw, gh, li))
        gi += 1

print(f"  glifos: {len(glyphs)} (linha0 + linha1), split row={split}")

# Header C gerado (layout dos glifos relativo ao canto do titulo).
hdr = os.path.join(ROOT, "Assets/@models/menu_title_glyphs.h")
with open(hdr, "w") as f:
    f.write("// GERADO por tools/extract_menu_sprites.py — NÃO editar à mão.\n")
    f.write("// Layout dos glifos do título (posição original relativa ao canto\n")
    f.write("// superior-esquerdo do título). Usado para montar o título letra a letra.\n")
    f.write("#ifndef MENU_TITLE_GLYPHS_H\n#define MENU_TITLE_GLYPHS_H\n\n")
    f.write(f"#define MENU_TITLE_GLYPH_COUNT {len(glyphs)}\n")
    f.write(f"#define MENU_TITLE_W {tx1-tx0+1}\n#define MENU_TITLE_H {ty1-ty0+1}\n\n")
    f.write("typedef struct MenuGlyphDef { const char *path; int x, y, w, h, line; } MenuGlyphDef;\n\n")
    f.write("static const MenuGlyphDef MENU_TITLE_GLYPHS[MENU_TITLE_GLYPH_COUNT] = {\n")
    for (fn, x, y, w, h, li) in glyphs:
        f.write(f'    {{ "Assets/Sprites/UI/Menu/glyphs/{fn}", {x}, {y}, {w}, {h}, {li} }},\n')
    f.write("};\n\n#endif\n")
print("  header:", os.path.relpath(hdr, ROOT))
print("OK")
