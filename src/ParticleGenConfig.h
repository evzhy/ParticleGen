#pragma once

#define IMPL_GLUT

#ifdef IMPL_GLUT
#define IMPL_OPENGL
#endif

const unsigned int gWindowWidth = 640;
const unsigned int gWindowHeight = 480;
const unsigned int gRedrawInterval = 30; // main thread sleep between frames, milliseconds

const char* const gWindowTitle = "Particle Generator";

const unsigned int gMaxParticleEffects = 2048;
const unsigned int gMaxParticlesPerEffect = 64;

const unsigned int gNumberOfGenerationThreads = 2;

const bool gDestroyOffscreenParticles = true;
const float gParticleLifetime = 2.f;
const float gParticleExplodePercent = 10.f; // 0..100
const float gParticleInitialSpeed = 50.f;   // 2D vector in pixels per second
const float gParticleDecceleratoin = 40.f;  // percent of current speed, per second
const float gParticleGravityAccel = 1.f;	// in pixels per second

decltype( gMaxParticleEffects ) gMaxParticleTotal = gMaxParticleEffects * gMaxParticlesPerEffect;
