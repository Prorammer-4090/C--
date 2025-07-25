#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	// note that we read the multiplication from right to left
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoords = vec2(aTexCoord.x, aTexCoord.y);
	FragPos = vec3(model * vec4(aPos, 1.0));
	
	// Transform normals to world space using the normal matrix
	// The normal matrix is the transpose of the inverse of the upper-left 3x3 of the model matrix
	Normal = mat3(transpose(inverse(model))) * aNormal;
}