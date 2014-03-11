varying vec3 transformed_normal;
varying vec3 light_direction;
varying vec3 spot_direction;
varying vec3 eye_vec;

uniform vec4 light_color;
uniform float spot_cutoff;
uniform float spot_exponent;
uniform float linear_attenuation;

void main()
{
  float ambient = 0.2;
  float constant_attenuation = 1.0;
  float quadratic_attenuation = 0.0;
  
  vec3 n = normalize(transformed_normal);
  float light_dist = length(light_direction);
  vec3 l = light_direction/light_dist;

  vec3 s_dir = normalize(spot_direction);
  float spot_cos = dot(l, -s_dir);

  //distance based attenuation.
  float attenuation = 1.0 / (constant_attenuation + linear_attenuation * light_dist + quadratic_attenuation * light_dist * light_dist);

  //outside of the spotlight range
  if(spot_cos < spot_cutoff)
  {
        attenuation = 0.0;
  }
  else //falloff toward sides of spotlight
  {
	attenuation *= pow(spot_cos, spot_exponent);
  }
  float diffuse = max(dot(l, n), 0.0);

  gl_FragColor = light_color*(gl_Color*(diffuse*attenuation + ambient));
}
