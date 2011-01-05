
varying vec3 diffuseColor;

varying vec4 viewPos;
varying vec4 coord;

uniform sampler2D lightIndexTex;
uniform sampler2D lightPosTex;
uniform sampler2D lightColorTex;

vec3 gamma(vec3 color)
{
   return pow(color, vec3(1.0 / 2.0));
}

void main()
{
   vec3 color = diffuseColor * 0.2;
   vec2 uv = ((coord.xy / coord.w) + 1.0) * 0.5;
   
   vec4 lightIndex = texture2D(lightIndexTex, vec2(uv.x, uv.y));
   
   vec4 lightPos = texture2D(lightPosTex, vec2(0.5, lightIndex.x + 0.5 / 256.0));
   vec4 lightColor = texture2D(lightColorTex, vec2(0.5, lightIndex.x + 0.5 / 256.0));
   
   float l = 1.0 - clamp(distance(lightPos.xyz, viewPos.xyz) / lightPos.w, 0.0, 1.0);
   
   //gl_FragColor = vec4(lightPos.xyz, 1.0);
   gl_FragColor = vec4(l, l, l, 1.0);
   //gl_FragColor = vec4(coord.y, 0.0, 0.0, 1.0);
   //gl_FragColor = vec4(lightColor.xyz, 1.0);
   //gl_FragColor = vec4(lightIndex.yyy, 1.0);
}