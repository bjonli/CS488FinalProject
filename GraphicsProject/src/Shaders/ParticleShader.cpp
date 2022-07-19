#include "ParticleShader.hpp"
#include "../Application/CS488Window.hpp"
#include "../Application/GlErrorCheck.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

Particle::Particle() 
    : pos(glm::vec3(0)), col(glm::vec4(0)), 
    fadeRate(glm::vec4(0)), velocity(glm::vec3(0)), life(0), size(0) {}

Particle::Particle(glm::vec3 p, glm::vec4 c, glm::vec4 f, glm::vec3 v, float s, float l) 
    : pos(p), col(c), fadeRate(f), velocity(v), life(l), size(s) {}

void Particle::tick(float timeElapsed) {
    if (life > 0) {
        pos += velocity*timeElapsed;
        col += fadeRate*timeElapsed;
        life -= timeElapsed;
    }
}

// if particles is dead, set distance to be negative, that way, after sorting, all dead 
// particles will be put at the end of the buffer
void Particle::updateCamDistance(glm::vec3 camPos) {
    if (life <= 0) { camDistance = -1; }
    else { camDistance = glm::length2(camPos-pos); }
}

bool Particle::operator<(const Particle& other) const {
    return camDistance > other.camDistance;
}

ParticleShader::ParticleShader(unsigned int maxParticles_) : 
	maxParticles(maxParticles_), isEnabled(true)
{
    particles.resize(maxParticles);
    tempColours.resize(maxParticles);
    tempOffsets.resize(maxParticles);

    generateProgramObject();
	attachVertexShader( "Particle.vs" );
	attachFragmentShader("Particle.fs" );
	link();
}

void ParticleShader::initData() {
    // Generate VBO and store particle vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint vbo_particle;
	glGenBuffers( 1, &vbo_particle);
    glBindBuffer( GL_ARRAY_BUFFER, vbo_particle);
    glBufferData( GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW );
    GLint posLocation = getAttribLocation("pos");
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO for offset+size
    glGenBuffers(1, &vbo_positions);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
    GLint offsetLocation = getAttribLocation("offset");
    glEnableVertexAttribArray(offsetLocation);
    glVertexAttribPointer(offsetLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(offsetLocation, 1);
    CHECK_GL_ERRORS;

    // VBO for color
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    GLint colorLocation = getAttribLocation("color");
    glEnableVertexAttribArray(colorLocation);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(colorLocation, 1);
    CHECK_GL_ERRORS;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// these we should not expect to change anytime soon
void ParticleShader::loadUniforms(glm::mat4& P) {
    enable();
    {
        // Perspective matrix
        glUniformMatrix4fv(getUniformLocation("Perspective"), 1, GL_FALSE, value_ptr(P));
        CHECK_GL_ERRORS;
    }
    disable();
}

void ParticleShader::tick(float timeElapsed) {
    for (int i=0; i<maxParticles; i++) {
        particles[i].tick(timeElapsed);
    }
}

void ParticleShader::addParticle(glm::vec3 p, glm::vec4 c, glm::vec4 f, glm::vec3 v, float s, float l) {
    // reached max particles, do not spawn another
    if (!isEnabled || newParticleIndex < 0 || particles[newParticleIndex].life > 0) { return; }
    // otherwise, add a particle
    particles[newParticleIndex] = Particle(p, c, f, v, s, l);
    newParticleIndex--;
}

void ParticleShader::drawScene(glm::mat4 V, glm::vec3 viewPos) {
	// leave early if not enabled
	if (!isEnabled) { return;  }
	// sort all particles and count the number of live particles
    for (int i=0; i<maxParticles; i++) { particles[i].updateCamDistance(viewPos);}
    std::sort(particles.begin(), particles.end());
    // reset the index now that particles are sorted so that all free space is at the end
    newParticleIndex = maxParticles-1;
    // copy offsets and colours into temp buffer so we can copy to vbo
    int numParticles = 0;
    for (; numParticles<maxParticles && particles[numParticles].life > 0; numParticles++) {
        tempOffsets[numParticles] = glm::vec4(
            particles[numParticles].pos, particles[numParticles].size);
        tempColours[numParticles] = particles[numParticles].col;
    }
    
    if (numParticles == 0) { return; }                   // if no particles, end early
    // draw the actual particles
	glBindVertexArray(vao);
    enable();
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        GLint location = getUniformLocation("View");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(V));
        location = getUniformLocation("viewPos");
        glUniform3fv(location, 1, value_ptr(viewPos));
        CHECK_GL_ERRORS;

        glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
        glBufferData(GL_ARRAY_BUFFER, 
            numParticles * sizeof(glm::vec4), &tempOffsets[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
        glBufferData(GL_ARRAY_BUFFER, 
            numParticles * sizeof(glm::vec4), &tempColours[0], GL_STATIC_DRAW);            
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles); 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    disable();
    glBindVertexArray(0);
}

bool ParticleShader::getIsEnabled() { return isEnabled; }

void ParticleShader::setIsEnabled(bool b) { isEnabled = b; }