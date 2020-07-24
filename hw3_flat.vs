#version 120
uniform mat4 MVP;
attribute vec3 vPos;
attribute vec2 vTexCoord;
attribute vec3 vnor;
uniform float hmult;
varying vec2 texture_coordinate;
varying vec3 fragVert;
varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 lightPos4;

void main()
{
	gl_Position = MVP*vec4(vPos[0], hmult*vPos[1] , vPos[2], 1.0);

	texture_coordinate = vTexCoord;
/*
	//mat4 temp = linmath::mat4x4_transpose(linmath::mat4x4_invert(MVP));
	mat3 normalMatrix;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			normalMatrix[i][j] = MVP[i][j];
		}
	}
	//mat3 normalMatrix = mat3x3_transpose(inverse(mat3(MVP)));
	normalMatrix = normalMatrix * vnor;

	float norm = sqrt(pow(normalMatrix[0],2)+pow(normalMatrix[1],2)+pow(normalMatrix[2],2));
	vec3 normal;
	if(norm!=0){
		normal[0] = normalMatrix[0]/norm;
		normal[1] = normalMatrix[1]/norm;
		normal[2] = normalMatrix[2]/norm;
	}
	else{
		normal[0] = normalMatrix[0];
		normal[1] = normalMatrix[1];
		normal[2] = normalMatrix[2];
	}
	*/
	//vec3 normal = linmath::vec3_norm(normalMatrix, normalMatrix);
	
	fragTexCoord = vTexCoord;
    fragNormal = vnor;
    fragVert = vPos;
	vec4 lightPos4;
	lightPos4[0]=400;
	lightPos4[1]=1000;
	lightPos4[2]=400;
	lightPos4[3]=0;
	lightPos4 = MVP*lightPos4;

}
