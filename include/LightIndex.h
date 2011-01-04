
#ifndef LIGHT_INDEX_H
#define LIGHT_INDEX_H

#include <vector>
#include "cinder/Surface.h"

namespace lidr
{
  typedef unsigned int LightId;

  class LightIndex
  {
    private:
      struct Light
      {
        float x, y, z;
        float attenuation;
        unsigned char r, g, b;
      };

    public:
      LightIndex();
      ~LightIndex();

      LightId createLight();

      void setPosition(LightId id, float x, float y, float z);
      void setAttenuation(LightId id, float attenuation);
      void setColor(LightId id, unsigned char r, unsigned char g, unsigned char b);

      void update();

    private:
      void updateColorSurface();
      void updatePositionSurface();
  
    private:
      std::vector<Light> m_lights;
      bool m_dirty;

      ci::Surface32f m_positionSurface;
      ci::Surface8u m_colorSurface;
  };

}

#endif

