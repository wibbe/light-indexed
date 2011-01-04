
varying vec3 diffuseColor;

vec3 sphericalHarmonics(vec3 normal)
{
   const float C1 = 0.429043;
   const float C2 = 0.511664;
   const float C3 = 0.743125;
   const float C4 = 0.886227;
   const float C5 = 0.247708;
   
   const vec3 L00  = vec3( 0.6841148,  0.6929004,  0.7069543);
   const vec3 L1m1 = vec3( 0.3173355,  0.3694407,  0.4406839);
   const vec3 L10  = vec3(-0.1747193, -0.1737154, -0.1657420);
   const vec3 L11  = vec3(-0.4496467, -0.4155184, -0.3416573);
   const vec3 L2m2 = vec3(-0.1690202, -0.1703022, -0.1525870);
   const vec3 L2m1 = vec3(-0.0837808, -0.0940454, -0.1027518);
   const vec3 L20  = vec3(-0.0319670, -0.0214051, -0.0147691);
   const vec3 L21  = vec3( 0.1641816,  0.1377558,  0.1010403);
   const vec3 L22  = vec3( 0.3697189,  0.3097930,  0.2029923);
   
   return C1 * L22 * (normal.x * normal.x - normal.y * normal.y) +
          C3 * L20 * normal.z * normal.z +
          C4 * L00 -
          C5 * L20 +
          2.0 * C1 * L2m2 * normal.x * normal.y +
          2.0 * C1 * L21  * normal.x * normal.z +
          2.0 * C1 * L2m1 * normal.y * normal.z +
          2.0 * C2 * L11  * normal.x +
          2.0 * C2 * L1m1 * normal.y +
          2.0 * C2 * L10  * normal.z;
}


void main()
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
   
   vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
   
   diffuseColor = sphericalHarmonics(normal);
}