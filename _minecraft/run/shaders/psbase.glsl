/* Fragment shader */
varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float ambientLevel;

// gl_FragCoord.z <- profondeur du pixel

const vec4 sun_color = vec4(1.f, 0.7f, 0, 1.f);
const vec4 moon_color = vec4(1.f,1.f,1.f,1.f);

const vec4 sunny_cloud_color = vec4(0.9f, 0.9f, 0.9f, 1.f);	// Define the cloud color!
const vec4 stormy_cloud_color = vec4(0.7f,0.7f,0.7f,1.f);	

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	
	// Calculating The Diffuse Term And Clamping It To [0;1]
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

	// Ignore the sun and moon and stars
	if (color != sun_color && color != moon_color && color != sunny_cloud_color && color != stormy_cloud_color)
		// Calculating The Final Color
		gl_FragColor = color * (DiffuseTerm * (1 - ambientLevel) + ambientLevel);
	else
		gl_FragColor = color;

	gl_FragColor.a = 1;
	gl_FragDepth = gl_FragCoord.z;
}