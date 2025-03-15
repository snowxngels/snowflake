#version 400

in vec2 texCoords;
in vec3 normal;
in vec3 tangent;
in vec3 binormal;
in vec3 viewDirection;

out vec4 FragColor;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform vec3 light_source;

void main(void)
{
    // allocate vars
    vec3 lightDir;
    vec3 albedo, bumpMap;
    vec3 bumpNormal;
    float roughness, metallic;
    vec3 F0;
    vec3 halfDirection;
    float NdotH, NdotV, NdotL, HdotV;
    float roughnessSqr, roughSqr2, NdotHSqr, denominator, normalDistribution;
    float smithL, smithV, geometricShadow;
    vec3 fresnel;
    vec3 specularity;
    vec4 color;

    // dont Invert the light direction for calculations.
    lightDir = normalize(light_source);

    // Sample the textures.
    albedo = texture(albedoTexture, texCoords).rgb;
    bumpMap = texture(normalTexture, texCoords).rgb;

    // Calculate the normal using the normal map.
    bumpMap = (bumpMap * 2.0f) - 1.0f; // offset to -1,1
    bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * binormal) + (bumpMap.z * normal) + 0.1;
    bumpNormal = normalize(bumpNormal);

    // Get the metalic and roughness from the roughness/metalness texture.
    roughness = texture(roughnessTexture, texCoords).r; // Assuming roughness is in the red channel
    metallic = texture(metallicTexture, texCoords).r; 

    // Surface reflection at zero degress. Combine with albedo based on metal. Needed for fresnel calculation.
    F0 = vec3(0.04f, 0.04f, 0.04f);
    F0 = mix(F0, albedo, metallic);

    vec3 normViewDirection = normalize(viewDirection);
    
    // Setup the vectors needed for lighting calculations.
    halfDirection = normalize(normViewDirection + lightDir); 
    NdotH = max(0.0f, dot(bumpNormal, halfDirection));
    NdotV = max(0.0f, dot(bumpNormal, normViewDirection));
    NdotL = max(0.0f, dot(bumpNormal, lightDir));
    HdotV = max(0.0f, dot(halfDirection, normViewDirection));

    // GGX normal distribution calculation.
    roughnessSqr = roughness * roughness;
    roughSqr2 = roughnessSqr * roughnessSqr;
    NdotHSqr = NdotH * NdotH;
    denominator = (NdotHSqr * (roughSqr2 - 1.0f) + 1.0f);
    denominator = 3.14159265359f * (denominator * denominator);
    normalDistribution = roughSqr2 / denominator;

    // Schlick geometric shadow calculation.
    smithL = NdotL / (NdotL * (1.0f - roughnessSqr) + roughnessSqr);
    smithV = NdotV / (NdotV * (1.0f - roughnessSqr) + roughnessSqr);
    geometricShadow = smithL * smithV;

    // Fresnel shlick approximation for fresnel term calculation.
    fresnel = F0 + (1.0f - F0) * pow(1.0f - HdotV, 5.0f);

    // Now calculate the bidirectional reflectance distribution function.
    specularity = (normalDistribution * fresnel * geometricShadow) / ((4.0f * (NdotL * NdotV)) + 0.00001f);

    // Final light equation.
    color.rgb = albedo + specularity;
    color.rgb = color.rgb * NdotL;

    // Set the alpha to 1.0f.
    color = vec4(color.rgb, 1.0f);

   FragColor = color;
   

}
