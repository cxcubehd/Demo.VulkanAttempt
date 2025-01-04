@echo off
:: Compile fragment.glsl as frag and vertex.glsl as vert to spv using naga
naga ./assets/shaders/vertex.glsl ./assets/shaders/vertex.spv --shader-stage vert
naga ./assets/shaders/fragment.glsl ./assets/shaders/fragment.spv --shader-stage frag
