
#ifndef LIGHT_INDEX_H
#define LIGHT_INDEX_H

#include <vector>
#include "cinder/Surface.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"

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
         LightIndex(int width, int height);
         ~LightIndex();
         
         LightId createLight();
         
         void setPosition(LightId id, float x, float y, float z);
         void setAttenuation(LightId id, float attenuation);
         void setColor(LightId id, unsigned char r, unsigned char g, unsigned char b);
         
         void update();
         void renderLightIndex();
         
         void bindLightIndexTexture(int unit);
         void unbindLightIndexTexture();
         
         void bindPositionTexture(int unit);
         void unbindPositionTexture();
         
         void bindColorTexture(int unit);
         void unbindColorTexture();
         
      private:
         void updateColorSurface();
         void updatePositionSurface();
         
         void sortLights();      
         bool collideWithChannel(int channel, LightId id);
         
         inline Light & getLight(LightId id)
         {
            return m_lights[id - 1];
         }
         
      private:
         std::vector<Light> m_lights;
         bool m_dirty;
         
         ci::Surface32f m_positionSurface;
         ci::Surface8u m_colorSurface;
         
         ci::gl::Texture * m_positionTex;
         ci::gl::Texture * m_colorTex;
         
         ci::gl::Fbo * m_colorIndex;
         
         std::vector<LightId> m_channels[4];
   };

}

#endif

