#define P1_SHADERS
#include <common.fh>

#ifdef JON_MOD_USE_RETROREFLECTIVE_DIFFUSE_MODEL
vec3 integrate_GGX_and_retroreflective_diffuse(float roughness, float n_dot_v)
{
  vec3 normal = vec3(0.0f, 0.0f, 1.0f);

  vec3 view = vec3(sqrt(1.0f - n_dot_v * n_dot_v), 0, n_dot_v);
  float a = 0.0f; float b = 0.0f; float c = 0.0f;
 #ifdef JON_MOD_USE_RETROREFLECTIVE_DIFFUSE_MODEL
	CONST uint num_samples = 256u;//the diffuse needs a ton more samples - and the specular ones looks even better with more too!
#else
	CONST uint num_samples = 32u;//was 32
#endif	
  for(uint i=0u; i< num_samples; ++i)
  {
	vec2 uv = hammersley_2d(i, num_samples);
	
	vec3 half_dir = importance_sample_GGX(uv, roughness, normal);
	vec3 light = 2 * dot(view, half_dir) * half_dir - view;
	float n_dot_l = max(0.0f, light.z);
	// float n_dot_l = light.z;
	float n_dot_h = clamp(half_dir.z, 0.0f, 1.0f);
	// float n_dot_h = half_dir.z;
	float v_dot_h = clamp(dot(view, half_dir), 0.0f, 1.0f);
	// float v_dot_h = dot(view, half_dir);
	if( n_dot_l > 0.0f)
	// if( true )
	{
		
		float k = (roughness*roughness)/2;
		float G = G_Smith(k, n_dot_v, n_dot_l);
		// float G = G_smith(roughness, n_dot_v, n_dot_l);
	
		float G_vis = G * v_dot_h / (n_dot_h * n_dot_v);
		// G_vis = Vis_Smith(roughness, n_dot_v, n_dot_l);
		// float G_vis = G / (4.0f*n_dot_l*n_dot_v);
		float F_c = pow(1.0f - v_dot_h, 5);
		// float F0 = 0.5f; // reflectance at normal incidence
		// float F_c = F0 + (1-F0)*pow( 1.0f - v_dot_h, 5 );
		a += (1.0f - F_c) * G_vis;
		b += F_c * G_vis;
#ifdef JON_MOD_USE_RETROREFLECTIVE_DIFFUSE_MODEL
		// we can throw the retroreflective diffuse BRDF in here!
		float a2 = roughness*roughness;
		float g = saturate((1.0 / 18.0) * log2(2.0 / a2 - 1.0));
		float f0 = (v_dot_h + pow5(1.0 - v_dot_h));
		float fdv = (1.0 - 0.75 * pow5(1.0 - n_dot_v));
		float fdl = (1.0 - 0.75 * pow5(1.0 - n_dot_l));
	
		// Rough (f0) to smooth (fdv * fdv) response interpolation
		float fd = mix(f0, fdv * fdl, saturate(2.2 * g - 0.5));
		
		// Retro reflectivity contribution.
		float fb = ((34.5 * g - 59.0) * g + 24.5) * v_dot_h * exp2(-max(73.2 * g - 21.2, 8.9) * sqrt(n_dot_h));
		c += (fd + fb) * n_dot_l;
#endif		
	}
	
  }
  vec3 sum = vec3(a, b, c * PI) * (1.0 / num_samples);
  return sum * (1.0 / dot(sum, vec3(1.0)));
}

void main()
{
  	OUT_Color = vec4(0);
    OUT_Color.xyz = integrate_GGX_and_retroreflective_diffuse(IO_uv0.x, IO_uv0.y);
  	// OUT_Color = vec4(IO_uv0, 1, 1);
}
#else
vec2 integrate_GGX(float roughness, float n_dot_v)
{
  vec3 normal = vec3(0.0f, 0.0f, 1.0f);

  vec3 view = vec3(sqrt(1.0f - n_dot_v * n_dot_v), 0, n_dot_v);
  float a = 0.0f; float b = 0.0f;
  CONST uint num_samples = 32u;
  for(uint i=0u; i< num_samples; ++i)
  {
    vec2 uv = hammersley_2d(i, num_samples);

    vec3 half_dir = importance_sample_GGX(uv, roughness, normal);
    vec3 light = 2 * dot(view, half_dir) * half_dir - view;
    float n_dot_l = max(0.0f, light.z);
    // float n_dot_l = light.z;
    float n_dot_h = clamp(half_dir.z, 0.0f, 1.0f);
    // float n_dot_h = half_dir.z;
    float v_dot_h = clamp(dot(view, half_dir), 0.0f, 1.0f);
    // float v_dot_h = dot(view, half_dir);

    if( n_dot_l > 0.0f)
    // if( true )
    {
      float k = (roughness*roughness)/2;
      float G = G_Smith(k, n_dot_v, n_dot_l);
      // float G = G_smith(roughness, n_dot_v, n_dot_l);

      float G_vis = G * v_dot_h / (n_dot_h * n_dot_v);
      // G_vis = Vis_Smith(roughness, n_dot_v, n_dot_l);
      // float G_vis = G / (4.0f*n_dot_l*n_dot_v);
      float F_c = pow(1.0f - v_dot_h, 5);
      // float F0 = 0.5f; // reflectance at normal incidence
      // float F_c = F0 + (1-F0)*pow( 1.0f - v_dot_h, 5 );
      a += (1.0f - F_c) * G_vis;
      b += F_c * G_vis;
    }
  }
  return vec2(a, b) / num_samples;
}

void main()
{
  	OUT_Color = vec4(0);
    OUT_Color.xy = integrate_GGX(IO_uv0.x, IO_uv0.y);
  	// OUT_Color = vec4(IO_uv0, 1, 1);
}
	
#endif