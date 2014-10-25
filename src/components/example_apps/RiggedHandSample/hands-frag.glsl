#ifdef GL_ES
precision highp float;
#endif

varying vec2 outTexCoord;
varying vec3 outPosition;
varying vec3 outNormal;

uniform sampler2D texture;

uniform bool useNormalMap;
uniform sampler2D normalMap;

uniform bool useSpecularMap;
uniform sampler2D specularMap;

uniform float opacity;
uniform float innerTransparency;

// rim lighting
uniform vec4 rimColor;
uniform float rimStart;
const float rimEnd = 1.0;

// material
uniform vec3 lightPos;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shininess;
uniform vec4 emission;

mat3 cotangent_frame(vec3 normal, vec3 position, vec2 uv)
{
  vec3 dp1 = dFdx(position);
  vec3 dp2 = dFdy(position);
  vec2 duv1 = dFdx(uv);
  vec2 duv2 = dFdy(uv);

  vec3 dp2perp = cross(dp2, normal);
  vec3 dp1perp = cross(normal, dp1);
  vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

  float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
  return mat3(T * invmax, B * invmax, normal);
}

vec3 perturb_normal(vec3 normal, vec3 view, vec2 texcoord)
{
  vec3 encodedNormal = texture2D(normalMap, texcoord).xyz;
  vec3 map = normalize(vec3(2.0, 2.0, 2.0) * encodedNormal - vec3(1.0, 1.0, 1.0));
  mat3 TBN = cotangent_frame(normal, view, texcoord);
  return normalize(TBN * map);
}

void main (void)
{
	vec4 Dm = texture2D( texture, outTexCoord);
	
  vec3 N = normalize(outNormal);
	vec3 L = normalize(lightPos);
  vec3 E = normalize(outPosition);

  vec3 normal;
  if (useNormalMap) {
    normal = perturb_normal(N, E, outTexCoord);
  } else {
    normal = N;
  }
  vec3 R = normalize(reflect(L, normal));
	
	vec4 Iamb = ambient * Dm;
	
  vec4 Idiff = diffuse * max(dot(normal, -L), 0.0) * Dm;
		
  vec4 specularColor = specular;
  if (useSpecularMap) {
    specularColor = texture2D(specularMap, outTexCoord);
  }

	vec4 Ispec = specularColor * pow( max( dot(R, E), 0.0 ), shininess );
		
  float normalToCam = 1.0 - abs(dot(normal, E));
  float rim = smoothstep(rimStart, rimEnd, normalToCam);
  vec4 Irim = rim*rimColor;

  float edgeMult = 1.0 - innerTransparency*abs(dot(normal, E));

  gl_FragColor = emission + Iamb + Idiff + Ispec + Irim;
  gl_FragColor = edgeMult*gl_FragColor;

  //float distanceMult = clamp(-(outPosition.z+10)/20, 0.0, 1.0);

  //gl_FragColor *= distanceMult;
  gl_FragColor *= opacity;
}