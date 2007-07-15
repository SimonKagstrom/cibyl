/**
 * Geometry utilities in support of polygon graphics in J2ME
 *
 * @author <a href="mailto:simonturner@users.sourceforge.net">Simon Turner</a> 
 * @version $Id: GeomUtils.java,v 1.1.1.1.2.3 2006/02/26 17:25:00 simonturner Exp $
 */
public abstract class GeomUtils {

    /**
     * Is the point represented by px and py within the triangle
     * represented by ax,ay,bx,by,cx,cy?
     *
     * @param px    The x of the point to test
     * @param py    The y of the point to test
     * @param ax    The x of the 1st point of the triangle
     * @param ay    The y of the 1st point of the triangle
     * @param bx    The x of the 2nd point of the triangle
     * @param by    The y of the 2nd point of the triangle
     * @param cx    The x of the 3rd point of the triangle
     * @param cy    The y of the 3rd point of the triangle
     * @return      true if the point is inside the triangle
     */
    static boolean withinBounds(int px, int py,
                                       int ax, int ay, 
                                       int bx, int by,
                                       int cx, int cy) {
        if (   px < min(ax, bx, cx) 
            || px > max(ax, bx, cx) 
            || py < min(ay, by, cy)
            || py > max(ay, by, cy) ) {
                return false;
        }
        boolean sameabc = sameSide(px, py, ax, ay, bx, by, cx, cy);
        boolean samebac = sameSide(px, py, bx, by, ax, ay, cx, cy);
        boolean samecab = sameSide(px, py, cx, cy, ax, ay, bx, by);
        return sameabc && samebac && samecab;
    }

    /**
     * Split a polygon into two, divided by the diagonal between the point
     * at aIndex and the point at bIndex. Nb: assumes that the line between
     * the two points is completely within the polygon.
     * 
     * @param xPoints   The x-points of the polygon
     * @param yPoints   The y-points of the polygon
     * @param aIndex    The index within the polygon of the first point
     *                  of the internal line
     * @param bIndex    The index within the polygon of the second point
     *                  of the internal line
     * @return          A 3D array of ints, where the 2 top-level arrays 
     *                  represent 2 new polygons; each polygon consists of a 
     *                  2D array of ints, where the 1st sub-array represents
     *                  xPoints and the second sub-array represents yPoints,  
     *                  eg: [[[ax1,ax2,ax3][ay1,ay2,ay3]],[[bx1,bx2][by1,by2]]]
     */
    static int[][][] split(int[] xPoints, int[] yPoints, int aIndex, int bIndex) {
        int firstLen, secondLen;
        if (bIndex < aIndex) {
            firstLen = (xPoints.length - aIndex) + bIndex + 1;
        } else {
            firstLen = (bIndex - aIndex) + 1;
        }
        secondLen = (xPoints.length - firstLen) + 2;
        int[][] first = new int[2][firstLen];
        int[][] second = new int[2][secondLen];
        for (int i=0; i<firstLen; i++) {
            int index = (aIndex + i) % xPoints.length;
            first[0][i] = xPoints[index];
            first[1][i] = yPoints[index];
        }
        for (int i=0; i<secondLen; i++) {
            int index = (bIndex + i) % xPoints.length;
            second[0][i] = xPoints[index];
            second[1][i] = yPoints[index];
        }
        int[][][] result = new int[2][][];
        result[0] = first;
        result[1] = second;
        return result;
    }

    /**
     * Trim an "ear" off a polygon
     *
     * @param xPoints   The x-points of the polygon
     * @param yPoints   The y-points of the polygon
     * @param earIndex  The index of the point to remove
     * @return          The supplied polygon, without the point at earIndex
     */
    static int[][] trimEar(int[] xPoints, int[] yPoints, int earIndex) {
        int[] newXPoints = new int[xPoints.length - 1];
        int[] newYPoints = new int[yPoints.length - 1];
        int[][] newPoly = new int[2][];
        newPoly[0] = newXPoints;
        newPoly[1] = newYPoints;
        int p = 0;
        for (int i=0; i<xPoints.length; i++) {
            if (i != earIndex) {
                newXPoints[p] = xPoints[i];
                newYPoints[p] = yPoints[i];
                p++;
            }
        }
        return newPoly;
    }

    /**
     * Get the index of the lowest value in the array
     * 
     * @param elements  The values from which the lowest is to be found
     * @return          The index of the lowest value in the array
     */
    static int indexOfLeast(int[] elements) {
        int index = 0;
        int least = elements[0];
        for (int i=1; i<elements.length; i++) {
            if (elements[i] < least) {
                index = i;
                least = elements[i];
            }
        }
        return index;
    }

    // internal helpers

    /**
     * @return  true if the line p1 and p2 are both on the same 
     *          side of the line between l1 and l2
     */
    private static boolean sameSide (int p1x, int p1y, int p2x, int p2y, 
                                     int l1x, int l1y, int l2x, int l2y) {
    	long lhs = ((p1x - l1x) * (l2y - l1y) - (l2x - l1x) * (p1y - l1y));
    	long rhs = ((p2x - l1x) * (l2y - l1y) - (l2x - l1x) * (p2y - l1y));
    	long product = lhs * rhs;
    	boolean result = product >= 0;
	    return result;
    }

    /**
     * @return  The smallest of three numbers
     */
    private static int min(int a, int b, int c) {
        return Math.min(Math.min(a,b),c);
    }

    /**
     * @return  The biggest of three numbers
     */
    private static int max(int a, int b, int c) {
        return Math.max(Math.max(a,b),c);
    }

}
