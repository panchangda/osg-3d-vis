#version 330 core

in vec4 v_color;
out vec4 FragColor;


void main(){
    FragColor = v_color;
	// FragColor = vec4(v_color.rgb, 1.0);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
