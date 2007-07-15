#ifndef __JMICROPOLYGON_H__
#define __JMICROPOLYGON_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <javax/microedition/lcdui.h>

/**
 * Draw an outlined polygon.
 *
 * @param xPoints a pointer to the x-coordinates
 * @param yPoints a pointer to the y-coordinates
 * @param count the number of (x,y) coordinates in the polygon
 */
void NOPH_PolygonGraphics_drawPolygon(NOPH_Graphics_t g, int* xPoints, int* yPoints, int count);

/**
 * Draw a filled polygon.
 *
 * @param xPoints a pointer to the x-coordinates
 * @param yPoints a pointer to the y-coordinates
 * @param count the number of (x,y) coordinates in the polygon
 */
void NOPH_PolygonGraphics_fillPolygon(NOPH_Graphics_t g, int* xPoints, int* yPoints, int count);

#if defined(__cplusplus)
}
#endif
#endif /* !__JMICROPOLYGON_H__ */
