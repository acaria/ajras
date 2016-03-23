#ifdef GL_ES
precision highp float;
#endif

const int MAX_SPOT_LIGHTS = 12;

varying vec2        v_texCoord;

//input texture
uniform vec2        u_contentSize;
uniform vec2        u_spritePosInSheet;
uniform vec2        u_spriteSizeRelToSheet;
uniform vec2        u_spriteOffset;
uniform int         u_spriteRotated;

//input light
uniform int         u_nbLights;
uniform vec3        u_ambientColor;
uniform float       u_spotLight[9 * MAX_SPOT_LIGHTS];
//brightness|//color_r|color_g|color_b|//pos_x|pos_y|pos_z//cutoff|halfradius

vec3 computeLighting(vec3 lightColor, vec3 lightDir, float brightness, float cutoff, float halfradius)
{
    float intercept = cutoff * halfradius;
    float dx_1 = 0.5 / intercept;
    float dx_2 = 0.5 / (cutoff - intercept);
    float offset = 0.5 + intercept * dx_2;
    float lightDist = length(lightDir);
    float falloffTermNear = clamp((1.0 - lightDist * dx_1), 0.0, 1.0);
    float falloffTermFar  = clamp((offset - lightDist * dx_2), 0.0, 1.0);
    float falloffSelect = step(intercept, lightDist);
    float falloffTerm = (1.0 - falloffSelect) * falloffTermNear + falloffSelect * falloffTermFar;
    
    return brightness * falloffTerm * lightColor;
}

void main(void)
{
    //texture
    vec4 texColor=texture2D(CC_Texture0, v_texCoord);
    vec2 spriteTexCoord = (v_texCoord - u_spritePosInSheet) / u_spriteSizeRelToSheet; // [0..1]
    vec2 pixelPos = spriteTexCoord * u_contentSize + u_spriteOffset; // [0..origSize]
    vec3 curPixelPosInLocalSpace = (u_spriteRotated == 1) ?
        vec3(pixelPos.y, pixelPos.x, 0.0) : vec3(pixelPos.x, u_contentSize.y - pixelPos.y, 0.0);
    
    //lights
    vec3 combined = u_ambientColor;
    if (u_nbLights > 0)
    {
        for(int i = 0; i < u_nbLights && i < MAX_SPOT_LIGHTS; ++i)
        {
            float lightBrightness = u_spotLight[i * 9 + 0];
            vec3 lightColor = vec3(u_spotLight[i * 9 + 1],
                                   u_spotLight[i * 9 + 2],
                                   u_spotLight[i * 9 + 3]);
            vec3 lightPos = vec3(u_spotLight[i * 9 + 4],
                                 u_spotLight[i * 9 + 5],
                                 u_spotLight[i * 9 + 6]);
            float lightCutoff = u_spotLight[i * 9 + 7];
            float lightHalfradius = u_spotLight[i * 9 + 8];
        
            combined += computeLighting(lightColor, curPixelPosInLocalSpace - lightPos,
                                        lightBrightness, lightCutoff, lightHalfradius);
        }
    }
    
    gl_FragColor = vec4(texColor.rgb * combined, texColor.a);
}