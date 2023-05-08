////////////////// JON MOD SETTINGS START //////////////////
//Included in common.h, that is included most anywhere

//#define JON_MOD_SPECULAR_OVERSHOOT_FIX //default on, fixes geometric light white bright artefact
#define JON_MOD_DISABLE_EGOSOFT_SMOOTHER_GRAZING_ANGLE //default on

#define JON_MOD_ROUGHNESS_REMAP // default on. Old Pixar trick, since artists has a tendency to overuse the lower end of the roughness and neglect the high end, this one does a gentle remap improving both!
#define JON_MOD_ROUGHNESS_REMAP_PRE_SQUARE_RANGE 0.5//default 0.5

//#define JON_MOD_ENFORCE_STRICT_METALLIC
//#define JON_MOD_ENFORCE_PBR_INPUT_COLORS		//will follow the loose PBR guidlines
//#define JON_MOD_ENFORCE_PBR_INPUT_COLORS_STRICT //will endorce the strict PBR guidlines


// Human skin lowest index of refraction(IOR)1.35 = 0.28 in unreal specular, or #47, rgb71, 28%brightness, as linear color
// Human skin highest index of refraction(IOR)1.55 = 0.58 in unreal specular, or #94, rgb148, 58%brightness, as linear color
// Human hair(and nails technically too) highest index of refraction(IOR)1.55 = 0.58 in unreal specular, or #94, rgb148,
// Specular scale is 0.08, so (0.28 * 0.08), etc...
#define JON_MOD_ENABLE_SUBSURFACE_GBUFFER_PACKING
#define JON_MOD_SUBSURFACE_SUBDERMAL_ROUGHNESS 0.5
#define JON_MOD_SUBSURFACE_EPIDERMAL_ROUGHNESS -0.1
#define JON_MOD_SUBSURFACE_EPIDERMAL_F0 0.0224f

#define JON_MOD_USE_RETROREFLECTIVE_DIFFUSE_MODEL//This upgrades EvalBRDF() to include this https://advances.realtimerendering.com/s2018/MaterialAdvancesInWWII-course_notes.pdf Which UE5 also uses.

#define JON_MOD_USE_LUMINANCE_FRESNEL

#define JON_MOD_USE_ROUGHNESS_SQRT_PROBE_ROUGHNESS //un-fuck-up-ing it

#define JON_MOD_SSSHADOWS //default on
#define JON_MOD_SSSHADOWS_SUPPORT_BOTH_PRIMARY_LIGHTS //default on
#define JON_MOD_SSSHADOWS_MAX_STEPS 32 //default 64, it's pretty rare to ever go this high be aware this is a times 2

#define JON_MOD_SSSHADOWS_FADE_DISTANCE 5000.0 //not all PCF shadows have the same distance, so some lods will still shadow pop if we just use that, so with this we can enforce a max distance where everything will use Screenspace Shadows
#define JON_MOD_SSSHADOWS_DITHER 2.0 //default 1

#define JON_MOD_SSSHADOWS_RAY_MAX_DISTANCE 240.0 //default 240.0 = 24.0m distance. I think 1 is about 10cm, so a hundred meters is 10000
#define JON_MOD_SSSHADOWS_BIAS 1.0 //default 10.0 = 1.0m
#define JON_MOD_SSSHADOWS_MAX_THICKNESS 1000.0 //default 1000.0 = 100.0m shadows thicker than this from the camera are ignored, so as to things near ain't casting shadows on things far

#define JON_MOD_SSSHADOWS_RAY_MAX_DISTANCE_NEAR 0.25 //default 10cm
#define JON_MOD_SSSHADOWS_BIAS_NEAR 0.05 //0.1cm
#define JON_MOD_SSSHADOWS_MAX_THICKNESS_NEAR 0.1 //1cm
#define JON_MOD_SSSHADOWS_FADE_DISTANCE_NEAR 10.0 //not all PCF shadows have the same distance, so some lods will still shadow pop if we just use that, so with this we can enforce a max distance where everything will use Screenspace Shadows
#define JON_MOD_SSSHADOWS_ATTENUATION_NEAR 1.0
#define JON_MOD_SSSHADO_FILTER // filters with screenspace derivatives, might give very different result at different resolution

#define JON_MOD_SSSHADOWS_POINTLIGHTS

//debugs
//#define JON_MOD_SSSHADOWS_DEBUG_MODE
//#define JON_MOD_SSSHADOWS_LIGHT_TO_DEBUG 0 //options are 0 or 1 only!

//#define JON_MOD_COMPARE_VANILLA_SPLIT_SCREEN

#define INVPI 	(0.318309886f)
//nice angle corrected blend
vec3 BlendReorientedNormals(vec3 n1, vec3 n2)
{

	n1 = vec3(n1.xy, n1.z + 1.0);
	n2 = vec3(-n2.xy, n2.z);

    return normalize(n1 * dot(n1, n2) - n2 * n1.z);
}
////////////////// JON MOD SETTINGS END //////////////////