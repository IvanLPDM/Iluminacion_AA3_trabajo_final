#version 440 core

uniform sampler2D textureSampler;
uniform vec3 lightPosition;
uniform vec3 moonPosition;
uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform bool flashlightOn;

uniform float outerConeAngle;
uniform float innerConeAngle;
uniform float maxDistance;

uniform vec3 flashlightColor;

in vec2 uvsFragmentShader;
in vec3 normalsFragmentShader;
in vec4 primitivePosition; 

out vec4 fragColor;

void main() {
    vec2 adjustedTexCoord = vec2(uvsFragmentShader.x, 1.0 - uvsFragmentShader.y);
    vec4 baseColor = texture(textureSampler, adjustedTexCoord);

    vec4 ambientColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 finalColor = baseColor.rgb * ambientColor.rgb;

    // Luz del sol
    if (lightPosition.y > 0.0) {
        vec3 lightDirection = normalize(lightPosition - primitivePosition.xyz);
        float sourceLightAngle = max(dot(normalsFragmentShader, lightDirection), 0.0);

        vec3 sunColor = mix(vec3(0.8, 0.4, 0.1), vec3(0.1, 0.2, 0.8), clamp(lightPosition.y, 0.0, 1.0));

        ambientColor = vec4(sunColor * 0.9, 1.0);
        finalColor += baseColor.rgb * ambientColor.rgb;
        finalColor += baseColor.rgb * sourceLightAngle * sunColor;
    }
    // Luz de la luna
    else {
        vec3 moonDirection = normalize(moonPosition - primitivePosition.xyz);
        float moonLightAngle = max(dot(normalsFragmentShader, moonDirection), 0.0);

        vec3 moonColor = mix(vec3(0.1, 0.2, 0.8), vec3(0.8, 0.4, 0.1), clamp(-lightPosition.y, 0.0, 1.0));

        ambientColor = vec4(moonColor * 1.4, 1.0);
        finalColor += baseColor.rgb * ambientColor.rgb;
        finalColor += baseColor.rgb * moonLightAngle * moonColor;
    }

    // Luz de la linterna
    if (flashlightOn) {
        vec3 lightDir = normalize(cameraFront);
        vec3 toFragment = normalize(primitivePosition.xyz - cameraPosition);
        float theta = dot(lightDir, toFragment);

        float epsilon = innerConeAngle - outerConeAngle;
        float intensity = clamp((theta - outerConeAngle) / epsilon, 0.0, 1.0);

        float distance = length(cameraPosition - primitivePosition.xyz);
        float attenuation = 1.0 / (distance * distance);
        attenuation *= (1.0 - smoothstep(0.0, maxDistance, distance));

        vec3 diffuse = flashlightColor * max(dot(normalsFragmentShader, toFragment), 0.0);
        finalColor += baseColor.rgb * diffuse * attenuation * intensity;
    }

    fragColor = vec4(finalColor, baseColor.a);
}