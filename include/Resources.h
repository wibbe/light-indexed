#pragma once
#include "cinder/CinderResources.h"

#define RES_PASS_THRU_VERT     CINDER_RESOURCE(../resources/, PassThru.vert, 128, GLSL)
#define RES_BLUR_HORIZ_FRAG    CINDER_RESOURCE(../resources/, BlurHorizontal.frag, 129, GLSL)
#define RES_BLUR_VERT_FRAG     CINDER_RESOURCE(../resources/, BlurVertical.frag, 130, GLSL)

#define RES_LIGHTING_VERT     CINDER_RESOURCE(../resources/, Lighting.vert, 131, GLSL)
#define RES_LIGHTING_FRAG     CINDER_RESOURCE(../resources/, Lighting.frag, 132, GLSL)