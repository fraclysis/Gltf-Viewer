#version 330 core
out vec4 FragColor;

in vec4 fCol;
in vec3 fPos;
in vec3 fNorm;
in vec2 fTEXCOORD_0;

uniform sampler2D normalTexture;
uniform float normalTextureScale;

uniform vec4 baseColorFactor;
uniform sampler2D baseColorTexture;

uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D metallicRoughnessTexture;

uniform sampler2D emissiveTexture;
uniform vec3 emissiveFactor;

uniform float occlusionTextureStrength;
uniform sampler2D occlusionTexture;

uniform float alphaCutoff;
uniform bool alphaCutoffEnabled;

uniform vec3 cameraPosition;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];

uniform vec3 directionalLight;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalTexture, fTEXCOORD_0).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fPos);
    vec3 Q2  = dFdy(fPos);
    vec2 st1 = dFdx(fTEXCOORD_0);
    vec2 st2 = dFdy(fTEXCOORD_0);

    vec3 N   = normalize(fNorm);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec4 objectColor = mix(vec4(0.0), texture(baseColorTexture, fTEXCOORD_0), baseColorFactor);
    if (alphaCutoffEnabled) {
        if(objectColor.a < alphaCutoff) {
            discard;
        }
    }

    vec3 albedo = pow(objectColor.rgb, vec3(2.2));
    float ao = texture(occlusionTexture, fTEXCOORD_0).r;
    ao = 1.0f;

    float roughness = mix(0.0, texture(metallicRoughnessTexture, fTEXCOORD_0).g, roughnessFactor);
    float metallic = mix(0.0, texture(metallicRoughnessTexture, fTEXCOORD_0).b, metallicFactor);

    vec3 F0 = vec3(0.4);

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraPosition - fPos); 

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 4 ; i++) {
        vec3 L = normalize(lightPositions[i] - fPos);
        vec3 H = normalize(V + L);
        
        float distance_ = length(lightPositions[i] - fPos);
        float attenuation = 1.0/ (distance_ * distance_);
        vec3 radiance = lightColors[i] * attenuation;
       
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }

        vec3 sun_color = vec3(1.0);
        vec3 sun_light_dir= vec3(0.0,0.5, 0.4);

        vec3 sL= directionalLight;
        vec3 sH = normalize(V + sL);
        vec3 sradiance = sun_color * 0.6;
          // cook-torrance brdf
        float NDF = DistributionGGX(N, sH, roughness);        
        float G   = GeometrySmith(N, V, sL, roughness);      
        vec3 F    = fresnelSchlick(max(dot(sH, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, sL), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, sL), 0.0);                
        Lo += (kD * albedo / PI + specular) * sradiance * NdotL;
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
    FragColor = vec4(color, objectColor.a);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}