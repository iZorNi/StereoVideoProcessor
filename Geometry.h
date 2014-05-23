#pragma once
#include <utility>
#include <algorithm>
#include <math.h>
using std::pair;

const double EPS = 1E-9;
#define det(a,b,c,d)  (a*d-b*c)
#define PI 3.14159265
 
struct pt {
	double x, y;
 
	bool operator< (const pt & p) const {
		return x < p.x-EPS || abs(x-p.x) < EPS && y < p.y - EPS;
	}

	pt(){}

	pt(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

struct line {
	double a, b, c;
 
	line() {}
	line (pt p, pt q) {
		a = p.y - q.y;
		b = q.x - p.x;
		c = - a * p.x - b * p.y;
		norm();
	}
 
	void norm() {
		double z = sqrt (a*a + b*b);
		if (abs(z) > EPS)
			a /= z,  b /= z,  c /= z;
	}
 
	double dist (pt p) const {
		return a * p.x + b * p.y + c;
	}
};

struct rect{
	pt  a ,b ,c ,d;

	rect(){}

	rect (pt a, pt b, pt c, pt d)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	rect (pt b, double height, double width)
	{
		this->b = b;
		a.x = b.x;
		a.y = b.y + height;
		c.y = b.y;
		c.x = b.x + width;
		d.x = c.x;
		d.y = a.y;
	}

	double square()
	{
		double length1, length2;
		length1 = sqrt( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) );
		length2 = sqrt( (c.x - b.x)*(c.x - b.x) + (c.y - b.y)*(c.y - b.y) );
		return length1*length2;
	}
};

#undef max
#undef min
class Geometry
{
	static pt center;;
public:
	Geometry(void);
	~Geometry(void);
	//pair<int,int> intersection(pt a1, pt b1, pt a2, pt b2);
	//returns top left point, and new size in arguments
	static pair<int, int> getNewRect(int top, int left, int height, int width, double rotation1, double rotation2, int &newHeight, int &newWidth);
private:
	static inline bool betw (double l, double r, double x) {
		return std::min(l,r) <= x + EPS && x <= std::max(l,r) + EPS;
	}
 
	static inline bool intersect_1d (double a, double b, double c, double d) {
		if (a > b)  std::swap (a, b);
		if (c > d)  std::swap (c, d);
		return std::max (a, c) <= std::min (b, d) + EPS;
	}
	
	static bool intersect (pt a, pt b, pt c, pt d, pt & left, pt & right);
	
	static pair<int,int> intesect(pt a, pt b, pt c, pt d);

	static pair<pt,pt> rotate(pt a, pt b, pt o, double angle);

	static pt rotate(pt a, pt o, double angle);

	//gets perpendicular CW
	static pt getPerpendicular(pt a, pt b);

	static inline double length(pt a, pt b)
	{
		return sqrt( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) );
	}

	//gets point on perpendicular CW
	static pt getPointOnPerpendicular(pt o, pt a, double distance);

	//gets point opposite to a relatively to o
	static pt getOppositePt(pt a, pt o);

	static rect rotateRect(rect r, double angle);

	//gets max rect intersection between r and rotatedR
	static rect getMaxRect(rect r, rect rotatedR);

	static int getMax(double a, double b, double c, double d);

	//gets rect intersection between r1 and r2
	static rect getRectIntersection(rect r1, rect r2);

	static rect makeCorrection(rect r, double angle);

};

