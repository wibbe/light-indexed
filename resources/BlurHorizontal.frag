#version 120


uniform sampler2D tex;
uniform float step;

void main()
{
   vec4 c0 = texture2D(tex, gl_TexCoord[0].xy + vec2(step * -3.0, 0.0)) * 0.05;
   vec4 c1 = texture2D(tex, gl_TexCoord[0].xy + vec2(step * -2.0, 0.0)) * 0.1;
   vec4 c2 = texture2D(tex, gl_TexCoord[0].xy + vec2(step * -1.0, 0.0)) * 0.2;
   vec4 c3 = texture2D(tex, gl_TexCoord[0].xy + vec2(step *  0.0, 0.0)) * 0.3;
   vec4 c4 = texture2D(tex, gl_TexCoord[0].xy + vec2(step *  1.0, 0.0)) * 0.2;
   vec4 c5 = texture2D(tex, gl_TexCoord[0].xy + vec2(step *  2.0, 0.0)) * 0.1;
   vec4 c6 = texture2D(tex, gl_TexCoord[0].xy + vec2(step *  3.0, 0.0)) * 0.05;
   
   gl_FragColor = c0 + c1 + c2 + c3 + c4 + c5 + c6;
}
