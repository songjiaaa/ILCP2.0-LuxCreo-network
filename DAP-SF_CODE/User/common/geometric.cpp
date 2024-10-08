/*
 * 文件名：geometric.cpp
  designed by yz , 2013-9-18
  修改时间：2015年 11月 26日 19:02:19 CST	fb
版本：	V1.1			2019-01-15
	
  功能：
	解析几何库，提供基本计算几何功能。
	使用C++，不使用STL，若有可能，向单片机\DSP移植
	基本功能：
	1、向量基本操作
	2、点、线、面的方程和生成
	3、平移、旋转变换
	4、相交、其他功能
*/

#include"geometric.h"
#include<math.h>

double deg_range(double d) //角度限制在正负180度
{
	while(d>180)
	{
		d-=360;
	}
	while(d<=-180)
	{
		d+=360;
	}
	return d;
}
double rad_range(double d) //弧度限制在正负180度
{
	while(d>PI)
	{
		d-=PI*2;
	}
	while(d<=-PI)
	{
		d+=PI*2;
	}
	return d;
}

//2D向量基本运算
#ifndef USECPP11 //4.9.2
Vector2D operator+(Vector2D p1,Vector2D p2) { return Vector2D(p1.x+p2.x,p1.y+p2.y); }
#else
Vector2D operator+(Vector2D &p1,Vector2D &p2)
{
	return Vector2D(p1.x+p2.x,p1.y+p2.y);
}
Vector2D operator+(Vector2D &&p1,Vector2D &&p2) { return p1+p2; }
Vector2D operator+(Vector2D &p1,Vector2D &&p2) { return p1+p2; }
Vector2D operator+(Vector2D &&p1,Vector2D &p2) { return p1+p2; }
#endif

#ifndef USECPP11 //4.9.2
Vector2D operator-(Vector2D p1,Vector2D p2) { return Vector2D(p1.x-p2.x,p1.y-p2.y); }
#else
Vector2D operator-(Vector2D &p1,Vector2D &p2)
{
	return Vector2D(p1.x-p2.x,p1.y-p2.y);
}
Vector2D operator-(Vector2D &&p1,Vector2D &&p2) { return p1-p2;}
Vector2D operator-(Vector2D &p1,Vector2D &&p2) { return p1-p2;}
Vector2D operator-(Vector2D &&p1,Vector2D &p2) { return p1-p2;}
#endif

#ifndef USECPP11 //4.9.2
double operator*(Vector2D p1,Vector2D p2) { return p1.x*p2.x+p1.y*p2.y; } //点乘
#else
double operator*(Vector2D &p1,Vector2D &p2) //点乘
{
	return p1.x*p2.x+p1.y*p2.y;
}
double operator*(Vector2D &&p1,Vector2D &&p2) { return p1*p2;}
double operator*(Vector2D &p1,Vector2D &&p2) { return p1*p2;}
double operator*(Vector2D &&p1,Vector2D &p2) { return p1*p2;}
#endif

Vector2D operator+(Vector2D &p1,double k)
{
	return Vector2D(p1.x+k,p1.y+k);
}
Vector2D operator-(Vector2D &p1,double k)
{
	return Vector2D(p1.x-k,p1.y-k);
}
Vector2D operator*(Vector2D &p1,double k)
{
	return Vector2D(p1.x*k,p1.y*k);
}
Vector2D operator/(Vector2D &p1,double k)
{
	return Vector2D(p1.x/k,p1.y/k);
}

#ifdef USECPP11 //4.9.2以上
Vector2D operator+(Vector2D &&p1,double k) { return p1+k; }
Vector2D operator-(Vector2D &&p1,double k) { return p1-k; }
Vector2D operator*(Vector2D &&p1,double k) { return p1*k; }
Vector2D operator/(Vector2D &&p1,double k) { return p1/k; }
#endif

double Vector2D::magnitude(void) //2D向量模运算
{
	return sqrt(x*x+y*y);
}
double Vector2D::norm(void) //2D向量模运算
{
	double mag=magnitude();
	if (fabs(mag)>Eps)
	{
		x/=mag;
		y/=mag;
	}
	return mag;
}
double Vector2D::distance(Vector2D &p1) //2D向量模运算
{
	Vector2D p=p1-*this;
	return p.norm();
}

double Vector2D::angle(Vector2D &p1) //两向量夹角
{
	return acos((p1.x*x+p1.y*y)/sqrt(p1.x*p1.x+p1.y*p1.y)/sqrt(x*x+y*y));
}
#ifdef USECPP11 //4.9.2以上
double Vector2D::distance(Vector2D &&p1) { return distance(p1);}
double Vector2D::angle(Vector2D &&p1) { return angle(p1); }
#endif

//3D向量基本运算
#ifndef USECPP11 //4.9.2
Vector3D operator+(Vector3D p1,Vector3D p2) { return Vector3D(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z); }
Vector3D operator-(Vector3D p1,Vector3D p2) { return Vector3D(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z); }
double operator*(Vector3D p1,Vector3D p2) { return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z; } //点乘
Vector3D operator^(Vector3D p1,Vector3D p2)
{
	Vector3D p=Vector3DEmpty;
	p.x=p1.y*p2.z-p1.z*p2.y;
	p.y=p1.z*p2.x-p1.x*p2.z;
	p.z=p1.x*p2.y-p1.y*p2.x;
	return p;
}
Vector3D operator+(Vector3D p1,double k)
{
	return Vector3D(p1.x+k,p1.y+k,p1.z+k);
}
Vector3D operator-(Vector3D p1,double k)
{
	return Vector3D(p1.x-k,p1.y-k,p1.z-k);
}
Vector3D operator*(Vector3D p1,double k)
{
	return Vector3D(p1.x*k,p1.y*k,p1.z*k);
}
Vector3D operator/(Vector3D p1,double k)
{
	return Vector3D(p1.x/k,p1.y/k,p1.z/k);
}
#else
Vector3D operator+(Vector3D &p1,Vector3D &p2)
{
	return Vector3D(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z);
}
Vector3D operator-(Vector3D &p1,Vector3D &p2)
{
	return Vector3D(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z);
}
double operator*(Vector3D &p1,Vector3D &p2) //点乘
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}
Vector3D operator^(Vector3D &p1,Vector3D &p2) //叉乘
{
	Vector3D p=Vector3DEmpty;
	p.x=p1.y*p2.z-p1.z*p2.y;
	p.y=p1.z*p2.x-p1.x*p2.z;
	p.z=p1.x*p2.y-p1.y*p2.x;
	return p;
}
Vector3D operator+(Vector3D &&p1,Vector3D &&p2) { return p1+p2; }
Vector3D operator+(Vector3D &p1,Vector3D &&p2) { return p1+p2; }
Vector3D operator+(Vector3D &&p1,Vector3D &p2) { return p1+p2; }
Vector3D operator-(Vector3D &&p1,Vector3D &&p2) { return p1-p2; }
Vector3D operator-(Vector3D &p1,Vector3D &&p2) { return p1-p2; }
Vector3D operator-(Vector3D &&p1,Vector3D &p2) { return p1-p2; }
double operator*(Vector3D &&p1,Vector3D &&p2) { return p1*p2; }
double operator*(Vector3D &p1,Vector3D &&p2) { return p1*p2; }
double operator*(Vector3D &&p1,Vector3D &p2) { return p1*p2; }
Vector3D operator^(Vector3D &&p1,Vector3D &&p2) { return p1^p2; }
Vector3D operator^(Vector3D &p1,Vector3D &&p2) { return p1^p2; }
Vector3D operator^(Vector3D &&p1,Vector3D &p2) { return p1^p2; }
Vector3D operator+(Vector3D &p1,double k)
{
	return Vector3D(p1.x+k,p1.y+k,p1.z+k);
}
Vector3D operator-(Vector3D &p1,double k)
{
	return Vector3D(p1.x-k,p1.y-k,p1.z-k);
}
Vector3D operator*(Vector3D &p1,double k)
{
	return Vector3D(p1.x*k,p1.y*k,p1.z*k);
}
Vector3D operator/(Vector3D &p1,double k)
{
	return Vector3D(p1.x/k,p1.y/k,p1.z/k);
}
Vector3D operator+(Vector3D &&p1,double k) { return p1+k; }
Vector3D operator-(Vector3D &&p1,double k) { return p1-k; }
Vector3D operator*(Vector3D &&p1,double k) { return p1*k; }
Vector3D operator/(Vector3D &&p1,double k) { return p1/k; }
#endif

double Vector3D::magnitude(void) //3D向量模运算
{
	return sqrt(x*x+y*y+z*z);
}
double Vector3D::norm(void) //3D向量模运算
{
	double mag=magnitude();
	if (fabs(mag)>Eps)
	{
		x/=mag;
		y/=mag;
		z/=mag;
	}
	return mag;
}
double Vector3D::distance(Vector3D &p1) //3D向量模运算
{
	Vector3D p=p1-*this;
	return p.norm();
}

double Vector3D::angle(Vector3D &p1) //两向量夹角
{
	return acos((p1.x*x+p1.y*y+p1.z*z)/sqrt(p1.x*p1.x+p1.y*p1.y+p1.z*p1.z)
		/sqrt(x*x+y*y+z*z));
}
#ifdef USECPP11 //4.9.2以上
double Vector3D::distance(Vector3D &&p1) { return distance(p1); }
double Vector3D::angle(Vector3D &&p1) { return angle(p1); }
#endif
///////////////////////////////////////////////////////////////
void Line2D::create(Vector2D &p1,Vector2D &p2) //两点生成线
{
	k=p2-p1;
	p=p1;
}
#ifdef USECPP11 //4.9.2以上
void Line2D::create(Vector2D &&p1,Vector2D &&p2) { create(p1,p2); }
void Line2D::create(Vector2D &p1,Vector2D &&p2)  { create(p1,p2); }
void Line2D::create(Vector2D &&p1,Vector2D &p2)  { create(p1,p2); }
#endif

Vector2D Line2D::vertical(void) //求此直线的垂线向量
{
	Vector2D vec;
	vec.x=-k.y;
	vec.y=k.x;
	return vec;
}
Line2D Line2D::vertical(Vector2D &p1) //求此直线的垂线（过已知点）
{
	Line2D ret;
	ret.k=vertical();
	ret.p=p1;
	return ret;
}
#ifdef USECPP11 //4.9.2以上
Line2D Line2D::vertical(Vector2D &&p1) { return vertical(p1); }
#endif

bool Line2D::cross(Line2D &l,Vector2D &p1) //求两直线的交点，false无交点
{
	double t=0; //以解方程的方法来求
	t=(k.x*l.k.y - k.y*l.k.x); //t1的系数,本身为1
	if(fabs(t)<0.000001)
	{
		return false;
	}
	t=(l.k.y*(l.p.x-p.x)+l.k.x*(p.y-l.p.y))/t;
	p1.x=k.x*t+p.x;
	p1.y=k.y*t+p.y;
	return true;
}
#ifdef USECPP11 //4.9.2以上
bool Line2D::cross(Line2D &&l,Vector2D &p1)  { return cross(l,p1); }
#endif

Vector2D Line2D::Cal_P_Dis(Vector2D &p1,double d) //在直线上，距离给定点一定距离的点。（距离有正负之分）
{
	double t;
	t=d/k.magnitude(); //然后得到参数
	return k*t+p1;//认为给定点严格在直线上
}
#ifdef USECPP11 //4.9.2以上
Vector2D Line2D::Cal_P_Dis(Vector2D &&p1,double d) { return Cal_P_Dis(p1,d); }
#endif

Vector2D Line2D::vert_point(Vector2D &p1) //点到直线的垂点
{
	Line2D lv;
	lv=vertical(p1); //求垂线
	Vector2D p;
	cross(lv,p); //求交点
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector2D Line2D::vert_point(Vector2D &&p1) { return vert_point(p1); }
#endif

double Line2D::distance(Vector2D &p1) //点到直线距离
{
	Vector2D p2=vert_point(p1); //求垂点
	return p2.distance(p1);
}
#ifdef USECPP11 //4.9.2以上
double Line2D::distance(Vector2D &&p1) { return distance(p1); }
#endif

////////////////////////////////////////////////////////////
void Line3D::create(Vector3D &p1,Vector3D &p2) //两点生成线
{
	k=p2-p1;
	p=p1;
}
#ifdef USECPP11 //4.9.2以上
void Line3D::create(Vector3D &&p1,Vector3D &&p2) { create(p1,p2); }
void Line3D::create(Vector3D &p1,Vector3D &&p2) { create(p1,p2); }
void Line3D::create(Vector3D &&p1,Vector3D &p2) { create(p1,p2); }
#endif

Line3D Line3D::vertical(Vector3D &p1) //求此直线的垂线（过已知点）
{
	Vector3D tmp;
	tmp=vert_point(p1); //求垂点
	Line3D l;
	l.create(tmp,p1);
	return l;
}
#ifdef USECPP11 //4.9.2以上
Line3D Line3D::vertical(Vector3D &&p1) { return vertical(p1); }
#endif

bool Line3D::cross(Line3D &l,Vector3D &p1) //求两直线的交点(相距最近的点)
{
	//首先求公垂线
	//列方程：K1*t1+B1 - K2*t2-B2 = Kv*tv
	//t1 t2 为最短垂线段的两个端点,K3为公垂线向量
	//方程：[k1 -k2 -kv]*[t1;t2;tv]=[b2-b1]
	//[t1;t2;tv]=[k1 -k2 -kv]^-1*[b2-b1]
	Vector3D kv;
	kv=k^l.k; //求垂线
	double d=kv.norm(); //归一化
	if (fabs(d)<Eps) //若两线平行
	{
		return false;
	}
	//解方程
	d=	k.x*(l.k.y*kv.z - l.k.z*kv.y) - 
		k.y*(l.k.x*kv.z - l.k.z*kv.x) + 
		k.z*(l.k.x*kv.y - l.k.y*kv.x); //行列式值
	double bx=l.p.x-p.x;
	double by=l.p.y-p.y;
	double bz=l.p.z-p.z; //B2-B1
	double t1=	(l.k.y*kv.z-l.k.z*kv.y)*bx +
				(l.k.z*kv.x-l.k.x*kv.z)*by +
				(l.k.x*kv.y-l.k.y*kv.x)*bz;
	double t2=	(k.y*kv.z-k.z*kv.y)*bx +
				(k.z*kv.x-k.x*kv.z)*by +
				(k.x*kv.y-k.y*kv.x)*bz;
	t1/=d;
	t2/=d;
	p1=(k*t1+p + l.k*t2+l.p)/2; //求线段中点
	return true; //算法需要求方程,以后实现
}
#ifdef USECPP11 //4.9.2以上
bool Line3D::cross(Line3D &&l,Vector3D &p1) { return cross(l,p1); }
#endif

double Line3D::line_distance(Line3D &l) //求两直线间距
{
	Vector3D kv;
	kv=k^l.k; //求垂线
	kv.norm(); //归一化
	//此方法无法求平行线
	return kv*(l.p-p); //求参数
}
#ifdef USECPP11 //4.9.2以上
double Line3D::line_distance(Line3D &&l) { return line_distance(l); }
#endif

Vector3D Line3D::Cal_P_Dis(Vector3D &p1,double d) //在直线上，距离给定点一定距离的点。（距离有正负之分）
{
	double t;
	t=d/k.magnitude(); //然后得到参数
	return k*t+p1;//认为给定点严格在直线上
}
#ifdef USECPP11 //4.9.2以上
Vector3D Line3D::Cal_P_Dis(Vector3D &&p1,double d) { return Cal_P_Dis(p1,d); }
#endif

Vector3D Line3D::vert_point(Vector3D &p1) //点到直线的垂点
{
	Surface sf;
	sf.create_vert(*this,p1); //求垂面
	Vector3D p;
	sf.cross(*this,p); //求线面交点
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector3D Line3D::vert_point(Vector3D &&p1) { return vert_point(p1); }
#endif

double Line3D::distance(Vector3D &p1) //点到直线距离
{
	Vector3D tmp;
	tmp=vert_point(p1);
	return p1.distance(tmp);
}
#ifdef USECPP11 //4.9.2以上
double Line3D::distance(Vector3D &&p1) { return distance(p1); }
#endif

///////////////////////////////////////////////////////////
void Surface::create(Line3D &l,Vector3D &p1) //点线确定面
{
	k=l.k^(p1-l.p); //叉乘取垂线
	b=k*p1;
}
#ifdef USECPP11 //4.9.2以上
void Surface::create(Line3D &&l,Vector3D &&p1) { create(l,p1); }
void Surface::create(Line3D &l,Vector3D &&p1)  { create(l,p1); }
void Surface::create(Line3D &&l,Vector3D &p1)  { create(l,p1); }
#endif

void Surface::create_vert(Line3D &l,Vector3D &p1) //点线确定面,线的垂面
{
	k=l.k;
	b=k*p1;
}
#ifdef USECPP11 //4.9.2以上
void Surface::create_vert(Line3D &&l,Vector3D &&p1) { create_vert(l,p1); }
void Surface::create_vert(Line3D &l,Vector3D &&p1) { create_vert(l,p1); }
void Surface::create_vert(Line3D &&l,Vector3D &p1) { create_vert(l,p1); }
#endif

bool Surface::cross(Line3D &l,Vector3D &p1) //线面交点，返回是否有交点
{
	//面方程：A·X=d
	//线方程：X=Kt+P
	//将X代入，A·Kt+A·P=b，求得t，进而求得X
	double AK=k*l.k;
	if (fabs(AK)<Eps)
	{
		return false;
	}
	double t=(b-k*l.p)/AK;
	p1=l.k*t+l.p;
	return true;
}
#ifdef USECPP11 //4.9.2以上
bool Surface::cross(Line3D &&l,Vector3D &p1) { return cross(l,p1); }
#endif

Vector3D Surface::vert_point(Vector3D &p1) //点面垂点
{
	Line3D l;
	l.k=k;
	l.p=p1;
	Vector3D p;
	cross(l,p);//垂线与平面的交点
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector3D Surface::vert_point(Vector3D &&p1) { return vert_point(p1); }
#endif

double Surface::distance(Vector3D &p1) //点面距
{
	Vector3D tmp;
	tmp=vert_point(p1);//求垂点
	return tmp.distance(p1);
}
#ifdef USECPP11 //4.9.2以上
double Surface::distance(Vector3D &&p1) { return distance(p1); }
#endif

///////////////////////////////////////////////////////////
//有向线段
Vector2D Segment2D::proj_point(Vector2D &p1) //点到线段距离最短的点（点到线段距离）
{//线段AB以外P点到AB最短距离的点为C
	Vector2D unit_ab=end-st; //AB单位向量
	double val_ab=unit_ab.norm();
	double val_ac=(p1-st)*unit_ab; //AP * (AB/abs(AB))
	if(val_ac>val_ab) return end; //投影点在线段终点B以外
	if(val_ac<0) return st; //投影点在线段终点B以外
	return unit_ab*val_ac+st;
}
#ifdef USECPP11 //4.9.2以上
Vector2D Segment2D::proj_point(Vector2D &&p1) { return proj_point(p1); } //点到线段距离最短的点（点到线段距离）
#endif

Vector3D Segment3D::proj_point(Vector3D &p1) //点到线段距离最短的点（点到线段距离）
{//线段AB以外P点到AB最短距离的点为C
	Vector3D unit_ab=end-st; //AB单位向量
	double val_ab=unit_ab.norm();
	double val_ac=(p1-st)*unit_ab; //AP * (AB/abs(AB))
	if(val_ac>val_ab) return end; //投影点在线段终点B以外
	if(val_ac<0) return st; //投影点在线段终点B以外
	return unit_ab*val_ac+st;
}
#ifdef USECPP11
Vector3D Segment3D::proj_point(Vector3D &&p1) { return proj_point(p1); } //点到线段距离最短的点（点到线段距离）
#endif
///////////////////////////////////////////////////////////
//2D矩阵变换
Vector2D matrix_trans2D(Vector2D &k,double t[][3])
{
	int i,s;
	Vector2D ret;
	double m[3] = {k.x,k.y,1};
	double r[3] = {0,0,0};
	for(i=0;i<3;i++)
	{
		for(s=0;s<3;s++)
		{
			r[i] = m[s] * t[s][i] + r[i];
		}
	}
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//3D矩阵变换
Vector3D matrix_trans3D(Vector3D &k,double t[][4])
{
	int i,s;
	Vector3D ret;
	double m[4] = {k.x,k.y,k.z,1};
	double r[4] = {0,0,0,0};
	for(i=0;i<4;i++)
	{
		for(s=0;s<4;s++)
		{
			r[i] = m[s] * t[s][i] + r[i];
		}
	}
	ret.x = r[0];
	ret.y = r[1];
	ret.z = r[2];
	return ret;
}

//2D向量平移变换
Vector2D translate2D(Vector2D &k,double tx,double ty)
{
	int i,s;
	Vector2D ret;
	double m[3] = {k.x,k.y,1};
	double t[3][3] = {{1,0,0},{0,1,0},{tx,ty,1}};
	double r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于(0,0)比例变换
Vector2D scale2D(Vector2D &k,double a,double d)
{
	int i,s;
	Vector2D ret;
	double m[3] = {k.x,k.y,1};
	double t[3][3] = {{a,0,0},{0,d,0},{0,0,1}};
	double r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于C(cx,cy)比例变换
Vector2D scaleToVector2D(Vector2D &k,Vector2D &c,double a,double d)
{
	int i,s;
	Vector2D ret;
	double m[3] = {k.x,k.y,1};
	double t[3][3] = {{a,0,0},{0,d,0},{c.x * (1-a),c.y * (1-d),1}};
	double r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于(0,0)旋转变换
Vector2D rotate2D(Vector2D &k,double r) //弧度
{
	return Vector2D( k.x*cos(r)-k.y*sin(r), k.x*sin(r)+k.y*cos(r));
}
#ifdef USECPP11 //4.9.2以上
Vector2D rotate2D(Vector2D &&k,double r) { return rotate2D(k,r); }
#endif

//2D相对于C(cx,cy)旋转变换
Vector2D rotateToVector2D(Vector2D &k,Vector2D &c,double r)
{
	int i,s;
	Vector2D ret;
	double m[3] = {k.x,k.y,1};
	double t[3][3] = {{cos(r),sin(r),0},{-sin(r),cos(r),0},{c.x - c.x * cos(r) + c.y * sin(r),c.y - c.x * sin(r) - c.y * cos(r),1}};
	double n[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			n[i] = m[s] * t[s][i] + n[i];
	ret.x = n[0];
	ret.y = n[1];
	return ret;
}

//3D旋转:z-x-y输入点和欧拉角,原位操作
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &euler)//弧度制
{
	double sa=sin(euler.x);
	double sb=sin(euler.y);
	double sr=sin(euler.z);
	double ca=cos(euler.x);
	double cb=cos(euler.y);
	double cr=cos(euler.z);
	return Vector3D(
		p.x*(cr*cb-sr*sa*sb	)+p.y*(-sr*ca	)+p.z*(cr*sb+sr*sa*cb	),
		p.x*(sr*cb+cr*sa*sb	)+p.y*(ca*cr	)+p.z*(sr*sb-cr*sa*cb	),
		p.x*(-ca*sb			)+p.y*(sa		)+p.z*(cb*ca	)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_zxy(p,euler); }
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_zxy(p,euler); }
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_zxy(p,euler); }
#endif

//3D旋转:z-y-x输入点和欧拉角,原位操作(航空欧拉角)
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &euler)//弧度制
{
	double sa=sin(euler.x);
	double sb=sin(euler.y);
	double sr=sin(euler.z);
	double ca=cos(euler.x);
	double cb=cos(euler.y);
	double cr=cos(euler.z);
	return Vector3D(
		p.x*(cr*cb	)+p.y*(-sr*ca+cr*sb*sa	)+p.z*(sr*sa+cr*sb*ca	),
		p.x*(sr*cb	)+p.y*(ca*cr+sr*sb*sa	)+p.z*(-cr*sa+sr*sb*ca	),
		p.x*(-sb	)+p.y*(cb*sa			)+p.z*(cb*ca			)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_zyx(p,euler); }
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_zyx(p,euler); }
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_zyx(p,euler); }
#endif

//3D旋转:x-y-z输入点和欧拉角,原位操作
Vector3D rotate3D_euler_xyz(Vector3D &p,Vector3D &euler)//弧度制
{
	double sa=sin(euler.x);
	double sb=sin(euler.y);
	double sr=sin(euler.z);
	double ca=cos(euler.x);
	double cb=cos(euler.y);
	double cr=cos(euler.z);
	return Vector3D(
		p.x*(cb*cr			)+p.y*(-cb*sr			)+p.z*(sb		),
		p.x*(ca*sr+cr*sa*sb)+p.y*(ca*cr-sa*sb*sr	)+p.z*(-cb*sa	),
		p.x*(sa*sr-ca*cr*sb)+p.y*(cr*sa+ca*sb*sr	)+p.z*(ca*cb	)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_xyz(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_xyz(p,euler); }
Vector3D rotate3D_euler_xyz(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_xyz(p,euler); }
Vector3D rotate3D_euler_xyz(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_xyz(p,euler); }
#endif

//四元数的模
double QuatVec::norm(void)
{
	double s=sqrt(w*w+x*x+y*y+z*z);
	w/=s;
	x/=s;
	y/=s;
	z/=s;
	return s;
}
//四元数的乘法
void operator*=(QuatVec &q1,QuatVec &q2)
{
	double w=q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	double x=q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	double y=q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	double z=q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	q1.w=w;
	q1.x=x;
	q1.y=y;
	q1.z=z;
	q1.norm();
}
#ifdef USECPP11 //4.9.2以上
void operator*=(QuatVec &q1,QuatVec &&q2) { q1*=q2; }
#endif

QuatVec operator*(QuatVec &q1,QuatVec &q2)
{
	QuatVec q=q1;
	q*=q2;
	return q;
}
#ifdef USECPP11 //4.9.2以上
QuatVec operator*(QuatVec &&q1,QuatVec &&q2) { return q1*q2; }
QuatVec operator*(QuatVec &q1,QuatVec &&q2) { return q1*q2; }
QuatVec operator*(QuatVec &&q1,QuatVec &q2) { return q1*q2; }
#endif

//四元数旋转,原位操作
void QuatVec::rot(Vector3D &p)
{
	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;
	double xy = x * y;
	double xz = x * z;
	double yz = y * z;
	double wx = w * x;
	double wy = w * y;
	double wz = w * z;
	double px=(1-2*(y2+z2)) * p.x + 2*(xy-wz)     * p.y + 2*(xz+wy)     * p.z;
	double py=2*(xy+wz)     * p.x + (1-2*(x2+z2)) * p.y + 2*(yz-wx)     * p.z;
	double pz=2*(xz-wy)     * p.x + 2*(yz+wx)     * p.y + (1-2*(x2+y2)) * p.z;
	p.x=px;
	p.y=py;
	p.z=pz;
}
Vector3D QuatVec::toEuler_zyx(void) //转换为欧拉角(航空欧拉角)
{
	Vector3D p;
	p.x = atan2(2 * (w * x + z * y) , 1 - 2 * (y * y + x * x));
	p.y = asin (2 * (w * y - x * z));
	p.z = atan2(2 * (w * z + y * x) , 1 - 2 * (y * y + z * z));
	return p;
}
Vector3D QuatVec::toEuler_zxy(void) //转换为欧拉角
{
	Vector3D p;
	p.x = asin(2 * (w * x + y * z));
	p.y = -atan2(2 * (x * z - w * y) , 1 - 2 * (y * y + x * x));
	p.z = -atan2(2 * (x * y - w * z) , 1 - 2 * (x * x + z * z));
	return p;
}
void QuatVec::fromEuler_zyx(Vector3D &p) //欧拉角转四元数(航空欧拉角)
{
	double cx = cos(p.x/2);
	double sx = sin(p.x/2);
	double cy = cos(p.y/2);
	double sy = sin(p.y/2);
	double cz = cos(p.z/2);
	double sz = sin(p.z/2);
	w = cx * cy * cz + sx * sy * sz;
	x = sx * cy * cz - cx * sy * sz;
	y = cx * sy * cz + sx * cy * sz;
	z = cx * cy * sz - sx * sy * cz;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_zyx(Vector3D &&p) { fromEuler_zyx(p); }
#endif

void QuatVec::fromAxis(Vector3D &axis,double angle) //从转轴和转角构造
{
	angle/=2;
	w=cos(angle);
	x=sin(angle)*axis.x;
	y=sin(angle)*axis.y;
	z=sin(angle)*axis.z;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromAxis(Vector3D &&axis,double angle) { fromAxis(axis,angle); }
#endif

//载荷旋转->载体欧拉角(航空欧拉角)输入当前欧拉角，载荷坐标系下的微小转动
void Loader2Carrier(Vector3D &cur,Vector3D &dt)//占用当前欧拉角作为输出
{
	QuatVec qcur;
	QuatVec qdt;
	//首先将两个欧拉角转换为两个四元数，看做是分别从两个物体的0位置开始旋转
	qcur.fromEuler_zyx(cur);
	qdt.fromEuler_zyx(dt);
	//计算两次旋转的结果
	qcur*=qdt;
	//Vector3D p;
	//p.x=1;p.y=0;p.z=0;
	//qcur.rot(p);
	//std::cout<<"rot: "<<"x: "<<p.x<<" y: "<<p.y<<" z: "<<p.z<<std::endl;
	//将合成的旋转转换为欧拉角输出
	cur=qcur.toEuler_zyx();
	//p.x=1;p.y=0;p.z=0;
	//rotate3D_euler_zyx(&p,&cur);
	//std::cout<<"eulerrot: "<<"x: "<<p.x<<" y: "<<p.y<<" z: "<<p.z<<std::endl;
}
////////////////////////////////////////////////////////////////
//地理算法
////////////////////////////////////////////////////////////////
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz_ellipse(Vector3D blh) //椭圆法，输入经纬度高程单位：度、米
{
	//首先转为弧度
	blh.x=deg2rad(blh.x); //L:经度
	blh.y=deg2rad(blh.y); //B:纬度
	const double a2=EARTH_R*EARTH_R; //地球半径平方
	const double b2=EARTH_R_SHORT*EARTH_R_SHORT; //地球短半轴平方
	const double e2=(a2-b2)/a2; //椭球第一偏心率的平方
	double sinB=sin(blh.y); 
	double w=sqrt(1-(e2*sinB*sinB));
	double n=EARTH_R/w;
	Vector3D tmp; //直角坐标
	tmp.x=(n+blh.z)*cos(blh.y)*cos(blh.x);
	tmp.y=(n+blh.z)*cos(blh.y)*sin(blh.x);
	tmp.z=(n*(1-e2)+blh.z)*sinB;
	return tmp;
}
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh_ellipse(Vector3D xyz) //椭圆法，输出经纬度高程单位：度、米
{
	double x2=xyz.x*xyz.x;
	double y2=xyz.y*xyz.y;
	const double a2=EARTH_R*EARTH_R; //地球半径平方
	const double b2=EARTH_R_SHORT*EARTH_R_SHORT; //地球短半轴平方
	const double e2=(a2-b2)/a2; //椭球第一偏心率的平方
	const double e12=(a2-b2)/b2; //椭球第二偏心率的平方
	double sqrt_x2_y2=sqrt(x2+y2);
	double theata=atan2(EARTH_R*xyz.z,EARTH_R_SHORT*sqrt_x2_y2);
	double sintheata=sin(theata);
	double costheata=cos(theata);
	Vector3D tmp; //经纬度
	tmp.x=atan2(xyz.y,xyz.x); //经度没变换，直接求
	tmp.y=atan2(xyz.z+EARTH_R_SHORT*e12*sintheata*sintheata*sintheata,
				sqrt_x2_y2-EARTH_R*e2*costheata*costheata*costheata);
	double sinB=sin(tmp.y);
	double n=EARTH_R/sqrt(1-(e2*sinB*sinB));
	tmp.z=sqrt_x2_y2/cos(tmp.y)-n;
	tmp.x=rad2deg(tmp.x);
	tmp.y=rad2deg(tmp.y);
	return tmp; //算法未完成
}
//简化版
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz(Vector3D blh) //输入经纬度高程单位：度、米
{
	//首先转为弧度
	blh.x=deg2rad(blh.x); //L:经度
	blh.y=deg2rad(blh.y); //B:纬度
	Vector3D tmp; //直角坐标
	double sinB=sin(blh.y);
	double cosB=cos(blh.y);
	tmp.z=(EARTH_R+blh.z); //绝对高度
	tmp.x=tmp.z*cosB*cos(blh.x);
	tmp.y=tmp.z*cosB*sin(blh.x);
	tmp.z=tmp.z*sinB;
	return tmp;
}
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh(Vector3D xyz) //输出经纬度高程单位：度、米
{
	double x2=xyz.x*xyz.x;
	double y2=xyz.y*xyz.y;
	double z2=xyz.z*xyz.z;
	Vector3D tmp; //经纬度
	tmp.x=atan2(xyz.y,xyz.x); //经度没变换，直接求
	tmp.y=atan2(xyz.z,sqrt(x2+y2)); //纬度先求角度
	tmp.z=sqrt(x2+y2+z2)-EARTH_R;
	tmp.x=rad2deg(tmp.x);
	tmp.y=rad2deg(tmp.y);
	return tmp;
}
