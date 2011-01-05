#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "cinder/Area.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Timer.h"
#include "cinder/Rand.h"

#include "World.h"
#include "LightIndex.h"
#include "Resources.h"

#define WORLD_SIZE 32

using namespace ci;
using namespace ci::app;
using namespace std;

class LIDRApp : public AppBasic
{
   public:
      void setup();
      void mouseDown(MouseEvent event);
      void mouseDrag(MouseEvent event);
      void update();
      void draw();
      
      void drawShadow();
      void blurShadows();
      
   private:
      ci::CameraPersp m_camera;
      ci::CameraOrtho m_shadowCamera;
      
      World * m_world;
      lidr::LightIndex * m_lightIndex;
      gl::Fbo * m_shadow;
      gl::Fbo * m_shadow2;
      ci::Area m_defaultArea;
      ci::MayaCamUI m_maya;
      ci::Timer m_timer;
      double m_lastTimeStamp;
      double m_stepTime;
      
      ci::gl::GlslProg m_blurHoriz;
      ci::gl::GlslProg m_blurVert;
      
      ci::gl::GlslProg m_lighting;
      
};

void LIDRApp::setup()
{   
   float offset = WORLD_SIZE * 0.55;
   m_shadowCamera.setOrtho(-offset, offset, -offset, offset, 1.0f, WORLD_SIZE * 2.0f);
   m_shadowCamera.lookAt(Vec3f(0.0f, WORLD_SIZE * 2.0f, 0.0f), Vec3f::zero());

   // Create world
   m_world = new World(WORLD_SIZE);
   m_world->buildFloatingMountain();
   
   // Create the light index
   m_lightIndex = new lidr::LightIndex(getWindowWidth(), getWindowHeight());
   
   // Add some random lights
   
   ci::Rand rand;
   for (int i = 0; i < 100; ++i)
   {
      lidr::LightId id = m_lightIndex->createLight();
      
      m_lightIndex->setPosition(id, -offset + rand.nextFloat(offset * 2.0f),
                                    -offset + rand.nextFloat(offset * 2.0f),
                                    -offset + rand.nextFloat(offset * 2.0f));
      m_lightIndex->setColor(id, rand.nextInt(256), rand.nextInt(256), rand.nextInt(256));
      m_lightIndex->setAttenuation(id, rand.nextFloat(2.0f, 10.0f));
   }
   
   /*
   {
      lidr::LightId id = m_lightIndex->createLight();
      
      m_lightIndex->setPosition(id, 0.0f, WORLD_SIZE, 0.0f);
      m_lightIndex->setColor(id, 255, 0, 0);
      m_lightIndex->setAttenuation(id, WORLD_SIZE);
   }
   */
   
   // Create shadow targets
   m_shadow = new gl::Fbo(128, 128, true);
   m_shadow2 = new gl::Fbo(128, 128, true);
   
   gl::enableDepthRead();
   gl::enableDepthWrite();
   gl::enable(GL_TEXTURE_2D);
   
   m_timer.start();
   m_lastTimeStamp = m_timer.getSeconds();
   
   // Load blur shaders
   m_blurHoriz = gl::GlslProg(loadResource(RES_PASS_THRU_VERT), loadResource(RES_BLUR_HORIZ_FRAG));
   m_blurVert = gl::GlslProg(loadResource(RES_PASS_THRU_VERT), loadResource(RES_BLUR_VERT_FRAG));
   
   m_lighting = gl::GlslProg(loadResource(RES_LIGHTING_VERT), loadResource(RES_LIGHTING_FRAG));
}

void LIDRApp::mouseDown(MouseEvent event)
{
   m_maya.mouseDown(event.getPos());
}

void LIDRApp::mouseDrag(MouseEvent event)
{
   m_maya.mouseDrag(event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown());
}

void LIDRApp::update()
{
   double timeStamp = m_timer.getSeconds();
   double dt = timeStamp - m_lastTimeStamp;
   m_lastTimeStamp = timeStamp;

   CameraPersp camera(m_maya.getCamera());
   camera.setPerspective(60.0f, getWindowWidth() / (float)getWindowHeight(), 1.0f, 1000.0f);
   m_maya.setCurrentCam(camera);
}

void LIDRApp::draw()
{
   drawShadow();
   blurShadows();
   
	gl::clear(Color(0.7, 0.7, 0.7));
   
   gl::setModelView(m_maya.getCamera());
   gl::setProjection(m_maya.getCamera());
   
   m_lightIndex->update(gl::getModelView());
   
   // Draw world
   m_lighting.bind();
   m_lighting.uniform("lightIndexTex", 0);
   m_lighting.uniform("lightPosTex", 1);
   m_lighting.uniform("lightColorTex", 2);
   
   m_lightIndex->bindLightIndexTexture(0);
   m_lightIndex->bindPositionTexture(1);
   m_lightIndex->bindColorTexture(2);
   
   m_world->simpleRender(Color8u(255, 20, 100));
   
   m_lightIndex->unbindLightIndexTexture();
   m_lightIndex->unbindPositionTexture();
   m_lightIndex->unbindColorTexture();
   m_lighting.unbind();
   
   // Draw shadow
   m_shadow->bindTexture();
   
   float size = WORLD_SIZE * 1.2f;
   
   gl::enableAlphaBlending();
   gl::color(ColorA8u(255, 255, 255, 200));
   gl::drawBillboard(Vec3f(0.0f, -WORLD_SIZE, 0.0f), Vec2f(size, size), 0.0f, Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f));
   gl::disableAlphaBlending();
   
   m_shadow->unbindTexture();
   
   // Update light index texture
   gl::disableDepthWrite();
   m_lightIndex->renderLightIndex();
   gl::enableDepthWrite();
   
   // Draw debug textures
   gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
   
   m_lightIndex->bindLightIndexTexture(0);
   gl::drawSolidRect(Rectf(10, getWindowHeight() - 138, 128 * getWindowAspectRatio(), getWindowHeight() - 10));
   m_lightIndex->unbindLightIndexTexture();
   
   m_lightIndex->bindPositionTexture(0);
   gl::drawSolidRect(Rectf(10, 10, 15, 266));
   m_lightIndex->unbindPositionTexture();
   
   m_lightIndex->bindColorTexture(0);
   gl::drawSolidRect(Rectf(20, 10, 25, 266));
   m_lightIndex->unbindColorTexture();
}

void LIDRApp::drawShadow()
{
   Area current = gl::getViewport();

   m_shadow->bindFramebuffer();
   gl::setViewport(m_shadow->getBounds());
   
   gl::clear(ColorA(0.0, 0.0, 0.0, 0.0));
   gl::setModelView(m_shadowCamera);
   gl::setProjection(m_shadowCamera);
   
   m_world->simpleRender(Color8u(0, 0, 0));
   
   m_shadow->unbindFramebuffer();
   
   gl::setViewport(current);
}

void LIDRApp::blurShadows()
{
   Area current = gl::getViewport();
   gl::pushMatrices();
   
   gl::setMatricesWindow(128, 128);
   //gl::setViewport(m_shadow->getBounds());
   gl::enableAlphaBlending();
   
   // Blur horizontal
   m_shadow2->bindFramebuffer();
   gl::clear(ColorA(0.0, 0.0, 0.0, 0.0));

   m_shadow->bindTexture();
   m_blurHoriz.bind();
   m_blurHoriz.uniform("tex", 0);
   m_blurHoriz.uniform("step", 1.0f / 128.0f);
   
   gl::drawSolidRect(Rectf(0, 0, 128, 128));
   
   m_blurHoriz.unbind();
   m_shadow->unbindTexture();

   // Blur vertical
   m_shadow->bindFramebuffer();
   gl::clear(ColorA(0.0, 0.0, 0.0, 0.0));
   
   m_shadow2->bindTexture();
   m_blurVert.bind();
   m_blurVert.uniform("tex", 0);
   m_blurVert.uniform("step", 1.0f / 128.0f);
   
   gl::drawSolidRect(Rectf(0, 0, 128, 128));
   
   m_blurVert.unbind();
   m_shadow2->unbindTexture();
   m_shadow->unbindFramebuffer();
        
   gl::disableAlphaBlending();
   gl::popMatrices();
   gl::setViewport(current);
}


CINDER_APP_BASIC(LIDRApp, RendererGl)
