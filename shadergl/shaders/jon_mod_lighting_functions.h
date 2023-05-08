////////////////// JON MOD LIGHTING FUNCTIONS //////////////////
//Included right before global_lights() in common.fh, that is included in all lighting code
// as well as in the start of lighting_common.h

#ifndef _JON_MOD_LIGHTING_FUNCTIONS_
//L we have to trasnform to view space
float ScreenSpaceShadows(	in vec3 light_ray, 
							in vec2 cascade_blend, 
							inout vec3 debug, 
							in int i)	
{
	float step_size = 1.0 / float(JON_MOD_SSSHADOWS_MAX_STEPS);
	
	// Compute ray step
	vec3 ray_step = light_ray * step_size * max(JON_MOD_SSSHADOWS_RAY_MAX_DISTANCE_NEAR, JON_MOD_SSSHADOWS_RAY_MAX_DISTANCE * cascade_blend.y);
	
	// Ray march towards the light
	float shadow = 0.0;
	vec2 ray_uv = vec2(0.0);
	vec2 fade = vec2(0.0);

	vec3 ray_pos = GetViewPos();
	float2 aspect = float2(0.5, -0.5 * V_viewportpixelsize.x / V_viewportpixelsize.y);

	//different ways of getting what we need
	//debug = fract(vec3(1.0 / vec2(GetDepth(GetFragUV()), GetDepth()), GetViewPos().z * 10.0));//why 10?
	//works too
	//debug = fract(vec3(1.0 / vec2(GetDepth((ray_pos.xy / ray_pos.z) * aspect + 0.5), GetDepth()), GetViewPos().z * 10.0));//why 10?
	
	float depth = (ray_pos.z * 10.0);
	vec2 dither = hash22(gl_FragCoord.xy);
	vec2 texel_size = (1.0 / V_viewportpixelsize.xy);
	float thickness_threshold = max(JON_MOD_SSSHADOWS_MAX_THICKNESS_NEAR, JON_MOD_SSSHADOWS_MAX_THICKNESS * ceil(cascade_blend.y));
	float depth_bias = max(JON_MOD_SSSHADOWS_BIAS_NEAR * (1.0 + depth * 0.001), JON_MOD_SSSHADOWS_BIAS * ceil(cascade_blend.y));
	float attenuation = ceil(cascade_blend.x) * step_size * JON_MOD_SSSHADOWS_ATTENUATION_NEAR;
	
	for (uint i = 0; i < JON_MOD_SSSHADOWS_MAX_STEPS; i++)
	{
		// Step the ray
		ray_pos -= ray_step;
		ray_uv = ray_pos.xy / ray_pos.z;

		fade = abs(ray_uv);
		
		// Ensure the UV coordinates are inside the screen
		if(max(fade.x, fade.y) > 1.0)
			break;
		
		float depth_step = 1.0 / GetDepth(ray_uv * aspect + 0.5 + uv2clip(fract(dither)) * texel_size);
		dither += step_size;
		float depth_delta = ray_pos.z * 10.0 - depth_step - depth_bias;
		
		if(depth_delta > 0.0 && depth_delta <  thickness_threshold)
		{
			// Fade out as we approach the edges of the screen
			fade				= 1.0 - saturate(fade * 100.0 - 99.0);
			shadow				= min(fade.x, fade.y);
			shadow				= 1 - attenuation * i;
			break;
		}

	}
	#ifdef JON_MOD_SSSHADOWS_DEBUG_MODE
		if(i == JON_MOD_SSSHADOWS_LIGHT_TO_DEBUG)
			debug.y = shadow;
	#endif
	
	return 1.0 - shadow * (cascade_blend.x + cascade_blend.y);
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
// Similar to UE4/5 but with actual luminance, since that's a nice touch, rather than just cspec.g
vec3 schlick_f(vec3 cspec, float v_dot_h)
{
	float f = pow5(1.0 - v_dot_h);
	#ifdef JON_MOD_COMPARE_VANILLA_SPLIT_SCREEN
		if(GetViewPos().x < 0.0)
			return f + (1-f) * cspec;
	#endif	
	return saturate(50.0 * dot(LUM_ITU601, cspec)) * f + (1.0 - f) * cspec;
}
// https://advances.realtimerendering.com/s2018/index.htm
// It has been extended here to fade out retro reflectivity contribution from area light in order to avoid visual artefacts.
vec3 chan_diff(vec3 cdiff, float a2, float n_dot_v, float n_dot_l, float v_dot_h, float n_dot_h, float arealight_weight, vec3 cspec)
{
	float g = saturate((1.0 / 18.0) * log2(2.0 / a2 - 1.0));
	
	float f0 = (v_dot_h + pow5(1.0 - v_dot_h));
	float fdv = (1.0 - 0.75 * pow5(1.0 - n_dot_v));
	float fdl = (1.0 - 0.75 * pow5(1.0 - n_dot_l));

	// Rough (f0) to smooth (fdv * fdv) response interpolation
	float fd = mix(f0, fdv * fdl, saturate(2.2 * g - 0.5));
	
	// Retro reflectivity contribution.
	float fb = ((34.5 * g - 59.0) * g + 24.5) * v_dot_h * exp2(-max(73.2 * g - 21.2, 8.9) * sqrt(n_dot_h));
	
	// It fades out when lights become area lights in order to avoid visual artefacts.
	fb *= arealight_weight;
	
	#ifdef JON_MOD_COMPARE_VANILLA_SPLIT_SCREEN
		if(GetViewPos().x < 0.0)
			return cdiff * (1.0 / PI) * saturate(1.0f - dot(LUM_ITU601, cspec));
	#endif
		
	return cdiff * ((1.0 / PI) * (fd + fb));
	
}
#define _JON_MOD_LIGHTING_FUNCTIONS_
#endif
