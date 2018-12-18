#ifndef VANILLA_H_INCLUDED
#define VANILLA_H_INCLUDED

#include <iostream>
#include <tuple>
#include <math.h>
#include <sstream>
#include <vector>
#include <array>
#include <stdlib.h>
#include <memory>

class Named
{
    std::string name = "";
public:
    Named(std::string nameTag);
    std::string getName() const;
};

class IDed
{
    int id = 0;
public:
    IDed(int ID);
    int getID();
};

class NumberManager
{
protected:

    std::vector<double> numbers; //numbers are ordered from smallest to greatest
public:
    NumberManager();
    virtual void addNumber(double number);
    virtual int findNumber(double code);
    virtual void removeNumber(double number);
    ~NumberManager();

};


class Point
{
public:
    double x,y;
    Point(double xCoord = 0, double yCoord = 0)
    {
        x = xCoord;
        y = yCoord;
    }
};

class Line
{

public:
    Point a,b ;
    double radians;
    Line(const Point& x,const Point&y)
    {
        a =x;
        b = y;
        radians = atan2((a.y - b.y),(a.x - b.x));
    }
};


void printPoint(const Point& p);
class Shape
{
    Point center = {0,0};
protected:
    virtual void setCenter(double x, double y)
    {
        center = {x,y};
    }
public:
    Shape()
    {

    }
    const Point& getCenter() const
    {
        return center;
    }
    virtual bool intersects(Shape& shape)
    {
        return false;
    }
    virtual bool lineIntersects(Line& line)
    {
      return false;
    }
    virtual bool pointIn(Point& p)
    {
        return false;
    }
};

bool LineAndLineIntersect(const Line& l1, const Line& l2);
class Rect;
class Polygon : public Shape
{
    protected:
    std::vector <Line> lines;
    double minX,minY,maxX,maxY;
public:
    Polygon(std::initializer_list<Point> list)
    {
       load(list);

    }
    Polygon()
    {

    }
    void load(std::initializer_list<Point> list)
    {
         int size = list.size();
        double xSum = 0, ySum = 0;
        Point first = (*list.begin());
        minX = first.x;
        maxX = first.x;
        minY = first.y;
        maxY = first.y;
        Point prevPoint;
        int count = 0;
        for (std::initializer_list<Point>::iterator i = list.begin(); i != list.end(); i ++)
        {
            Point current = (*i);
            xSum += current.x;
            ySum += current.y;
            if (current.x < minX)
            {
                minX = current.x;
            }
            else if (current.x > maxX)
            {
                maxX = current.x;
            }
            if (current.y < minY)
            {
                minY = current.y;
            }
            else if (current.y >  maxY)
            {
                maxY = current.y;
            }

            if (count > 0)
            {
                lines.push_back(Line(prevPoint,current));
            }
            count ++;
            prevPoint = (*i);

        }


        setCenter(xSum/size, ySum/size);
    }

    double getMinX() const
    {
        return minX;
    }
    double getMaxX() const
    {
        return maxX;
    }
    double getMinY() const
    {
        return minY;
    }
    double getMaxY() const
    {
        return maxY;
    }
    virtual  Rect getBoundingRect() const;
    virtual const Line* intersects(const Polygon& other) const
    {
        const std::vector<Line> otherVec = other.getLines();
        int linesSize = lines.size();
        int otherVecSize = otherVec.size();
      //  std::cout << linesSize << " " << otherVecSize << std::endl;
        for (int i = 0; i < otherVecSize; i ++)
        {  const Line* value = lineIntersects(otherVec[i]);
            if (value)
            {
                return value;
            }
        }
        return nullptr;
    }
    virtual const Line* lineIntersects(const Line& line) const
    {
        int size = lines.size();
        for (int j = 0; j < size; j ++)
            {
                if (LineAndLineIntersect(lines[j],line))
                {
                    return &line;
                }
            }
            return nullptr;
    }
    virtual void increment(double xDistance, double yDistance)
    {
        int size = lines.size();
        for (int i = 0; i < size; i ++)
        {
            lines[i].a.x += xDistance;
            lines[i].a.y += yDistance;
            lines[i].b.x += xDistance;
            lines[i].b.y += yDistance;
        }
        minX += xDistance;
        maxX += xDistance;
        minY += yDistance;
        maxY += yDistance;
    }
    const std::vector<Line>& getLines() const
    {
        return lines;
    }
};


class Rect : public Polygon
{
public:
    double x,y,w,h;
    Rect(double xCoord = 0, double yCoord = 0, double width = 0, double height = 0)
    {
        x = xCoord;
        y = yCoord;
        w = width;
        h = height;
        //setCenter(x + w/2, y + h/2);
        load({Point(xCoord,yCoord), Point(xCoord + width,yCoord),  Point(xCoord + width, yCoord + height),Point(xCoord, yCoord + height),Point(xCoord, yCoord)});

    }
    Rect getBoundingRect()const
    {
       //std::cout << x << "" << y << std::endl;;
        return Rect(*this);
    }
    void increment(double xDistance, double yDistance)
    {
        x += xDistance;
        y += yDistance;
        Polygon::increment(xDistance, yDistance);
    }
    operator= (Rect r)
    {
        std::swap(x ,r.x);
        y = r.y;
        w = r.w;
        h = r.h;

    }
    using Polygon::intersects;
    bool intersects(Rect& r2)
    {
        if (this->x <= r2.x + r2.w && this->x + this->w >= r2.x && this->y <= r2.y + r2.h && this->y + this->h >= r2.y)
        {
            return true;
        }
        return false;
    }
};


void printRect(const Rect& r);
bool PointInRect(const Point& point, const Rect& rect); //returns true if the point is in the rect or on the borders

bool LineAndLineIntersect(const Point& a, const Point& b, const Point& c, const Point& d);

bool LineAndLineIntersect(const Line& l1, const Line& l2);

bool LineAndRectIntersect(const Rect& r, double x1, double y1, double x2, double y2 );

bool RectIntersect(const Rect& r1, const Rect& r2);

double PointDistance(const Point& p1, const Point& p2);

int convertTo1(double number); // a method that converts a number to 1 or -1 depending on its sign. If entry is 0, return 0;

class GradientNumber
{
    double first, last;
    double current;
    double speed;
    double* goal;
public:
    GradientNumber(double a = 0, double b = 0, double velocity= 1)
    {
      load(a,b,velocity);
    }
    void load (double a,double b, double velocity)
    {
        speed = velocity;
        first = a;
        last = b;
        current = a;
        goal = &last;
       // std::cout << a << " " << b << std::endl;
    }
    double update()
    {
        if (current == first)
        {
            goal = &last;
        }
        else if (current == last)
        {
            goal = &first;
        }
        current += convertTo1(*goal - current)*std::min(fabs(speed), fabs(*goal - current));
        return current;
    }
};

int findIntLength(int x);
std::string convert(double input);

std::string convert(int input); //takes an int and returns the int in string form

double convert(std::string input);

int charCount(std::string s, char c); //returns how many times c shows up in s;

std::string* divideString(std::string input); //divides a string into parts and puts them all into an array
bool rectIntersectWithinRange(const Rect& r1, double dist,const Rect& r2); //checks if a rectange 2dist larger on all dimension than r1 would intersect with r2
bool rectIntersectWithinRange(float x1, float y1, float w1, float h1, double dist, float x2, float y2, float w2, float h2);

#endif // VANILLA_H_INCLUDED
