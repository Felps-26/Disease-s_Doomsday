#!/usr/bin/env python3
# bake_collision_mask.py
# Gera Assets/Maps/map_body_mask.h: uma occupancy grid compacta (bitmask) da
# silhueta de Assets/Sprites/Map/corpo.png, usada como FONTE AUTORITATIVA de
# colisao do mapa corporal (Fase 1). Processada UMA vez aqui (offline); o jogo
# apenas decodifica o bitmask e gera um distance field no load. Reexecute este
# script se corpo.png mudar:  python3 tools/bake_collision_mask.py
import os, sys, numpy as np
from PIL import Image
from scipy import ndimage

HERE=os.path.dirname(os.path.abspath(__file__))
ROOT=os.path.dirname(HERE)
IMG=os.path.join(ROOT,"Assets","Sprites","Map","corpo.png")
OUT=os.path.join(ROOT,"Assets","Maps","map_body_mask.h")
GN=320               # resolucao da grade (celula = 4000/320 = 12.5 px de mundo)
THR=10               # luminancia minima da silhueta (inclui as linhas escuras decorativas)
CLOSE=1              # fechamento morfologico: liga cabeca/tronco/membros nas juntas

def largest(m):
    lab,n=ndimage.label(m,structure=np.ones((3,3)))
    if n==0: return m
    s=ndimage.sum(np.ones_like(lab),lab,range(1,n+1)); return lab==1+int(np.argmax(s))

im=np.array(Image.open(IMG).convert("RGB")).astype(np.float32)
lum=0.299*im[...,0]+0.587*im[...,1]+0.114*im[...,2]
IMGN=im.shape[0]
ix=np.clip(((np.arange(GN)+0.5)*IMGN/GN).astype(int),0,IMGN-1)
body=lum[np.ix_(ix,ix)]>THR
body=largest(body)
if CLOSE>0:
    body=largest(ndimage.binary_closing(body,structure=ndimage.generate_binary_structure(2,1),iterations=CLOSE))

# valida conectividade do espaco jogavel (erosao pelo raio do jogador = 20px)
cell=4000.0/GN
dist=ndimage.distance_transform_edt(body)*cell
passable=dist>=20.0
lab,_=ndimage.label(passable,structure=[[0,1,0],[1,1,1],[0,1,0]])
sx=int(2000//40);
# metrica STEP=40 igual ao teste de aceitacao
STEP=40;COLS=100; gi=np.clip(((np.arange(COLS)*STEP+STEP*0.5)/cell).astype(int),0,GN-1)
D=dist[np.ix_(gi,gi)]; pass40=D>=20.0
lab2,_=ndimage.label(pass40,structure=[[0,1,0],[1,1,1],[0,1,0]]); start=lab2[int(1780//40),int(2000//40)]
disc=int(pass40.sum()-(lab2==start).sum())
print(f"GN={GN} thr={THR} close={CLOSE}  bodyCells={int(body.sum())}  STEP40 disconnected={disc}")
assert disc==0, "ERRO: espaco jogavel desconectado; ajuste THR/CLOSE."

# empacota row-major, LSB-first
flat=body.flatten(order='C').astype(np.uint8)
packed=np.packbits(flat,bitorder='little')
with open(OUT,"w") as f:
    f.write("// map_body_mask.h - GERADO por tools/bake_collision_mask.py (NAO editar a mao)\n")
    f.write("// Occupancy grid autoritativa da silhueta de Assets/Sprites/Map/corpo.png.\n")
    f.write("// Bit (gy*GN+gx), LSB-first. 1=dentro do corpo (jogavel), 0=void externo.\n")
    f.write("#ifndef MAP_BODY_MASK_H\n#define MAP_BODY_MASK_H\n\n")
    f.write(f"#define MAPBODY_MASK_GN {GN}\n")
    f.write(f"#define MAPBODY_MASK_BYTES {len(packed)}\n\n")
    f.write("static const unsigned char MAPBODY_MASK_BITS[MAPBODY_MASK_BYTES] = {\n")
    for i in range(0,len(packed),20):
        f.write("  "+",".join(str(int(b)) for b in packed[i:i+20])+",\n")
    f.write("};\n\n#endif // MAP_BODY_MASK_H\n")
print("escrito:",OUT,"  bytes:",len(packed))
