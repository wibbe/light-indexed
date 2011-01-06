
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
      
      inline size_t cell(size_t index) const
      {
         assert(index < m_cells.size());
         return m_cells[index];
      }
      
      inline void set(size_t x, size_t y, size_t z, size_t value)
      {
         size_t i = getIndex(x, y, z);
         assert(i < m_cells.size());
         m_cells[i] = value;
      }
      
   private:
   
      inline int getNeigbour(int x, int y, int z, int i) const
      {
         ci::Vec3<int> offset = getNeigbourOffset(i);
         
         if ((x + offset.x) >= 0 &&
             (x + offset.x) < m_size &&
             (y + offset.y) >= 0 &&
             (y + offset.y) < m_size &&
             (z + offset.z) >= 0 &&
             (z + offset.z) < m_size)
            return getIndex(x + offset.x, y + offset.y, z + offset.z);
            
         return -1;
      }
      
      inline ci::Vec3<int> getNeigbourOffset(int i) const
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
         
         return offsets[i];
      }
      
   private:
      size_t m_size;
      std::vector<size_t> m_cells;
};

#endif


