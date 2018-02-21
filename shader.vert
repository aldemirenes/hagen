#version 410

// TODO
// transform normal and light position

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


float calculateHeight(vec2 textureCoord) {
  vec4 textureColor = texture(
    rgbTexture,
    vec2(1 - textureCoord.x / widthTexture, 1 - textureCoord.y / heightTexture)
  );
  float calculatedHeight = (0.2126 * textureColor.x  + 0.7152 * textureColor.y + 0.0722 * textureColor.z);
  return calculatedHeight * heightFactor;
}

vec3 faceNormal(vec3 v1, vec3 v2, vec3 v3) {
  v1.y = calculateHeight(v1.xz);
  v2.y = calculateHeight(v2.xz);
  v3.y = calculateHeight(v3.xz);

  vec3 crossRes = cross(v2-v1, v3-v1);
  return normalize(crossRes);
}

void main()
{
   vec3 lightPos = vec3(widthTexture/2, widthTexture + heightTexture, heightTexture/2);
   ToLightVector = normalize(lightPos - position);
   ToCameraVector = normalize(cameraPosition - position);

   textureCoordinate.x = 1 - position.x / widthTexture;
   textureCoordinate.y = 1 - position.z / heightTexture;

   // set gl_Position variable correctly to give the transformed vertex position
   gl_Position.xz = position.xz;

   // get texture value, compute height
   gl_Position.y = calculateHeight(position.xz);
   gl_Position.w = 1.0;
   gl_Position = MVP * gl_Position;

   vec3 tri[3];
   vec3 totalNormal = vec3(0, 0, 0);

   float x = position.x;
   float y = position.y;
   float z = position.z;

   // compute normal vector using also the heights of neighbor vertices
   if (x-1 >= 0 && z-1 >= 0) {
      tri[0].xz = vec2(x-1, z);
      tri[1].xz = vec2(x, z);
      tri[2].xz = vec2(x, z-1);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);
   }

   if (x-1 >=0 && z+1 <= heightTexture) {
      tri[0].xz = vec2(x-1, z+1);
      tri[1].xz = vec2(x, z);
      tri[2].xz = vec2(x-1, z);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);

      tri[0].xz = vec2(x-1, z+1);
      tri[1].xz = vec2(x, z+1);
      tri[2].xz = vec2(x, z);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);
   }

   if (x+1 <= widthTexture && z+1 <= heightTexture) {
      tri[0].xz = vec2(x, z+1);
      tri[1].xz = vec2(x+1, z);
      tri[2].xz = vec2(x, z);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);
   }

   if (x+1 <= widthTexture && z-1 >= 0) {
      tri[0].xz = vec2(x, z);
      tri[1].xz = vec2(x+1, z);
      tri[2].xz = vec2(x+1, z-1);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);

      tri[0].xz = vec2(x, z);
      tri[1].xz = vec2(x+1, z-1);
      tri[2].xz = vec2(x, z-1);

      totalNormal += faceNormal(tri[0], tri[1], tri[2]);
   }

   vertexNormal = normalize(totalNormal);
}
