
varying vec3 diffuseColor;

vec3 gamma(vec3 color)
{
   return pow(color, vec3(1.0 / 2.0));
}

void main()
{
   gl_FragColor = vec4(gamma(diffuseColor * 0.5), 1.0);
}