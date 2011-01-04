
#include "World.h"
#include "Simplex.h"

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

World::World(size_t size)
   : m_size(size)
{
   m_cells.resize(m_size * m_size * m_size, AIR);
}

World::~World()
{
}

void World::randomize(size_t numberOfLivingCells)
{
   assert(numberOfLivingCells < m_cells.size());
   
   while (numberOfLivingCells > 0)
   {
      size_t i = ci::Rand::randInt(m_cells.size());
      if (m_cells[i] == AIR)
      {
         m_cells[i] = DIRT;
         numberOfLivingCells--;
      }
   }
}

void World::buildFloatingMountain()
{
   float caves, center_falloff, plateau_falloff, density;
   
   for (int z = 0; z < m_size; ++z)
      for (int y = 0; y < m_size; ++y)
         for (int x = 0; x < m_size; ++x)
         {
            float xf = x / (float)m_size;
            float yf = y / (float)m_size;
            float zf = z / (float)m_size;
            
            if (yf <= 0.8)
               plateau_falloff = 1.0;
            else if (0.8 < yf && yf < 0.9)
               plateau_falloff = 1.0 - (yf - 0.8) * 10.0;
            else
               plateau_falloff = 0.0;
               
            center_falloff = 0.1 / (pow((xf - 0.5) * 1.5, 2) +
                                    pow((yf - 1.0) * 0.8, 2) +
                                    pow((zf - 0.5) * 1.5, 2));
                                    
            caves = pow(simplexNoise(1, xf * 5, yf * 5, zf * 5), 3);
            density = simplexNoise(5, xf, yf * 0.5, zf) * center_falloff * plateau_falloff;
            density *= pow(noise((xf + 1) * 3.0, (yf + 1) * 3.0, (zf + 1) * 3.0) + 0.4, 1.8);
            //if(caves < 0.5)
            //   density = 0;

            set(x, y, z, density > 3.1 ? DIRT : AIR);
         }
}

void World::simpleRender(ci::Color8u const& color)
{
   using namespace ci;
   
   float offset = m_size * -0.5f;
   
   gl::color(color);
   
   for (size_t z = 0; z < m_size; ++z)
      for (size_t y = 0; y < m_size; ++y)
         for (size_t x = 0; x < m_size; ++x)
            if (cell(x, y, z) == DIRT)
               gl::drawCube(Vec3f(offset, offset, offset) + Vec3f(x, y, z), Vec3f(1.0f, 1.0f, 1.0f));
}

void World::generateMesh(ci::TriMesh & mesh)
{
   for (size_t z = 0; z < m_size; ++z)
      for (size_t y = 0; y < m_size; ++y)
         for (size_t x = 0; x < m_size; ++x)
         {
            if (cell(x, y, z) == AIR)
            {
               for (size_t i = 0; i < 6; ++i)
               {
                  if (int index = getNeigbour(x, y, z, i) != -1)
                  {
                  }
               }
            }
         }
}

