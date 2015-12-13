#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//uniform vec2 screenSize;
//uniform vec2 blendCenter;
//uniform vec2 spritePosition;
//uniform vec2 spriteFrameOrigin;
//uniform vec2 spriteFrameSize;

const float kMin = 0.0196070;
const float kMax = 0.8784313;

const float kMult = 1.0 / (kMax - kMin);

#define ChannelBlend_ColorBurn(A,B) ((B==0)?B:max(0,(255-((255-A)<<8)/B)))
#define ChannelBlend_SoftLight(A,B) ((B<128)?(2*((A>>1)+64))*((float)B/255):(255-(2*(255-((A>>1)+64))*(float)(255-B)/255)))

void main(void)
{
    //float f1 = screenSize.x;
    //float f2 = blendCenter.x;
    //float f3 = spritePosition.x;
    //float f4 = spriteFrameOrigin.x;
    //float f5 = spriteFrameSize.x;
    
    vec4 col = texture2D(CC_Texture0, v_texCoord).rgba;
    //col.g += spritePosition.x / screenSize.x;
    
    gl_FragColor = v_fragmentColor * col;
}
