#ifndef SHADERS_H
#define SHADERS_H


#ifdef WIN32
#include <windows.h>
#include <Microcosm/extensions.h>
#endif
#include <OpenGL/OpenGL.h>
#include <string>


const std::string liquidVertSource(
"varying vec4 vsPosition;\n"
"varying vec3 vsNormal;\n"
"varying vec3 vsLight0Vec;\n"
"varying vec3 vsLight1Vec;\n"
"varying vec3 vsEyeVec;\n"
"varying vec4 gl_TexCoord[1];\n"
"uniform vec3 texcoordmix;\n"
"void main(void){\n"
"	vsNormal = gl_NormalMatrix * gl_Normal;\n"
"	vsLight0Vec = gl_LightSource[0].position.xyz;\n"
"	vsLight1Vec = gl_LightSource[1].position.xyz;\n"
"	vsPosition = gl_ModelViewMatrix * gl_Vertex;\n"
"	vsEyeVec = -vsPosition.xyz;\n"
"	vec3 vsNormalNormalized = normalize(vsNormal);\n"
"	gl_TexCoord[0].xyz = vec3(mix(length(gl_Vertex.xyz), gl_Vertex.x + gl_Vertex.y + gl_Vertex.z + 2.0, texcoordmix.x),\n"
"	mix(dot(normalize(-vsPosition.xyz), vsNormalNormalized) * 0.5 + 0.49, vsNormalNormalized.x * 0.49 + 0.5, texcoordmix.y),\n"
"	texcoordmix.z);\n"
"	gl_Position = ftransform();\n"
"	//gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n"
"}\n"
);

const std::string liquidFragSource(
"varying vec4 vsPosition;\n"
"varying vec3 vsNormal;\n"
"varying vec3 vsLight0Vec;\n"
"varying vec3 vsLight1Vec;\n"
"varying vec3 vsEyeVec;\n"
"varying vec4 gl_TexCoord[1];\n"
"uniform sampler1D diffusetex;\n"
"vec4 diffuse;\n"
"vec3 specular;\n"
"// Should just have one light function and pass the light index as a variable.  Using a constant\n"
"// index to gl_LightSource works around a problem in legacy ATI drivers.\n"
"void directionalLight0(in vec3 lightvec, in vec3 normal, in vec3 eyevec){\n"
"	// diffuse\n"
"	float norm_dot_dir = max(0.0, dot(normal, lightvec));\n"
"	diffuse.rgb += max(gl_LightSource[0].diffuse.rgb * norm_dot_dir, gl_LightSource[0].ambient.rgb);\n"
"	if(norm_dot_dir > 0.0){\n"
"		// blinn specular\n"
"		//vec3 halfvec = normalize(eyevec + lightvec);\n"
"		//specular += gl_LightSource[index].specular.rgb * pow(max(dot(normal, halfvec), 0.0), 50.0);\n"
"		// phong specular\n"
"		//vec3 reflectvec = (normal * (2.0 * norm_dot_dir)) - lightvec;\n"
"		vec3 reflectvec = reflect(-lightvec, normal);\n"
"		specular += gl_LightSource[0].specular.rgb * pow(max(dot(eyevec, reflectvec), 0.0), 50.0);\n"
"	}\n"
"}\n"
"void directionalLight1(in vec3 lightvec, in vec3 normal, in vec3 eyevec){\n"
"	float norm_dot_dir = max(0.0, dot(normal, lightvec));\n"
"	diffuse.rgb += max(gl_LightSource[1].diffuse.rgb * norm_dot_dir, gl_LightSource[1].ambient.rgb);\n"
"	if(norm_dot_dir > 0.0){\n"
"		vec3 reflectvec = reflect(-lightvec, normal);\n"
"		specular += gl_LightSource[1].specular.rgb * pow(max(dot(eyevec, reflectvec), 0.0), 50.0);\n"
"	}\n"
"}\n"
"void main(void){\n"
"	diffuse = vec4(0.0, 0.0, 0.0, 1.0);\n"
"	specular = vec3(0.0, 0.0, 0.0);\n"
"	vec3 vsNormalNormalized = normalize(vsNormal);\n"
"	vec3 vsEyeVecNormalized = normalize(vsEyeVec);\n"
"	directionalLight0(normalize(vsLight0Vec), vsNormalNormalized, vsEyeVecNormalized);\n"
"	directionalLight1(normalize(vsLight1Vec), vsNormalNormalized, vsEyeVecNormalized);\n"
"	vec4 texcol0 = texture1D(diffusetex, gl_TexCoord[0].x);\n"
"	vec4 texcol1 = texture1D(diffusetex, gl_TexCoord[0].y);\n"
"	diffuse *= mix(texcol0, texcol1, gl_TexCoord[0].z);\n"
"	// pre-multiplied alpha\n"
"	diffuse.rgb *= diffuse.a;\n"
"	// smart specular addition\n"
"	diffuse.rgb += specular * (vec3(1.0, 1.0, 1.0) - diffuse.rgb);\n"
"	diffuse = mix(gl_Fog.color, diffuse, clamp((gl_Fog.end - length(vsPosition.xyz)) * gl_Fog.scale, 0.0, 1.0));\n" 
"	gl_FragColor = diffuse;\n"
"}\n"
);


GLhandleARB shaderProgram;
GLint texCoordMixUniform;


void initShaders(){
	GLhandleARB vertShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	const GLchar* vertString = liquidVertSource.c_str();
	glShaderSourceARB(vertShader, 1, &vertString, NULL);
	glCompileShaderARB(vertShader);

	GLhandleARB fragShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	const GLchar* fragString = liquidFragSource.c_str();
	glShaderSourceARB(fragShader, 1, &fragString, NULL);
	glCompileShaderARB(fragShader);

	shaderProgram = glCreateProgramObjectARB();
	glAttachObjectARB(shaderProgram, vertShader);
	glAttachObjectARB(shaderProgram, fragShader);
	glLinkProgramARB(shaderProgram);

	glUseProgramObjectARB(shaderProgram);
	GLint loc = glGetUniformLocationARB(shaderProgram, "diffusetex");
	glUniform1iARB(loc, 0);
	texCoordMixUniform = glGetUniformLocationARB(shaderProgram, "texcoordmix");
}



#endif
