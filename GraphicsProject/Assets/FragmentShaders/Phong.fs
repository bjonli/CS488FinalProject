#version 330
// INPUTS/OUTPUTS -----------------------------------------
in VsOutFsIn {
	vec3 fragPos; 
	vec3 normal;  
	vec2 texCoords;
    vec4 fragPosDirLightSpace; 
} fs_in;

out vec4 fragColour;

// LIGHTS AND LANTERNS ------------------------------------
uniform int shadowsEnabled;
struct DirectionalLight {
    vec3 direction;
    vec3 rgbIntensity;
};
uniform DirectionalLight directionalLight;
uniform vec3 dirLightPos;
uniform sampler2D shadowMap;

// for now, we cap number of lights to be 10
#define NR_POINT_LIGHTS 10
struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
    vec3 attenuationVals;
};
uniform LightSource pointLights[NR_POINT_LIGHTS];
uniform int numPointLights;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

// define lantern types
#define LANTERN_NORMAL 0
#define LANTERN_GRAYSCALE 1
#define LANTERN_CELSHADING 2
#define LANTERN_INVERT 3

// for now, we cap number of lanterns to be 10
#define NR_LANTERNS 10
struct Lantern {
    vec3 position;
    float radius;
    int lanternType;
};
uniform Lantern lanterns[NR_LANTERNS];
uniform int numLanterns;

// TEXTURES AND MATERIALS 
#define MATERIAL_PLAIN 0
#define MATERIAL_TEXTURE 1
uniform int materialType;
struct Material {
    vec4 kd;			// last element transparency
    vec3 ks;
    float shininess;
};
uniform Material material;
uniform sampler2D colourTexture;

// view Position
uniform vec3 viewPosition;

vec3 phongModelperPointLight(
    vec3 fragPosition, 
    vec3 fragNormal,
    LightSource light, 
    vec3 viewDir,
	vec3 kd,
	vec4 ks
) {
	// end early if light not on
	if (light.rgbIntensity == vec3(0)) {return vec3(0, 0, 0);}

    // calculate attenuation
    float d = length(light.position - fragPosition);
    vec3 a = light.attenuationVals;
    float attenuation = 1.0/(a[0] + a[1]*d + a[2]*d*d);

    // Direction from fragment to light source.
    vec3 lightDir = normalize(light.position - fragPosition);
    
    // calculate diffuse
    float n_dot_l = max(dot(fragNormal, lightDir), 0.0);
	vec3 diffuse = kd * n_dot_l * attenuation;
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), ks[3]);
	vec3 specular = vec3(ks) * spec * attenuation;

    return light.rgbIntensity * (diffuse + specular);
}

// random float from 0 to 1
float rand(vec4 seed4) {
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float dirLightShadowCalculation (
    vec4 fragPosDirLightSpace,
    vec3 fragPosition, 
    vec3 fragNormal
) {
	if (shadowsEnabled == 0) { return 0; }
    // perform perspective divide
    vec3 projCoords = fragPosDirLightSpace.xyz / fragPosDirLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(dirLightPos - fragPosition);
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.01);
    // check whether current frag pos is in shadow

    // PCF to perform soft shadows.
    // average the shadow values among 9 surrounding pixels
	// CR-soon: PCF ruins the shading of spheres, investigate further
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            for (int i=0; i<4; i++) {
                int index = int(16*rand(vec4(gl_FragCoord.xyy, i)))%16;

                float pcfDepth = texture(shadowMap, 
                    projCoords.xy + (vec2(x, y)) * texelSize
                ).r; 
                shadow += currentDepth - bias > pcfDepth  ? 1 : 0.0;        
            }
        }    
    }
    shadow /= 9.0;
    shadow = currentDepth - bias > closestDepth  ? 1 : 0.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 phongModelforDirectionalLight(
    vec3 fragPosition, 
    vec3 fragNormal,
    vec4 fragPosDirLightSpace,
    vec3 viewDir,
	vec3 kd,
	vec4 ks
) {
    vec3 lightDir = normalize(-directionalLight.direction);
    // calculate diffuse
    float n_dot_l = max(dot(fragNormal, lightDir), 0.0);

	vec3 diffuse = kd * n_dot_l;
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), ks[3]);
	vec3 specular = vec3(ks) * spec;

    // calculate shadow
    float shadow = dirLightShadowCalculation(
        fragPosDirLightSpace,
        fragPosition,
        fragNormal
    );

    return directionalLight.rgbIntensity * (diffuse+specular) * (1-shadow);
}

vec4 phongModel(
    vec3 fragPosition, 
    vec3 fragNormal,
    vec4 fragPosDirLightSpace,
	vec2 fragTexCoords,
    vec3 viewDir,
    bool includeSpecular
) {
    vec3 totalColour = ambientIntensity;

	// figure out the Material
	vec3 kd = vec3(0);
	vec4 ks = vec4(0, 0, 0, 1);
	float transparency = 1;
	if (materialType == MATERIAL_PLAIN) {		// get kd from material
		kd = vec3(material.kd);
		transparency = material.kd.a;
		if (includeSpecular) {
			ks = vec4(material.ks, material.shininess);
		}
	} 
	else {										// get kd from texture
		vec4 rgba = texture(colourTexture, fragTexCoords);
		kd = vec3(rgba);
		transparency = rgba.a;
		// no specular for now
	}

	// directional light first
    totalColour += phongModelforDirectionalLight(
        fragPosition, fragNormal, fragPosDirLightSpace, viewDir, kd, ks
    );
	// all the point lights
    for (int i=0; i<numPointLights; i++) {
        totalColour += phongModelperPointLight(
            fragPosition, fragNormal, pointLights[i], viewDir, kd, ks
        );
    } 
    return vec4(totalColour, transparency);
}

vec4 invert(vec4 c) {
    return vec4(vec3(1)-vec3(c), c.a);
}

vec4 grayscale(vec4 c) {
    float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
    return vec4(gray, gray, gray, c.a);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 celShading(vec4 c) {
    vec3 hsv = rgb2hsv(vec3(c));
    // the ranges are [0, 0.5] and [0.5, 1]
    if (hsv[2] < 0.5) { hsv[2] = 0.25; }
    else { hsv[2] = 0.75; }
    // CR-someday, check if I should keep c.a, or for opaque
    return vec4(hsv2rgb(hsv), c.a); 
}

void main() {
    vec3 viewDir = normalize(viewPosition - fs_in.fragPos);
	// by default, the fragment will be phong shaded
    vec4 phongColour = phongModel(
        fs_in.fragPos,
        fs_in.normal, 
        fs_in.fragPosDirLightSpace,
		fs_in.texCoords,
        viewDir, true
    );
    fragColour = phongColour;

    for (int i=0; i<numLanterns; i++) {
        // distance check
        vec3 lpos = lanterns[i].position;
        float rad = lanterns[i].radius;
        int lanternType = lanterns[i].lanternType;
        vec3 delta = lpos-fs_in.fragPos;
        float squareDis = dot(delta, delta);
        // if in the radius, apply the filter
        if (squareDis <= rad*rad) {
            // CR-someday: maybe apply multiple filters at once
            switch (lanterns[i].lanternType) {
                case LANTERN_GRAYSCALE:
                    fragColour = grayscale(phongColour);
                    break;
                case LANTERN_INVERT:
                    fragColour = invert(phongColour);
                    break;
                case LANTERN_CELSHADING:
                    // we do not include specular for cellshading, it looks wierd
                    fragColour = celShading(phongModel(
                        fs_in.fragPos, 
                        fs_in.normal, 
                        fs_in.fragPosDirLightSpace, 
						fs_in.texCoords,
                        viewDir, false
                    ));
					//translucent objects no longer see through in cel shading
					fragColour[3] = 1;
                    break;
            }
        }
        // if near a lantern that is activated and not normal,
        // then blend it with some white
        float squareDiff = abs(rad*rad-squareDis);
        if (squareDiff < 10 && rad > 0.01) {
            float blendFactor = 1/(1+30*squareDiff*squareDiff);
            fragColour.r += (1-fragColour.r)*blendFactor;
            fragColour.g += (1-fragColour.g)*blendFactor;
            fragColour.b += (1-fragColour.b)*blendFactor;
        }
    }
}
