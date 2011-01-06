
#include "World.h"
#include "Simplex.h"

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/app/App.h"

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
   using namespace ci;
   
   Vec3f offset = Vec3f(m_size * -0.5f, m_size * -0.5f, m_size * -0.5f);
   
   for (size_t z = 0; z < m_size; ++z)
      for (size_t y = 0; y < m_size; ++y)
         for (size_t x = 0; x < m_size; ++x)
         {
            if (cell(x, y, z) == AIR)
            {
               for (size_t i = 0; i < 6; ++i)
               {
                  int index = getNeigbour(x, y, z, i);
                  
                  if (index != -1)
                  {
                     if (cell(index) == DIRT)
                     {
                        Vec3f normal = getNeigbourOffset(i);
                        Vec3f up = std::abs(normal.dot(Vec3f::yAxis())) > 0.9f ? Vec3f::zAxis() : Vec3f::yAxis();                   
                        Vec3f side = normal.cross(up) * 0.5f;
                        up = up * 0.5f;
                        
                        Vec3f pos = offset + Vec3f(x, y, z) + (normal * 0.5f);
                        
                        size_t indexStart = mesh.getNumVertices();
                        
                        
                        // Layout of the quad
                        //
                        // 0 --------------- 3
                        // |        ^ up     |
                        // |        |        |
                        // |  side  |        |
                        // |  <-----o pos    |
                        // |                 |
                        // |                 |
                        // |                 |
                        // 1 --------------- 2
                        
                        // Create the vertex data
                        mesh.appendVertex(pos + side + up);
                        mesh.appendVertex(pos + side - up);
                        mesh.appendVertex(pos - side - up);
                        mesh.appendVertex(pos - side + up);
                        mesh.appendNormal(-normal);
                        mesh.appendNormal(-normal);
                        mesh.appendNormal(-normal);
                        mesh.appendNormal(-normal);
                        
                        // Create triangle data
                        mesh.appendTriangle(indexStart + 0, indexStart + 1, indexStart + 2);
                        mesh.appendTriangle(indexStart + 0, indexStart + 2, indexStart + 3);
                     }
                  }
               }
            }
         }
}

