#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time; // Time variable to animate the effect

// Output fragment color
out vec4 finalColor;

void main()
{
    // Retrieve base texture color
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Distorção suave como se fosse visto por uma lente de microscópio.
    // Amplitude reduzida (0.1 -> 0.025) para evitar o "tremor" constante da tela
    // que dificultava mirar e cansava a vista.
    vec2 center = vec2(0.5, 0.5);
    vec2 dist = fragTexCoord - center;
    float len = length(dist);
    vec2 uv = fragTexCoord + (dist * len * 0.025 * sin(time * 2.0));

    // Recalcula o texel com o UV distorcido
    texelColor = texture(texture0, uv);

    // Efeito de vinheta (escurecer as bordas). Mantém piso de ~55% de brilho
    // para que inimigos perto das bordas continuem visíveis.
    float vignette = mix(0.55, 1.0, smoothstep(0.85, 0.25, len));
    
    // Scanlines sutis (Estilo CRT / Microscópio Eletrônico)
    float scanline = sin(uv.y * 800.0) * 0.04;
    
    // Tonalidade biológica (leve desvio pro verde/ciano)
    texelColor.r *= 0.9;
    texelColor.g *= 1.1;
    texelColor.b *= 1.05;

    // Aplica vinheta e scanline
    texelColor.rgb -= vec3(scanline);
    texelColor.rgb *= vignette;
    
    finalColor = texelColor * colDiffuse * fragColor;
}
