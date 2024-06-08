#version 440 core

uniform sampler2D textureSampler;
uniform vec3 color;
uniform vec3 lightPosition;      // Posición del sol
uniform vec3 moonPosition;       // Posición de la luna
uniform vec3 cameraPosition;     // Posición de la cámara (y la luz)
uniform vec3 cameraFront;
uniform bool flashlightOn;

uniform float outerConeAngle;
uniform float innerConeAngle;
uniform float maxDistance;       // Distancia máxima de la linterna

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
    if (lightPosition.y > 0.0) 
    {
        vec3 lightDirection = normalize(lightPosition - primitivePosition.xyz);
        float sourceLightAngle = max(dot(normalsFragmentShader, lightDirection), 0.0);

        if (lightPosition.y < 0.5) {
            ambientColor = vec4(0.5, 0.2, 0.1, 0.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if (lightPosition.y < 1) {
            ambientColor = vec4(0.8, 0.4, 0.1, 0.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if (lightPosition.y > 1) {
            ambientColor = vec4(1.2, 0.8, 0.3, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }

        finalColor += baseColor.rgb * sourceLightAngle;
    }

    // Luz de la luna
    else
    {
        vec3 moonDirection = normalize(moonPosition - primitivePosition.xyz);
        float moonLightAngle = max(dot(normalsFragmentShader, moonDirection), 0.0);

        if (lightPosition.y > -0.5) {
            ambientColor = vec4(0.0, 0.1, 0.4, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if (lightPosition.y > -1) {
            ambientColor = vec4(0.1, 0.2, 0.8, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if (lightPosition.y < -1) {
            ambientColor = vec4(0.1, 0.2, 0.6, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }

        finalColor += baseColor.rgb * moonLightAngle;
    }

    // Luz de la cámara (linterna)
    if (flashlightOn) {
        vec3 flashlightDir = normalize(cameraFront);
        vec3 toFragment = normalize(primitivePosition.xyz - cameraPosition);
        float distance = length(cameraPosition - primitivePosition.xyz);

        if (distance <= maxDistance) {
            float cosTheta = dot(toFragment, flashlightDir);
            float cosInner = cos(radians(innerConeAngle));
            float cosOuter = cos(radians(outerConeAngle));

            if (cosTheta > cosOuter) {
                // Calcula la intensidad en función de la posición dentro del cono
                float intensity;
                if (cosTheta > cosInner) {
                    intensity = 1.0; // Máxima intensidad en el cono interno
                } else {
                    intensity = smoothstep(cosOuter, cosInner, cosTheta); // Disminuye la intensidad hacia el borde del cono externo
                }
                // Atenuación de la intensidad por la distancia al fragmento
                float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
                // Aplicar iluminación puntual en el fragmento
                vec3 diffuse = baseColor.rgb * intensity * attenuation;
                finalColor += diffuse;
            }
        }
    }

    fragColor = vec4(finalColor, baseColor.a);
}