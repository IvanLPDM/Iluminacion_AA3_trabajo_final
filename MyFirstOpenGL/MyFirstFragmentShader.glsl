#version 440 core

uniform sampler2D textureSampler;
uniform vec3 color;
uniform vec3 lightPosition;      // Posici�n del sol
uniform vec3 moonPosition;       // Posici�n de la luna
uniform vec3 cameraPosition;     // Posici�n de la c�mara (y la luz)
uniform vec3 cameraFront;
uniform bool flashlightOn;

uniform float outerConeAngle;
uniform float innerConeAngle;

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

        if(lightPosition.y < 0.5)
        {
            ambientColor = vec4(0.5, 0.2, 0.1, 0.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if(lightPosition.y < 1)
        {
            ambientColor = vec4(0.8, 0.4, 0.1, 0.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if(lightPosition.y > 1)
        {
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

        if(lightPosition.y > -0.5)
        {
            ambientColor = vec4(0.0, 0.1, 0.4, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if(lightPosition.y > -1)
        {
            ambientColor = vec4(0.1, 0.2, 0.8, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }
        if(lightPosition.y < -1)
        {
            ambientColor = vec4(0.1, 0.2, 0.6, 1.0);
            finalColor += baseColor.rgb * ambientColor.rgb;
        }

        

        finalColor += baseColor.rgb * moonLightAngle;
    }


    // Luz de la c�mara (linterna)
    if(flashlightOn)
    {
        vec3 camDir = normalize(cameraPosition - primitivePosition.xyz);
        vec3 camLightDir = normalize(cameraPosition + cameraFront - primitivePosition.xyz); // Direcci�n hacia donde mira la c�mara
        float camDiff = max(dot(normalsFragmentShader, camLightDir), 0.0);
        vec3 camLight = camDiff * vec3(1.0, 1.0, 1.0); // Ajusta la intensidad de la luz de la c�mara seg�n necesites

        // Calcular la distancia desde la c�mara hasta el fragmento
            float distance = length(cameraPosition - primitivePosition.xyz);
            // Calcular la atenuaci�n cuadr�tica de la luz por la distancia
            float attenuation = 1.0 / (1.0 + 0.8 * distance + 0.1 * distance * distance);
            // Sumar la contribuci�n de la luz al color final
            finalColor += baseColor.rgb * attenuation;
    }
    



    fragColor = vec4(finalColor, baseColor.a);
}