/* Vertex shader */
varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float elapsed;
uniform mat4 invertView;

const float water_var = 4.f;
const float affine = 0.70f;

// Color definition
const vec4 water_color = vec4(0,(162.f/255.f),float(232.f/255.f),1);
const vec4 sunny_cloud_color = vec4(0.9, 0.9, 0.9, 1);	// Define the cloud color!
const vec4 stormy_cloud_color = vec4(0.7,0.7,0.7,1);	

void main()
{
	// Transforming The Vertex
	vec4 position = vec4(gl_Vertex);
	vec4 new_color = vec4(gl_Color);

	// Get the color of the vertex : if it's blue, this is the water
	if (gl_Color.b == water_color.b)
	{
		// Apply on water
		position.z = gl_Vertex.z + sin(elapsed + ((gl_Vertex.x  / 8.48836))) * water_var;
		//new_color.a = 0.2;
	}
	/*else if (gl_Color == sunny_cloud_color || gl_Color == stormy_cloud_color)
	{
		new_color.a = 0.2;
	}*/
	
	gl_Position = gl_ModelViewProjectionMatrix * position;

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal;	// Transfert the normal to the pshader

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);

	//Couleur
	color = new_color;
}