
uniform sampler2D diffuse;
uniform vec2 scroll;
varying vec2 texCoordVar;

void main() {
    gl_FragColor = texture2D(diffuse, texCoordVar + scroll);
}