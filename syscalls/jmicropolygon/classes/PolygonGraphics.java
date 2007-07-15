import javax.microedition.lcdui.Graphics;

/**
 * Polygon rendering for J2ME MIDP 1.0.
 *
 * <p>This has its own fillTriangle() method because of the 
 * absence of that method in MIDP 1.0 (unlike MIDP 2.0). </p>
 *
 * @author <a href="mailto:simonturner@users.sourceforge.net">Simon Turner</a> 
 * @version $Id: PolygonGraphics.java,v 1.1.1.1.2.2 2006/04/08 12:58:06 simonturner Exp $
 */
public class PolygonGraphics {

    public static void drawPolygon(Graphics g, int xPointsAddr, int yPointsAddr,
                                   int count) {
        if ((xPointsAddr % 4) != 0) System.out.println("Unaligned X addr:" + xPointsAddr);
        if ((yPointsAddr % 4) != 0) System.out.println("Unaligned Y addr:" + yPointsAddr);
        xPointsAddr /= 4;
        yPointsAddr /= 4;
        int[] xPoints = new int[count];
        int[] yPoints = new int[count];

        for (int i = 0; i < count; i++)
        {
            xPoints[i] = CRunTime.memory[xPointsAddr + i];
            yPoints[i] = CRunTime.memory[yPointsAddr + i];
        }

        drawPolygon(g, xPoints, yPoints);
    }

    public static void fillPolygon(Graphics g, int xPointsAddr, int yPointsAddr,
                                   int count) {
        int[] xPoints = new int[count];
        int[] yPoints = new int[count];

        if ((xPointsAddr % 4) != 0) System.out.println("Unaligned X addr:" + xPointsAddr);
        if ((yPointsAddr % 4) != 0) System.out.println("Unaligned Y addr:" + yPointsAddr);
        xPointsAddr /= 4;
        yPointsAddr /= 4;

        for (int i = 0; i < count; i++)
        {
            xPoints[i] = CRunTime.memory[xPointsAddr + i];
            yPoints[i] = CRunTime.memory[yPointsAddr + i];
        }

        fillPolygon(g, xPoints, yPoints);
    }

    /**
     * Draw a polygon
     *
     * @param g         The Graphics object to draw the polygon onto
     * @param xPoints   The x-points of the polygon
     * @param yPoints   The y-points of the polygon
     */
    public static void drawPolygon(Graphics g, int[] xPoints, int[] yPoints) {
        int max = xPoints.length - 1;
        for (int i=0; i<max; i++) {
            g.drawLine(xPoints[i], yPoints[i], xPoints[i+1], yPoints[i+1]);
        }
        g.drawLine(xPoints[max], yPoints[max], xPoints[0], yPoints[0]);
    }

    /**
     * Fill a polygon
     *
     * @param g         The Graphics object to draw the polygon onto
     * @param xPoints   The x-points of the polygon
     * @param yPoints   The y-points of the polygon
     */
    public static void fillPolygon(Graphics g, int[] xPoints, int[] yPoints) {
        while (xPoints.length > 2) {
            // a, b & c represents a candidate triangle to draw. 
            // a is the left-most point of the polygon
            int a = GeomUtils.indexOfLeast(xPoints);
            // b is the point after a
            int b = (a + 1) % xPoints.length;
            // c is the point before a
            int c = (a > 0) ? a - 1 : xPoints.length - 1;
            // The value leastInternalIndex holds the index of the left-most 
            // polygon point found within the candidate triangle, if any.
            int leastInternalIndex = -1;
            boolean leastInternalSet = false;
            // If only 3 points in polygon, skip the tests
            if (xPoints.length > 3) {
                // Check if any of the other points are within the candidate triangle
                for (int i=0; i<xPoints.length; i++) {
                    if (i != a && i != b && i != c) {
                        if (GeomUtils.withinBounds(xPoints[i], yPoints[i], 
                                                   xPoints[a], yPoints[a],
                                                   xPoints[b], yPoints[b],
                                                   xPoints[c], yPoints[c])) {
                            // Is this point the left-most point within the candidate triangle?
                            if (!leastInternalSet || xPoints[i] < xPoints[leastInternalIndex]) {
                                leastInternalIndex = i;
                                leastInternalSet = true;
                            }
                        }
                    }
                }
            }
            // No internal points found, fill the triangle, and reservoir-dog the polygon
            if (!leastInternalSet) {
                g.fillTriangle(xPoints[a], yPoints[a], xPoints[b], yPoints[b], xPoints[c], yPoints[c]);
                int[][] trimmed = GeomUtils.trimEar(xPoints, yPoints, a);
                xPoints = trimmed[0];
                yPoints = trimmed[1];
            // Internal points found, split the polygon into two, using the line between
            // "a" (left-most point of the polygon) and leastInternalIndex (left-most  
            // polygon-point within the candidate triangle) and recurse with each new polygon
            } else {
                int[][][] split = GeomUtils.split(xPoints, yPoints, a, leastInternalIndex);
                int[][] poly1 = split[0];
                int[][] poly2 = split[1];
                fillPolygon(g, poly1[0], poly1[1]);
                fillPolygon(g, poly2[0], poly2[1]);
                break;
            }
        }
    }
}
