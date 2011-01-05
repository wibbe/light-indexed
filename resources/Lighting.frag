
varying vec3 diffuseColor;

varying vec4 viewPos;
varying vec3 viewNormal;
varying vec4 coord;

uniform sampler2D lightIndexTex;
uniform sampler2D lightPosTex;
uniform sampler2D lightColorTex;

vec3 gamma(vec3 color)
{
   return pow(color, vec3(1.0 / 2.0));
}

vec3 pointLight(vec4 lightPos, vec3 lightColor, vec3 pos, vec3 normal)
{
   vec3 lightDir = lightPos.xyz - pos;
   float dist = length(lightDir);
   lightDir = normalize(lightDir);
   
   float intensity = max(0.0, dot(normal, lightDir)) * (1.0 - clamp(dist / lightPos.w, 0.0, 1.0));
   return lightColor * intensity;
}

void main()
{
   vec3 color = diffuseColor * 0.1;
   vec2 uv = ((coord.xy / coord.w) + 1.0) * 0.5;
   vec3 normal = normalize(viewNormal);
   
   vec4 lightIndex = texture2D(lightIndexTex, vec2(uv.x, uv.y));
   
   //if (lightIndex.x > 0.0)
   {
      vec4 lightPos = texture2D(lightPosTex, vec2(0.5, lightIndex.x + 0.5 / 256.0));
      vec4 lightColor = texture2D(lightColorTex, vec2(0.5, lightIndex.x + 0.5 / 256.0));
      color += pointLight(lightPos, lightColor.rgb, viewPos.xyz, normal);
   }
   //if (lightIndex.y > 0.0)
   {
      vec4 lightPos = texture2D(lightPosTex, vec2(0.5, lightIndex.y + 0.5 / 256.0));
      vec4 lightColor = texture2D(lightColorTex, vec2(0.5, lightIndex.y + 0.5 / 256.0));
      color += pointLight(lightPos, lightColor.rgb, viewPos.xyz, normal);
   }
   //if (lightIndex.z > 0.0)
   {
      vec4 lightPos = texture2D(lightPosTex, vec2(0.5, lightIndex.z + 0.5 / 256.0));
      vec4 lightColor = texture2D(lightColorTex, vec2(0.5, lightIndex.z + 0.5 / 256.0));
      color += pointLight(lightPos, lightColor.rgb, viewPos.xyz, normal);
   }
   //if (lightIndex.a > 0.0)
   {
      vec4 lightPos = texture2D(lightPosTex, vec2(0.5, lightIndex.a + 0.5 / 256.0));
      vec4 lightColor = texture2D(lightColorTex, vec2(0.5, lightIndex.a + 0.5 / 256.0));
      color += pointLight(lightPos, lightColor.rgb, viewPos.xyz, normal);
   }
   
   gl_FragColor = vec4(color, 1.0);
}