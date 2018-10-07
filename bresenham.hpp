#ifndef BRESENHAM_HPP
#define BRESENHAM_HPP

template <typename Callback>
bool bresenham(int x0, int y0, int x1, int y1, Callback && callback)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy)/2, e2;

    while(callback(x0, y0))
    {
        if (x0 == x1 && y0 == y1)
            return true;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
    return false;
}

#endif // BRESENHAM_HPP
