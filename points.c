#include "Point.h"
#include "Circle.h"

int main(int argc, char ** argv)
{
    void * p;

    initPoint();
    initCircle();

    while (*++argv)
    {
        switch (**argv)
        {
        case 'p':
            p = new(Point, 1, 2);
            break;
        case 'c':
            p = new(Circle, 1, 2, 3);
            break;
        default:
            continue;
        }
        void * point = new(Point, 3, 5);
        draw(point);
        move(point, 1, 2);
        draw(point);
        puto(point, stdout);
        delete(point);
        draw(p);
        move(p, 10, 20);
        draw(p);
        delete(p);
    }
    return 0;
}