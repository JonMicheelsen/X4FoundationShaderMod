////////////////// JON MOD LIGHTING FUNCTIONS //////////////////
//Included right before global_lights() in common.fh, that is included in all lighting code

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
