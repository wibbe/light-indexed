
#include "LightIndex.h"
#include <casser>

namespace lidr
{

  LightIndex::LightIndex()
    : m_dirty(true),
      m_positionSurface(1, 256),
      m_colorSurface(1, 256)
  {
    updateColorSurface();
    updatePositionSurface();
  }
  
  LightIndex::~LightIndex()
  {
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

    Light & light = m_lights[id];
    light.x = x;
    light.y = y;
    light.z = z;

    m_dirty = true;
  }

  void LightIndex::setAttenuation(LightId id, float attenuation)
  {
    assert(id > 0 && id <= m_lights.size());

    Light & light = m_lights[id];
    light.attenuation = attenuation;

    m_dirty = true;
  }

  void LightIndex::setColor(LightId id, unsigned char r, unsigned char g, unsigned char b)
  {
    assert(id > 0 && id <= m_lights.size());

    Light & light = m_lights[id];
    light.r = r;
    light.g = g;
    light.b = b;

    m_dirty = true;
  }

  void LightIndex::update()
  {
    if (m_dirty)
    {
      updateColorSurface();
      updatePositionSurface();

      m_dirty = false;
    }
  }

  void LightIndex::updateColorSurface()
  {
    cu::Surface8u::Iter it = m_colorSurface.getIter();
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
        Light & light m_lights[i - 1];
        it.r() = light.r;
        it.g() = light.g;
        it.b() = light.b;
        it.a() = 0;
      }

      i++;
    }
  }

  void LightIndex::updatePositionSurface()
  {
    cu::Surface32f::Iter it = m_positionSurface.getIter();
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
        Light & light m_lights[i - 1];
        it.r() = light.x;
        it.g() = light.y;
        it.b() = light.z;
        it.a() = light.attenuation;
      }

      i++;
    }
  }

}

