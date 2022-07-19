#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 texCoords;

// transformation matrices
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Perspective;
uniform mat4 dirLightSpaceMatrix;

out VsOutFsIn {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	// the position of the fragment in relation to the directional depthMap
	vec4 fragPosDirLightSpace;
} vs_out;

void main() {
	//-- Convert position and normal to Eye-Space:
	vs_out.fragPos = vec3(Model * vec4(position, 1.0));
	vs_out.normal = normalize(mat3(transpose(inverse(Model))) * normal);
	vs_out.texCoords = texCoords;
	vs_out.fragPosDirLightSpace = dirLightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
	
	gl_Position = Perspective * View * Model * vec4(position, 1.0);
}
