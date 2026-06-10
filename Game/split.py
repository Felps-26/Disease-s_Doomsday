import re

with open('src/telas/telas.c', 'r', encoding='utf-8') as f:
    text = f.read()

funcs = [
    'UpdateBtnState',
    'UpdateButtonsMenu',
    'UpdateButtonsControles',
    'UpdateButtonsPause',
    'UpdateButtonsGameOver',
    'UpdateButtonsVitoria',
    'UpdateButtonsSkins',
    'UpdateButtonsSaveSelect',
    'UpdateButtonsLoadSelect',
    'UpdateButtonsSettings'
]

# We need to also expose skinsButtons, settingsButtons etc from static
text = text.replace('static UIButton skinsButtons', 'UIButton skinsButtons')
text = text.replace('static UIButton skinsReturnBtn', 'UIButton skinsReturnBtn')
text = text.replace('static UIButton settingsButtons', 'UIButton settingsButtons')
text = text.replace('static UIButton settingsReturnBtn', 'UIButton settingsReturnBtn')

input_controller_code = """#include "../../include/input_controller.h"
#include "../../include/telas.h"
#include "../../include/gameplay.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

extern UIButton menuButtons[6];
extern UIButton pauseButtons[5];
extern UIButton controlsButton;
extern UIButton gameOverButtons[2];
extern UIButton victoryButtons[2];
extern UIButton skinsButtons[6];
extern UIButton skinsReturnBtn;
extern UIButton settingsButtons[4];
extern UIButton settingsReturnBtn;

"""

for func in funcs:
    match = re.search(r'(?:static\s+)?(?:void|bool|int)\s+' + func + r'\s*\([^)]*\)\s*\{', text)
    if not match:
        continue
    start = match.start()
    
    brace_count = 0
    end = -1
    for i in range(start, len(text)):
        if text[i] == '{':
            brace_count += 1
        elif text[i] == '}':
            brace_count -= 1
            if brace_count == 0:
                end = i + 1
                break
    
    if end != -1:
        func_code = text[start:end]
        # Remove static from function definition if it was static
        func_code = func_code.replace("static void", "void")
        input_controller_code += func_code + '\n\n'
        text = text[:start] + text[end:]

with open('src/gameplay/input_controller.c', 'w', encoding='utf-8') as f:
    f.write(input_controller_code)

with open('src/telas/telas.c', 'w', encoding='utf-8') as f:
    f.write(text)
