
#include "LightIndex.h"
#include "Resources.h"

#include "cinder/gl/gl.h"
#include "cinder/Area.h"
#include "cinder/app/App.h"

#include <cassert>

using namespace ci;

namespace lidr
{
   
   LightIndex::LightIndex(int width, int height)
      : m_dirty(true),
        m_colorSurface(1, 256, false)
   {  
      // Create color index buffer, use alpha and color, but no depth
      m_colorIndex = new gl::Fbo(width, height, true, true, false);
      
      m_colorTex = new gl::Texture(1, 256);
      
      gl::Fbo::Format positionFormat;
      positionFormat.setColorInternalFormat(GL_RGBA32F_ARB);
      positionFormat.enableDepthBuffer(false);
      positionFormat.enableMipmapping(false);
      positionFormat.setMinFilter(GL_NEAREST);
      positionFormat.setMagFilter(GL_NEAREST);
      m_positionFbo = new gl::Fbo(1, 256, positionFormat);
      
      m_colorShader = gl::GlslProg(app::loadResource(RES_COLOR_VERT), app::loadResource(RES_COLOR_FRAG));
      m_positionShader = gl::GlslProg(app::loadResource(RES_POSITION_VERT), app::loadResource(RES_POSITION_FRAG));
   }
   
   LightIndex::~LightIndex()
   {
      delete m_colorIndex;
      delete m_positionFbo;
      delete m_colorTex;
   }
   
   LightId LightIndex::createLight()
   {
      if (m_lights.size() >= 255)
         return -1;
      
      Light light = { 0.0f, 0.0f, 0.0f, 100.0f, 255, 255, 255 };
      m_lights.push_back(light);
      m_dirty = true;
      
      return m_lights.size();
   }
   
   void LightIndex::setPosition(LightId id, float x, float y, float z)
   {
      assert(id > 0 && id <= m_lights.size());
      
      Light & light = getLight(id);
      light.x = x;
      light.y = y;
      light.z = z;
      
      m_dirty = true;
   }
   
   void LightIndex::setAttenuation(LightId id, float attenuation)
   {
      assert(id > 0 && id <= m_lights.size());
      
      Light & light = getLight(id);
      light.attenuation = attenuation;
      
      m_dirty = true;
   }
   
   void LightIndex::setColor(LightId id, unsigned char r, unsigned char g, unsigned char b)
   {
      assert(id > 0 && id <= m_lights.size());
      
      Light & light = getLight(id);
      light.r = r;
      light.g = g;
      light.b = b;
      
      m_dirty = true;
   }
   
   void LightIndex::bindLightIndexTexture(int unit)
   {
      m_colorIndex->bindTexture(unit);
   }
   
   void LightIndex::unbindLightIndexTexture()
   {
      m_colorIndex->unbindTexture();
   }
   
   void LightIndex::bindPositionTexture(int unit)
   {
      m_positionFbo->bindTexture(unit);
   }
   
   void LightIndex::unbindPositionTexture()
   {
      m_positionFbo->unbindTexture();
   }
   
   void LightIndex::bindColorTexture(int unit)
   {
      m_colorTex->bind(unit);
   }
   
   void LightIndex::unbindColorTexture()
   {
      m_colorTex->unbind();
   }
   
   void LightIndex::update(Matrix44f const& view)
   {
      updatePositionSurface(view);
   
      if (m_dirty)
      {
         updateColorSurface();
         
         m_colorTex->update(m_colorSurface);
         
         sortLights();
         
         m_dirty = false;
      }
   }
   
   void LightIndex::renderLightIndex()
   {
      Area current = gl::getViewport();
      gl::setViewport(m_colorIndex->getBounds());

      m_colorIndex->bindFramebuffer();
      
      gl::clear(ColorA(0, 0, 0, 0), false);
      
      m_colorShader.bind();
      
      // Go thru all the channels and render them one by one
      for (int channel = 0; channel < 4; ++channel)
      {
         glColorMask(channel == 0, channel == 1, channel == 2, channel == 3);
         
         for (std::vector<LightId>::const_iterator it = m_channels[channel].begin(), end = m_channels[channel].end();
              it != end;
              ++it)
         {
            Light & light = getLight(*it);
            
            m_colorShader.uniform("color", Vec4f(*it / 255.0, *it / 255.0, *it / 255.0, *it / 255.0));

            gl::drawSphere(Vec3f(light.x, light.y, light.z), light.attenuation);
         }
      }
      
      m_colorShader.unbind();
      
      m_colorIndex->unbindFramebuffer();
      
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      gl::setViewport(current);
   }
   
   
   void LightIndex::sortLights()
   {
      // Start by clearing all the channels
      for (int i = 0; i < 4; ++i)
         m_channels[i].clear();
         
      // Go thru all the lights and try to sort them into a channel
      for (LightId id = 1, size = m_lights.size() + 1; id < size; ++id)
      {
         for (int channel = 0; channel < 4; ++channel)
            if (!collideWithChannel(channel, id))
            {
               m_channels[channel].push_back(id);
               break;
            }
      }
   }
   
   bool LightIndex::collideWithChannel(int channel, LightId id)
   {
      Light & light1 = getLight(id);
      
      std::vector<LightId>::const_iterator it = m_channels[channel].begin(), end = m_channels[channel].end();
      for (; it != end; ++it)
      {
         Light & light2 = getLight(*it);
         
         float x = light1.x - light2.x;
         float y = light1.y - light2.y;
         float z = light1.z - light2.z;
         float currentDist = x * x + y * y + z * z;
         float attenuation = light1.attenuation + light2.attenuation;
         
         if (currentDist <= (attenuation * attenuation))
            return true;
      }
      
      return false;
   }
   
   void LightIndex::updateColorSurface()
   {
      ci::Surface8u::Iter it = m_colorSurface.getIter();
      size_t i = 0;    
      
      while (it.line())
      {
         it.pixel();
         
         if (i == 0 || i > m_lights.size())
         {
            it.r() = 0;
            it.g() = 0;
            it.b() = 0;
         }
         else
         {
            Light & light = m_lights[i - 1];
            it.r() = light.r;
            it.g() = light.g;
            it.b() = light.b;
         }
         
         i++;
      }
   }
   
   void LightIndex::updatePositionSurface(Matrix44f const& view)
   {
      Area current = gl::getViewport();
      m_positionFbo->bindFramebuffer();
      m_positionShader.bind();
      
      gl::pushMatrices();
      gl::setMatricesWindow(1, 256);
      
      gl::clear(ColorA(0, 0, 0, 0), false);
      
      glBegin(GL_POINTS);
      
      for (int i = 0; i < 256; ++i)
      {
         if (i == 0 || i > m_lights.size())
         {
            glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
            glVertex3i(0, 256 - i, 0);
         }
         else
         {
            Light & light = m_lights[i - 1];
            Vec3f pos = view.transformPoint(Vec3f(light.x, light.y, light.z));
            
            glColor4f(pos.x, pos.y, pos.z, light.attenuation);
            glVertex3i(0, 256 - i, 0);
         }
      }
      
      glEnd();
      
      m_positionShader.unbind();
      m_positionFbo->unbindFramebuffer();
      
      gl::popMatrices();
      gl::setViewport(current);
   
      /*
   
      ci::Surface32f::Iter it = m_positionSurface.getIter();
      size_t i = 0;
      
      while (it.line())
      {
         it.pixel();
         
         if (i == 0 || i > m_lights.size())
         {
            it.r() = 0;
            it.g() = 0;
            it.b() = 0;
            it.a() = 0;
         }
         else
         {
            Light & light = m_lights[i - 1];
            Vec3f pos = view.transformPoint(Vec3f(light.x, light.y, light.z));
            ci::app::console() << pos << "\n";
            
            it.r() = pos.x;
            it.g() = pos.y;
            it.b() = pos.z;
            it.a() = light.attenuation;
         }
         
         i++;
      }
      
      */
   }
   
}

