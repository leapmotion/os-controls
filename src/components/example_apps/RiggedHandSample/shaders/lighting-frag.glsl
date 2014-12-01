#ifdef GL_ES
precision highp float;
#endif

varying vec2 outTexCoord;
varying vec3 outPosition;
varying vec3 outNormal;

uniform bool use_texture;
uniform sampler2D texture;

uniform bool useNormalMap;
uniform sampler2D normalMap;

uniform bool useSpecularMap;
uniform sampler2D specularMap;

// material
uniform vec4 diffuse_light_color;
uniform vec4 ambient_light_color;
uniform float ambient_lighting_proportion;
uniform vec4 specular;
uniform float shininess;
uniform vec4 emission;

// lighting
#define MAX_LIGHTS 5
uniform vec3 lightPos[MAX_LIGHTS];
uniform float lightStrengths[MAX_LIGHTS];
uniform int numLights;
uniform float attenuation;

uniform float minDepthDist;
uniform float maxDepthDist;

uniform bool depthImage;
uniform bool normalImage;
uniform bool flatImage;

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
  mat3 TBN = cotangent_frame(normal, -view, texcoord);
  return normalize(TBN * map);
}

void main (void)
{
  if (depthImage) {
    float dist = length(outPosition);
    float value = 1.0 - clamp((dist - minDepthDist)/(maxDepthDist - minDepthDist), 0.0, 1.0);
    gl_FragColor.rgb = vec3(value);
    gl_FragColor.a = 1.0;
  } else if (flatImage) {
    gl_FragColor = emission + ambient_lighting_proportion*ambient_light_color;
  } else {
    vec4 Dm = vec4(1.0);
    if (use_texture) {
      Dm = texture2D(texture, outTexCoord);
    }

    vec3 N = normalize(outNormal);
    vec3 E = normalize(-outPosition);

    vec3 normal = N;
    if (useNormalMap) {
      normal = perturb_normal(N, E, outTexCoord);
    }

    if (normalImage) {
      gl_FragColor.rbg = 0.5*(normal + vec3(1.0)); // .rbg is intentional to flip y and z
      gl_FragColor.a = 1.0;
    } else {
      vec4 specularColor = specular;
      if (useSpecularMap) {
        specularColor = specular * texture2D(specularMap, outTexCoord);
      }

      vec4 finalColor = emission + ambient_lighting_proportion * ambient_light_color * Dm;
      for (int i=0; i<numLights; i++) {
        vec3 lightDir = lightPos[i] - outPosition;
        float dist = length(lightDir);
        vec3 L = lightDir / dist;
        vec3 R = normalize(-reflect(L, normal));

        float ratio = minDepthDist / dist;
        float falloff = (1.0-attenuation) + attenuation*ratio*ratio;

        vec4 Idiff = lightStrengths[i] * falloff * diffuse_light_color * max(dot(normal, L), 0.0) * Dm;

        vec4 Ispec = lightStrengths[i] * falloff * specularColor * pow(max(dot(R, E), 0.0), shininess);

        finalColor = finalColor + Idiff + Ispec;
      }

      finalColor.a = 1.0;
      gl_FragColor = finalColor;
    }
  }
}
