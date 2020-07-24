#version 120

varying vec2 texture_coordinate;

uniform sampler2D texturemap;



varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragVert;
varying vec4 lightPos4;

void main()
{
   int dist;
	
    vec4 cx = texture2D(texturemap, texture_coordinate);
   vec3 position;
   vec3 intensities;
   position[0] = lightPos4[0];
   position[1] = lightPos4[1];
   position[2] = lightPos4[2];
   intensities[0] = 1.0;
   intensities[1] = 1.0;
   intensities[2] = 1.0;

    	
   vec3 ambientLightColor ;
   ambientLightColor[0]=0.3;
   ambientLightColor[1]=0.3;
   ambientLightColor[2]=0.3;
   
   float ambientStrength = 0.25;
   vec3 ambient = ambientStrength * ambientLightColor;
   vec4 amb2 = vec4(ambient, 1.0);
   vec4 result = amb2 + cx;
   
   
   

   //
    
    //calculate the location of this fragment (pixel) in world coordinates
    //vec3 fragPosition = vec3(model * vec4(fragVert, 1));
    
    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = position - fragVert;

    //calculate the cosine of the angle of incidence
    float brightness = dot(fragNormal, surfaceToLight) / (length(surfaceToLight) * length(fragNormal));
    brightness = clamp(brightness, 0, 1);
    brightness = 1;
    //calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)
    //vec4 surfaceColor = texture(tex, fragTexCoord);
    //result += vec4(brightness * intensities * cx.rgb, cx.a);
    cx[0] *= intensities[0] * brightness;
    cx[1] *= intensities[1] * brightness;
    cx[2] *= intensities[2] * brightness;
    result += cx;

    gl_FragColor = result;
}
