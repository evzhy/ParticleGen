#pragma once

#include "ParticleGenTypes.h"
#include <vector>

/* two such structures per particle calculation thread:
   - first for reading from calculation thread and render (main) thread
   - second for writing from calculation thread only
   - every calculation thread switches its pair at end of each calculation cycle, except when rendering is in progress
*/
class ParticleData
{
public:
	std::vector<gen::Vec3> data;	  // particle points from begin, new effect coords from end
	std::size_t usedVerticesCount{0}; // actually used renderable vertices from beginning of array

	ParticleData( const ParticleData& other ) = delete;
	void operator=( const ParticleData& other ) = delete;

	ParticleData( const ParticleData&& other )
		: data( std::move( other.data ) ), usedVerticesCount( other.usedVerticesCount )
	{
	}
	void operator=( const ParticleData&& other )
	{
		data = std::move( other.data );
		usedVerticesCount = other.usedVerticesCount;
	}

	static auto Make( std::size_t bufferSize ) -> ParticleData&&
	{
		return std::move( ParticleData( bufferSize ) );
	}

protected:
	ParticleData( std::size_t bufferSize )
	{
		data.resize( bufferSize );
	}
};

/* utility class for rendering phase in main thread
   - sets rendering atomic to true in constructor, sets to false in destructor
   - gets references for passive (ready for rendering) ParticleData of each thread
*/
class IRenderData
{
public:
	virtual ~IRenderData( ) = default;

	virtual auto GetDataBuffersCount( ) -> std::size_t = 0;
	virtual auto GetParticleData( std::size_t index ) -> ParticleData& = 0; // get renderable ParticleData of certain thread
};
