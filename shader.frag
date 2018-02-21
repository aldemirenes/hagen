#version 410

// Output Color
out vec4 color;

uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;

// Texture-related data;
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// Data from Vertex Shader
in vec2 textureCoordinate;
in vec3 vertexNormal; // For Lighting computation
in vec3 ToLightVector; // Vector from Vertex to Light;
in vec3 ToCameraVector; // Vector from Vertex to Camera;

void main() {

  // Assignment Constants below
  // get the texture color
  vec4 textureColor = texture(rgbTexture, textureCoordinate);

  // apply Phong shading by using the following parameters
  vec3 ka = vec3(0.25,0.25,0.25); // reflectance coeff. for ambient
  vec3 Ia = vec3(0.3,0.3,0.3); // light color for ambient
  vec3 Id = vec3(1.0, 1.0, 1.0); // light color for diffuse
  vec3 kd = vec3(1.0, 1.0, 1.0); // reflectance coeff. for diffuse
  vec3 Is = vec3(1.0, 1.0, 1.0); // light color for specular
  vec3 ks = vec3(1.0, 1.0, 1.0); // reflectance coeff. for specular
  int specExp = 100; // specular exponent

  vec3 L = ToLightVector;
  vec3 V = ToCameraVector;
  vec3 H = normalize(L + V);
  vec3 N = vertexNormal;

  float NdotL = dot(N, L); // for diffuse component
  float NdotH = dot(N, H); // for specular component

  vec3 diffuseColor = Id * kd * max(0, NdotL);
  vec3 specularColor = Is * ks * pow(max(0, NdotH), specExp);
  vec3 ambientColor = Ia * ka;

  // compute ambient component
  vec4 ambient = vec4(ambientColor, 1);
  // compute diffuse component
  vec4 diffuse = vec4(diffuseColor, 1);
  // compute specular component
  vec4 specular = vec4(specularColor, 1);

  // compute the color using the following equation
  color = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
  //color = vec4(clamp( textureColor.xyz * vec3(specular), 0.0, 1.0), 1.0);

  //color = vec4(clamp(textureColor.xyz, 0.0, 1.0), 1.0);
}
