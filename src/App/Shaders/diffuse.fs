#version 330 core

struct DirectionalLight 
{
	vec3 direction;

	vec3 color;
	float ambientStrength;
	float specularStrength;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

	vec3 color;
	float ambientStrength;
	float specularStrength;
};

uniform sampler2D albedoTexture;
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;
uniform vec3 viewDir;


in vec3 Pos;
in vec3 Normal;
in vec2 UV;

out vec4 FragColor;

vec3 CalculateDirectionalLighting(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return light.color * (albedo * (light.ambientStrength + diff) + light.specularStrength * spec);
}

vec3 CalculateSpotLighting(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0f + 1.0f * dist + 1.0f * (dist * dist));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return attenuation * intensity * light.color * (albedo * (light.ambientStrength + diff) + light.specularStrength * spec);
}

void main()
{
	vec3 albedo = texture(albedoTexture, UV).rgb;

	vec3 directionalLightResult = CalculateDirectionalLighting(directionalLight, Normal, viewDir, albedo);
	vec3 spotLightResult = CalculateSpotLighting(spotLight, Normal, Pos, viewDir, albedo);

    FragColor = vec4(directionalLightResult + spotLightResult, 1.0);
}