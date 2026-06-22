#!/usr/bin/env python3
# bake_collision_mask.py — ALTERNATIVA em Python ao baker canônico em C.
#
# O caminho REPRODUZÍVEL e oficial é `make collision-mask` (tools/bake_collision_mask.c),
# que NÃO depende de pip. Este script produz a MESMA máscara para quem tem
# numpy/PIL/scipy instalados. Reexecute se corpo.png ou a transformação mudar.
#
# Gera Assets/Maps/map_body_mask.h: occupancy grid (bitmask) da silhueta de
# Assets/Sprites/Map/corpo.png — FONTE AUTORITATIVA da colisão do corpo.
#
# Pontos-chave (idênticos ao baker C):
#  - Transformação imagem->mundo IGUAL à do renderer (DrawMapBody): aspecto
#    PRESERVADO, centrada, altura = MAP_HEIGHT*MAPBODY_IMG_SCALE. As constantes
#    são LIDAS de Assets/Maps/map_body.h e include/game.h (fonte única).
#  - corpo.png é RGB (sem alpha): usa LUMINÂNCIA (pernas escuras ~12-21, fundo ~6-9).
#  - Fechamento morfológico liga membros ao tronco; buracos internos (órgãos) são
#    PREENCHIDOS -> órgãos NÃO bloqueiam. Mantém o maior componente conexo.
import os, re, sys
import numpy as np
from PIL import Image
from scipy import ndimage

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
IMG  = os.path.join(ROOT, "Assets", "Sprites", "Map", "corpo.png")
OUT  = os.path.join(ROOT, "Assets", "Maps", "map_body_mask.h")

def define(path, name, cast=float):
    txt = open(path).read()
    m = re.search(r"#define\s+%s\s+([0-9.]+)f?" % re.escape(name), txt)
    if not m: raise SystemExit("constante %s nao encontrada em %s" % (name, path))
    return cast(m.group(1))

GAMEH = os.path.join(ROOT, "include", "game.h")
MBH   = os.path.join(ROOT, "Assets", "Maps", "map_body.h")
MAPW  = define(GAMEH, "MAP_WIDTH", int);   MAPH = define(GAMEH, "MAP_HEIGHT", int)
SCALE = define(MBH, "MAPBODY_IMG_SCALE");  DX = define(MBH, "MAPBODY_IMG_DX"); DY = define(MBH, "MAPBODY_IMG_DY")
GN    = define(MBH, "MAPBODY_BAKE_GN", int)
LUMA  = define(MBH, "MAPBODY_BAKE_LUMA", int)
CLOSE = define(MBH, "MAPBODY_BAKE_CLOSE", int)
PLAYER_R = define(MBH, "BODY_PLAYER_RADIUS")

def largest(m):
    lab, n = ndimage.label(m, structure=np.ones((3, 3)))
    if n == 0: return m
    s = ndimage.sum(np.ones_like(lab), lab, range(1, n + 1))
    return lab == 1 + int(np.argmax(s))

im = np.array(Image.open(IMG).convert("RGB")).astype(np.float32)
ih, iw = im.shape[:2]
lum = 0.299 * im[..., 0] + 0.587 * im[..., 1] + 0.114 * im[..., 2]

# Transformação imagem -> mundo (igual ao renderer).
aspect = iw / ih
wh = MAPH * SCALE; ww = wh * aspect
cx = MAPW * 0.5 + DX; cy = MAPH * 0.5 + DY
tlx = cx - ww * 0.5; tly = cy - wh * 0.5
cell = MAPW / GN

gxw = (np.arange(GN) + 0.5) * cell
gyw = (np.arange(GN) + 0.5) * cell
U = (gxw - tlx) / ww            # por coluna
V = (gyw - tly) / wh            # por linha
px = np.clip((U * iw).astype(int), 0, iw - 1)
py = np.clip((V * ih).astype(int), 0, ih - 1)
inx = (U >= 0) & (U < 1); iny = (V >= 0) & (V < 1)
fg = (lum[np.ix_(py, px)] > LUMA) & iny[:, None] & inx[None, :]

if CLOSE > 0:
    fg = ndimage.binary_closing(fg, structure=np.ones((3, 3)), iterations=CLOSE)
body = ndimage.binary_fill_holes(fg)   # órgãos/buracos internos viram corpo
body = largest(body)

# Validação: área caminhável (erosão pelo raio do jogador) é um único componente.
dist = ndimage.distance_transform_edt(body) * cell
passable = dist >= PLAYER_R
lab, _ = ndimage.label(passable, structure=[[0, 1, 0], [1, 1, 1], [0, 1, 0]])
sx, sy = np.unravel_index(int(np.argmax(dist)), dist.shape)  # centro seguro = folga máxima
start = lab[sx, sy]
disc = int(passable.sum() - (lab == start).sum()) if start > 0 else int(passable.sum())
print("GN=%d thr=%d close=%d bodyCells=%d desconectados=%d maxClear=%.0fpx" %
      (GN, LUMA, CLOSE, int(body.sum()), disc, float(dist.max())))
assert disc == 0, "ERRO: area caminhavel desconectada; ajuste threshold/transform."

flat = body.flatten(order="C").astype(np.uint8)
packed = np.packbits(flat, bitorder="little")
with open(OUT, "w") as f:
    f.write("// map_body_mask.h - GERADO pelo baker de colisao (make collision-mask). NAO editar a mao.\n")
    f.write("// Origem    : Assets/Sprites/Map/corpo.png (%dx%d, RGB/luminancia)\n" % (iw, ih))
    f.write("// Mundo     : %dx%d   Grade: %dx%d (celula %.3f px de mundo)\n" % (MAPW, MAPH, GN, GN, cell))
    f.write("// Transform : aspecto preservado, centrado; altura=MAP_HEIGHT*%.2f, dx=%.1f, dy=%.1f (ver map_body.h)\n" % (SCALE, DX, DY))
    f.write("// Criterio  : luminancia>%d ; buracos internos preenchidos ; maior componente conexo\n" % LUMA)
    f.write("// Comando   : make collision-mask  (ou python3 tools/bake_collision_mask.py)\n")
    f.write("// Bit (gy*GN+gx), LSB-first. 1=dentro do corpo (jogavel), 0=void externo.\n")
    f.write("#ifndef MAP_BODY_MASK_H\n#define MAP_BODY_MASK_H\n\n")
    f.write("#define MAPBODY_MASK_GN %d\n" % GN)
    f.write("#define MAPBODY_MASK_BYTES %d\n\n" % len(packed))
    f.write("static const unsigned char MAPBODY_MASK_BITS[MAPBODY_MASK_BYTES] = {\n")
    for i in range(0, len(packed), 20):
        f.write("  " + ",".join(str(int(b)) for b in packed[i:i + 20]) + ",\n")
    f.write("};\n\n#endif // MAP_BODY_MASK_H\n")
print("escrito:", OUT, " bytes:", len(packed))
