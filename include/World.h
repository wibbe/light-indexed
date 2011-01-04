
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cassert>

#include "cinder/Color.h"
#include "cinder/TriMesh.h"

class World
{
   public:
      enum CELL_TYPES
      {
         AIR = 0,
         DIRT = 1
      };
      
   public:
      World(size_t size);
      ~World();
      
      void randomize(size_t numberOfLivingCells);
      void buildFloatingMountain();
      
      void simpleRender(ci::Color8u const& color);
      void generateMesh(ci::TriMesh & mesh);

      
      inline size_t size() const { return m_size; }
      
      inline size_t getIndex(size_t x, size_t y, size_t z) const
      {
         return x + (z * m_size) + (y * m_size * m_size);
      }
      
      inline size_t cell(size_t x, size_t y, size_t z) const
      {
         size_t i = getIndex(x, y, z);
         assert(i < m_cells.size());
         return m_cells[i];
      }
      
      inline void set(size_t x, size_t y, size_t z, size_t value)
      {
         size_t i = getIndex(x, y, z);
         assert(i < m_cells.size());
         m_cells[i] = value;
      }
      
   private:
   
      inline int getNeigbour(int x, int y, int z, int i)
      {
         assert(i < 24);
         
         const ci::Vec3<int> offsets[] = {
            ci::Vec3<int>(-1,  0,  0),
            ci::Vec3<int>( 1,  0,  0),
            ci::Vec3<int>( 0,  0, -1),
            ci::Vec3<int>( 0,  0,  1),
            ci::Vec3<int>( 0, -1,  0),
            ci::Vec3<int>( 0,  1,  0),
            ci::Vec3<int>(-1, -1, -1),
            ci::Vec3<int>(-1, -1,  0),
            ci::Vec3<int>(-1, -1,  1),
            ci::Vec3<int>(-1,  0, -1),
            ci::Vec3<int>(-1,  0,  1),
            ci::Vec3<int>(-1,  1, -1),
            ci::Vec3<int>(-1,  1,  0),
            ci::Vec3<int>(-1,  1,  1),
            ci::Vec3<int>( 0, -1, -1),
            ci::Vec3<int>( 0, -1,  1),
            ci::Vec3<int>( 0,  1, -1),
            ci::Vec3<int>( 0,  1,  0),
            ci::Vec3<int>( 0,  1,  1),
            ci::Vec3<int>( 1, -1, -1),
            ci::Vec3<int>( 1, -1,  0),
            ci::Vec3<int>( 1, -1,  1),
            ci::Vec3<int>( 1,  0, -1),
            ci::Vec3<int>( 1,  0,  1),
            ci::Vec3<int>( 1,  1, -1),
            ci::Vec3<int>( 1,  1,  0),
            ci::Vec3<int>( 1,  1,  1)
         };
         
         if ((x + offsets[i].x) >= 0 &&
             (x + offsets[i].x) < m_size &&
             (y + offsets[i].y) >= 0 &&
             (y + offsets[i].y) < m_size &&
             (z + offsets[i].z) >= 0 &&
             (z + offsets[i].z) < m_size)
            return getIndex(x + offsets[i].x, y + offsets[i].y, z + offsets[i].z);
            
         return -1;
      }
      
   private:
      size_t m_size;
      std::vector<size_t> m_cells;
};

#endif


