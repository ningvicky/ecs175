// positions are 4 component vectors (opengl will fill in the 4th component with 1)
attribute vec4 position;  //input
attribute vec3 normal;

varying vec3 transformed_normal;  //output
varying vec3 light_direction;
varying vec3 eye_vec;
varying vec3 spot_direction;

uniform mat4 proj_mat;      //input
uniform mat4 view_mat;      //update more often
uniform mat4 model_mat;
uniform mat3 normal_mat;

uniform vec4 material_color;
uniform vec3 spot_dir;
uniform vec3 spot_pos;

void main()
{
  //output the position for the fragment shader
  gl_Position = proj_mat * view_mat * model_mat * position;

  //output the color for the fragment shader
  gl_FrontColor = material_color;

  //output the transformed normal
  transformed_normal = normal_mat*normal;

  //vector from the eye to the position in world space
  eye_vec = -vec3(view_mat * model_mat * position);

  //transform the light to view space
  light_direction = vec3(view_mat * vec4(spot_pos,1))+ eye_vec;
  spot_direction = vec3(view_mat * normalize(vec4(spot_dir,0.0)));
}
