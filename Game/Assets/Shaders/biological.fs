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
    
    // Distorção suave como se fosse visto por uma lente de microscópio
    vec2 center = vec2(0.5, 0.5);
    vec2 dist = fragTexCoord - center;
    float len = length(dist);
    vec2 uv = fragTexCoord + (dist * len * 0.1 * sin(time * 2.0));
    
    // Recalcula o texel com o UV distorcido
    texelColor = texture(texture0, uv);
    
    // Efeito de vinheta (escurecer as bordas)
    float vignette = smoothstep(0.8, 0.3, len);
    
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
