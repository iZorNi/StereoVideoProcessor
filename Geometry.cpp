#include "stdafx.h"
#include "Geometry.h"
using std::pair;


pt Geometry::center;

Geometry::Geometry(void)
{
}


Geometry::~Geometry(void)
{
}

bool Geometry::intersect (pt a, pt b, pt c, pt d, pt & left, pt & right) {
	if (! intersect_1d (a.x, b.x, c.x, d.x) || ! intersect_1d (a.y, b.y, c.y, d.y))
		return false;
	line m (a, b);
	line n (c, d);
	double zn = det (m.a, m.b, n.a, n.b);
	if (abs (zn) < EPS) {
		if (abs (m.dist (c)) > EPS || abs (n.dist (a)) > EPS)
			return false;
		if (b < a)  std::swap (a, b);
		if (d < c)  std::swap (c, d);
		left = std::max (a, c);
		right = std::min (b, d);
		return true;
	}
	else {
		left.x = right.x = - det (m.c, m.b, n.c, n.b) / zn;
		left.y = right.y = - det (m.a, m.c, n.a, n.c) / zn;
		return betw (a.x, b.x, left.x)
			&& betw (a.y, b.y, left.y)
			&& betw (c.x, d.x, left.x)
			&& betw (c.y, d.y, left.y);
	}
}

pair<pt,pt> Geometry::rotate(pt a, pt b, pt o, double angle)
{
	pair<pt,pt> res;
	res.first = rotate(a,o,angle);
	res.second = rotate(b,o, angle);
	return res;

}

pt Geometry::rotate(pt a, pt o, double angle)
{
	pt res;
	res.x = o.x + (a.x - o.x)*cos(angle) + (a.y-o.y)*sin(angle);
	res.y = o.y - (a.x - o.x)*sin(angle) + (a.y-o.y)*cos(angle);
	return res;
}

pt Geometry::getPerpendicular(pt o, pt a)
{
	pt res;
	res.x = o.y - a.y;
	res.y = a.x - o.x;
	return res;
}

pt Geometry::getPointOnPerpendicular(pt o, pt a, double distance)
{
	pt res, perp, ptPerp;
	perp = getPerpendicular(a,o);
	ptPerp.x = perp.x+o.x;
	ptPerp.y = perp.y+o.y;
	res.x = o.x + distance*(perp.x)/length(ptPerp,o);
	res.y = o.y + distance*(perp.y)/length(ptPerp,o);
	return res;
}

pt Geometry::getOppositePt(pt a, pt o)
{
	pt tmp , res;
	tmp.x = -(a.x - o.x);
	tmp.y = -(a.y - o.y);
	res.x = tmp.x + o.x;
	res.y = tmp.y + o.y;
	return res;
}

rect Geometry::rotateRect(rect r, double angle)
{
	pt m,n;
	if((abs(angle-0)<EPS)||(abs(angle-180)<EPS))
		return r;
	if(angle>270.0)
	{
		angle=360-angle;
	} 
	else if(angle>180.0)
	{
		angle-=180.0;
	}
	else if(angle>90.0)
	{
		angle=180-angle;
	}
	rect rotR;
	m.x = r.a.x;
	m.y = center.y;
	n.y = m.y;
	n.x = r.c.x;
	pair<pt, pt> m1n1 = rotate(m, n, center, angle*PI/180);
	rotR.d = getPointOnPerpendicular(m1n1.second, center, length(r.c, r.d)/2 );
	rotR.c = getOppositePt(rotR.d, m1n1.second);
	rotR.b = getOppositePt(rotR.d, center);
	rotR.a = getOppositePt(rotR.b, m1n1.first);


	return rotR;


}

rect Geometry::getMaxRect(rect r, rect rotatedR)
{
	rect ef, kl, pr, st;
	double sef, skl, spr, sst;
	if( intersect(r.c, r.b, rotatedR.c, rotatedR.b, ef.b, ef.a) )
	{
		ef.a.x = ef.b.x;
		ef.a.y = r.a.y;
		ef.c = getOppositePt(ef.a, center);
		ef.d = getOppositePt(ef.b, center);
		sef = ef.square();
	}
	else
	{
		sef = 0.0;
	}

	if( intersect(r.a, r.b, rotatedR.c, rotatedR.b, kl.a, kl.b) )
	{
		kl.d.y = kl.a.y;
		kl.d.x = r.c.x;
		if (intersect(kl.a, kl.d, rotatedR.a, rotatedR.d, kl.d, kl.c))
		{
			kl.c = getOppositePt(kl.a, center);
			kl.b = getOppositePt(kl.d, center);
			kl.a.x = kl.b.x;
			kl.c.x = kl.d.x;
			skl =kl.square();
		}
		else
		{
			skl = 0.0;
		}
	}
	else
	{
		skl = 0.0;
	}

	if( intersect(r.a, r.b, rotatedR.a, rotatedR.b, pr.a, pr.b) )
	{
		pr.d.y = pr.a.y;
		pr.d.x = r.c.x;
		if (intersect(pr.a, pr.d, rotatedR.a, rotatedR.d, pr.d, pr.c))
		{
			if(pr.d.x>rotatedR.c.x)
				pr.d.x = rotatedR.c.x;
			pr.c = getOppositePt(pr.a, center);
			pr.b = getOppositePt(pr.d, center);
			pr.a.x = pr.b.x;
			pr.c.x = pr.d.x;
			spr =pr.square();
		}
		else
		{
			spr = 0.0;
		}
	}
	else
	{
		spr = 0.0;
	}

	if( intersect(r.a, r.d, rotatedR.a, rotatedR.b, st.a, st.a) )
	{
		st.b.y = r.c.y;
		st.b.x = st.a.x;
		if (intersect(st.a, st.b, rotatedR.b, rotatedR.c, st.b, st.c))
		{
			st.c = getOppositePt(st.a, center);
			st.d = getOppositePt(st.b, center);
			st.a.y = st.d.y;
			st.c.y = st.b.y;
			sst =st.square();
		}
		else
		{
			sst = 0.0;
		}
	}
	else
	{
		sst = 0.0;
	}
	switch(getMax(sef,skl,spr,sst))
	{
	case 1:
		return ef;
	case 2:
		return kl;
	case 3:
		return pr;
	case 4:
		return st;
	default:
		return ef;
	}

}

int Geometry::getMax(double a, double b, double c, double d)
{
	double tmp = std::max( std::max(a,b), std::max(c,d) );
	if(abs(tmp - a)<EPS)
		return 1;
	else if(abs(tmp - b)<EPS)
		return 2;
	else if(abs(tmp - c)<EPS)
		return 3;
	else if(abs(tmp - d)<EPS)
		return 4;
	else return 0;
}

rect Geometry::getRectIntersection(rect r1, rect r2)
{
	rect res;
	res.a.x = std::max(r1.a.x, r2.a.x);
	res.a.y = std::min(r1.a.y, r2.a.y);
	res.c = getOppositePt(res.a, center);
	res.b.x = res.a.x;
	res.b.y = res.c.y;
	res.d.x = res.c.x;
	res.d.y = res.a.y;
	return res;
}

rect Geometry::makeCorrection(rect r, double angle)
{
	rect resRect;
	if(angle>270)
	{
		r.a = getOppositePt(r.a, pt(r.a.x, center.y));
		r.b = getOppositePt(r.b, pt(r.b.x, center.y));
		r.c = getOppositePt(r.c, pt(r.c.x, center.y));
		r.d = getOppositePt(r.d, pt(r.d.x, center.y));
		resRect.a = r.d;
		resRect.b = r.a;
		resRect.c = r.b;
		resRect.d = r.c;
	}
	else if(angle>180)
	{
		resRect.a = r.a;
		resRect.b = r.b;
		resRect.c = r.c;
		resRect.d = r.d;
	}
	else if(angle>90)
	{
		r.a = getOppositePt(r.b, pt(center.x, r.b.y));
		r.b = getOppositePt(r.a, pt(center.x, r.a.y));
		r.c = getOppositePt(r.d, pt(center.x, r.d.y));
		r.d = getOppositePt(r.c, pt(center.x, r.c.y));
		resRect.a = r.b;
		resRect.b = r.c;
		resRect.c = r.d;
		resRect.d = r.a;
	}
	else
	{
		resRect.a = r.a;
		resRect.b = r.b;
		resRect.c = r.c;
		resRect.d = r.d;
	}
	return resRect;
}

pair<int,int> Geometry::getNewRect(int top, int left, int height, int width, double rotation1, double rotation2, int &newHeight, int &newWidth)
{
	pt topLeft;
	topLeft.x = left;
	topLeft.y = top;
	center.x = (left+width)/2.0;
	center.y = (top+height)/2.0;
	rect src = rect(topLeft, height, width);
	while(rotation1>=360.0)
	{
		rotation1 -= 360.0;
	}
	while(rotation2>=360.0)
	{
		rotation2 -= 360.0;
	}
	rect rotatedRect1 = rotateRect(src, rotation1);
	rect rotatedRect2 = rotateRect(src, rotation2);
	rect tmp1 = getMaxRect(src, rotatedRect1);
	rect tmp2 = getMaxRect(src, rotatedRect2);
	rect resRect = getRectIntersection(tmp1,tmp2);
	pair<int,int> res = pair<int,int>( floor(resRect.b.y), ceil(resRect.b.x));
	newHeight = floor(resRect.a.y - resRect.b.y);
	newWidth = floor(resRect.d.x - resRect.a.x);
	return res;
}
