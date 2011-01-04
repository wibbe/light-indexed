
#include "LightIndex.h"
#include <casser>

namespace lidr
{

  LightIndex::LightIndex()
  {
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
    return m_lights.size();
  }

  void LightIndex::setPosition(LightId id, float x, float y, float z)
  {
    assert(id > 0 && id <= m_lights.size());

    Light & light = m_lights[id];
    light.x = x;
    light.y = y;
    light.z = z;
  }

  void LightIndex::setAttenuation(LightId id, float attenuation)
  {
    assert(id > 0 && id <= m_lights.size());

    Light & light = m_lights[id];
    light.attenuation = attenuation;
  }

  void LightIndex::setColor(LightId id, unsigned char r, unsigned char g, unsigned char b)
  {
    assert(id > 0 && id <= m_lights.size());

    Light & light = m_lights[id];
    light.r = r;
    light.g = g;
    light.b = b;
  }

}

