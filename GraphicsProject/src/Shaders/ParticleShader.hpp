#pragma once
#include "ShaderProgram.hpp"
#include <glm/glm.hpp>
#include <vector>

class Particle {
    public:
        Particle();
        Particle(glm::vec3 p, glm::vec4 c, glm::vec4 f, glm::vec3 v, float s, float l);
        
		glm::vec3 pos;
        glm::vec4 col;
        glm::vec4 fadeRate;
        glm::vec3 velocity;
        float size;
        float life;
        
        void tick(float timeElapsed);
        
        float camDistance;								// usedForSorting
        void updateCamDistance(glm::vec3 camPos);
        bool operator<(const Particle& other) const;	// sort by distance
};

// particle manager and particle shader
class ParticleShader : public ShaderProgram {
    unsigned int maxParticles;
    std::vector<Particle> particles;
    std::vector<glm::vec4> tempOffsets;
    std::vector<glm::vec4> tempColours;
    int newParticleIndex;                   // used for faster addition of new particles

    GLuint vao;
    GLuint vbo_positions;
    GLuint vbo_colors;
	bool isEnabled;

    public:
        ParticleShader(unsigned int maxParticles_); 
        void initData();
        void loadUniforms(glm::mat4& P);

        void addParticle(glm::vec3 p, glm::vec4 c, glm::vec4 f, glm::vec3 v, float s, float l);
        void tick(float timeElapsed);
        void drawScene(glm::mat4 V, glm::vec3 viewPos);

		bool getIsEnabled();
		void setIsEnabled(bool b);
};
