/*
 * 文件名：geometric.h
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
 
	使用的方程：
				2D			3D
	直线方程：y=kx+b	x=Kx*t+X0 |
						y=Ky*t+Y0 |-> X=Kt+X0
						z=Kz*t+Z0 | 
	面方程：|	X=K1t1 + K2t2 + X0 
			|	AX=b	A为面的垂线向量
	四元数的基本数学方程为:
	q = cos (a/2) + i(x * sin(a/2)) + j(y * sin(a/2)) + k(z * sin(a/2))
	其中a表示旋转角度，(x,y,z)表示旋转轴。
*/

#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include "main.h"

const double PI=3.1415926; //PI
#define deg2rad(a)	((a)*(PI/180.0))
#define rad2deg(a)	((a)*(180.0f/PI))

double deg_range(double d); //角度限制在正负180度
double rad_range(double d); //弧度限制在正负180度

//定义2D点结构
struct Vector2D
{
	double x;
	double y;
	Vector2D(){}
	Vector2D(double xx,double yy)
	{
		x=xx;y=yy;
	}
	double magnitude(void); //取模
	double norm(void); //对自己进行归一化
	double distance(Vector2D &p1); //距离
	double angle(Vector2D &p1); //两向量夹角
#ifdef USECPP11 //4.9.2以上
	double distance(Vector2D &&p1); //距离
	double angle(Vector2D &&p1); //两向量夹角
#endif
};
const Vector2D Vector2DEmpty(0,0);

Vector2D operator+(Vector2D &p1,double k);
Vector2D operator-(Vector2D &p1,double k);
Vector2D operator*(Vector2D &p1,double k);
Vector2D operator/(Vector2D &p1,double k);

#ifndef USECPP11 //4.9.2
Vector2D operator+(Vector2D p1,Vector2D p2);
Vector2D operator-(Vector2D p1,Vector2D p2);
double operator*(Vector2D p1,Vector2D p2); //点乘
#else
Vector2D operator+(Vector2D &p1,Vector2D &p2);
Vector2D operator+(Vector2D &&p1,Vector2D &&p2);
Vector2D operator+(Vector2D &p1,Vector2D &&p2);
Vector2D operator+(Vector2D &&p1,Vector2D &p2);
Vector2D operator-(Vector2D &&p1,Vector2D &&p2);
Vector2D operator-(Vector2D &p1,Vector2D &&p2);
Vector2D operator-(Vector2D &&p1,Vector2D &p2);
Vector2D operator-(Vector2D &p1,Vector2D &p2);
double operator*(Vector2D &p1,Vector2D &p2); //点乘
double operator*(Vector2D &&p1,Vector2D &&p2); //点乘
double operator*(Vector2D &p1,Vector2D &&p2); //点乘
double operator*(Vector2D &&p1,Vector2D &p2); //点乘
Vector2D operator+(Vector2D &&p1,double k);
Vector2D operator-(Vector2D &&p1,double k);
Vector2D operator*(Vector2D &&p1,double k);
Vector2D operator/(Vector2D &&p1,double k);
#endif

//定义3D点结构
struct Vector3D
{
	double x;
	double y;
	double z;
	Vector3D(){}
	Vector3D(double xx,double yy,double zz)
	{
		x=xx;y=yy;z=zz;
	}
	double magnitude(void); //取模
	double norm(void); //对自己进行归一化
	double distance(Vector3D &p1); //距离
	double angle(Vector3D &p1); //两向量夹角
#ifdef USECPP11 //4.9.2以上
	double distance(Vector3D &&p1); //距离
	double angle(Vector3D &&p1); //两向量夹角
#endif
};
const Vector3D Vector3DEmpty(0,0,0);

#ifndef USECPP11 //4.9.2
Vector3D operator+(Vector3D p1,Vector3D p2);
Vector3D operator-(Vector3D p1,Vector3D p2);
double operator*(Vector3D p1,Vector3D p2); //点乘
Vector3D operator^(Vector3D p1,Vector3D p2);
Vector3D operator+(Vector3D p1,double k);
Vector3D operator-(Vector3D p1,double k);
Vector3D operator*(Vector3D p1,double k);
Vector3D operator/(Vector3D p1,double k);
#else
Vector3D operator+(Vector3D &p1,Vector3D &p2);
Vector3D operator+(Vector3D &&p1,Vector3D &&p2);
Vector3D operator+(Vector3D &p1,Vector3D &&p2);
Vector3D operator+(Vector3D &&p1,Vector3D &p2);
Vector3D operator-(Vector3D &p1,Vector3D &p2);
Vector3D operator-(Vector3D &&p1,Vector3D &&p2);
Vector3D operator-(Vector3D &p1,Vector3D &&p2);
Vector3D operator-(Vector3D &&p1,Vector3D &p2);
double operator*(Vector3D &p1,Vector3D &p2); //点乘
double operator*(Vector3D &&p1,Vector3D &&p2); //点乘
double operator*(Vector3D &p1,Vector3D &&p2); //点乘
double operator*(Vector3D &&p1,Vector3D &p2); //点乘
Vector3D operator^(Vector3D &p1,Vector3D &p2); //叉乘
Vector3D operator^(Vector3D &&p1,Vector3D &&p2); //叉乘
Vector3D operator^(Vector3D &p1,Vector3D &&p2); //叉乘
Vector3D operator^(Vector3D &&p1,Vector3D &p2); //叉乘
Vector3D operator+(Vector3D &p1,double k);
Vector3D operator+(Vector3D &&p1,double k);
Vector3D operator-(Vector3D &p1,double k);
Vector3D operator-(Vector3D &&p1,double k);
Vector3D operator*(Vector3D &p1,double k);
Vector3D operator*(Vector3D &&p1,double k);
Vector3D operator/(Vector3D &p1,double k);
Vector3D operator/(Vector3D &&p1,double k);
#endif

//定义2D线结构
struct Line2D
{
	Vector2D k;
	Vector2D p;
	void create(Vector2D &p1,Vector2D &p2); //两点生成线
	Vector2D vertical(void); //求此直线的垂线向量
	Line2D vertical(Vector2D &p1); //求此直线的垂线（过已知点）
	bool cross(Line2D &l,Vector2D &p1); //求两直线的交点
	Vector2D Cal_P_Dis(Vector2D &p1,double d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector2D vert_point(Vector2D &p1); //点到直线的垂点
	double distance(Vector2D &p1); //点到直线距离

#ifdef USECPP11 //4.9.2以上
	void create(Vector2D &&p1,Vector2D &&p2); //两点生成线
	void create(Vector2D &p1,Vector2D &&p2); //两点生成线
	void create(Vector2D &&p1,Vector2D &p2); //两点生成线
	Line2D vertical(Vector2D &&p1); //求此直线的垂线（过已知点）
	bool cross(Line2D &&l,Vector2D &p1); //求两直线的交点
	Vector2D Cal_P_Dis(Vector2D &&p1,double d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector2D vert_point(Vector2D &&p1); //点到直线的垂点
	double distance(Vector2D &&p1); //点到直线距离
#endif
};
const Line2D Line2DEmpty={Vector2DEmpty,Vector2DEmpty};

//定义3D线结构
struct Line3D
{
	Vector3D k;
	Vector3D p;
	void create(Vector3D &p1,Vector3D &p2); //两点生成线
	Line3D vertical(Vector3D &p1); //求此直线的垂线（过已知点）
	bool cross(Line3D &l,Vector3D &p1); //求两直线的交点(相距最近的点)
	double line_distance(Line3D &l); //求两直线间距
	Vector3D Cal_P_Dis(Vector3D &p1,double d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector3D vert_point(Vector3D &p1); //点到直线的垂点
	double distance(Vector3D &p1); //点到直线距离

#ifdef USECPP11 //4.9.2以上
	void create(Vector3D &&p1,Vector3D &&p2); //两点生成线
	void create(Vector3D &p1,Vector3D &&p2); //两点生成线
	void create(Vector3D &&p1,Vector3D &p2); //两点生成线
	Line3D vertical(Vector3D &&p1); //求此直线的垂线（过已知点）
	bool cross(Line3D &&l,Vector3D &p1); //求两直线的交点(相距最近的点)
	double line_distance(Line3D &&l); //求两直线间距
	Vector3D Cal_P_Dis(Vector3D &&p1,double d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector3D vert_point(Vector3D &&p1); //点到直线的垂点
	double distance(Vector3D &&p1); //点到直线距离
#endif
};
const Line3D Line3DEmpty={Vector3DEmpty,Vector3DEmpty};

//定义面结构
struct Surface
{
	Vector3D k;
	double b;
	void create(Line3D &l,Vector3D &p1); //点线确定面
	void create_vert(Line3D &l,Vector3D &p1); //点线确定面,线的垂面
	bool cross(Line3D &l,Vector3D &p1); //线面交点，返回是否有交点
	Vector3D vert_point(Vector3D &p1); //点面垂点
	double distance(Vector3D &p1); //点面距

#ifdef USECPP11 //4.9.2以上
	void create(Line3D &&l,Vector3D &&p1); //点线确定面
	void create(Line3D &l,Vector3D &&p1); //点线确定面
	void create(Line3D &&l,Vector3D &p1); //点线确定面
	void create_vert(Line3D &&l,Vector3D &&p1); //点线确定面,线的垂面
	void create_vert(Line3D &l,Vector3D &&p1); //点线确定面,线的垂面
	void create_vert(Line3D &&l,Vector3D &p1); //点线确定面,线的垂面
	bool cross(Line3D &&l,Vector3D &p1); //线面交点，返回是否有交点
	Vector3D vert_point(Vector3D &&p1); //点面垂点
	double distance(Vector3D &&p1); //点面距
#endif
};
const Surface SurfaceEmpty={Vector3DEmpty,0};

//定义2D有向线段
struct Segment2D
{
	Vector2D st;
	Vector2D end;
	Segment2D(){}
	Segment2D(const Vector2D &p1,const Vector2D &p2)
	{
		st=p1;
		end=p2;
	}
	Vector2D proj_point(Vector2D &p1); //点到线段距离最短的点（点到线段距离）
#ifdef USECPP11 //4.9.2以上
	Segment2D(Vector2D &&p1,Vector2D &&p2):Segment2D(p1,p2) {} //两点生成线
	Segment2D(Vector2D &p1,Vector2D &&p2):Segment2D(p1,p2) {} //两点生成线
	Segment2D(Vector2D &&p1,Vector2D &p2):Segment2D(p1,p2) {} //两点生成线
	Vector2D proj_point(Vector2D &&p1); //点到线段距离最短的点（点到线段距离）
#endif
};
const Segment2D Segment2DEmpty(Vector2DEmpty,Vector2DEmpty);

//定义3D有向线段
struct Segment3D
{
	Vector3D st;
	Vector3D end;
	Segment3D(){}
	Segment3D(const Vector3D &p1,const Vector3D &p3)
	{
		st=p1;
		end=p3;
	}
	Vector3D proj_point(Vector3D &p1); //点到线段距离最短的点（点到线段距离）
#ifdef USECPP11
	Segment3D(Vector3D &&p1,Vector3D &&p3):Segment3D(p1,p3) {} //两点生成线
	Segment3D(Vector3D &p1,Vector3D &&p3):Segment3D(p1,p3) {} //两点生成线
	Segment3D(Vector3D &&p1,Vector3D &p3):Segment3D(p1,p3) {} //两点生成线
	Vector3D proj_point(Vector3D &&p1); //点到线段距离最短的点（点到线段距离）
#endif
};
const Segment3D Segment3DEmpty(Vector3DEmpty,Vector3DEmpty);

//四元数
struct QuatVec
{
	double w;
	double x;
	double y;
	double z;
	double norm(void); //对自己进行归一化
	void rot(Vector3D &p); //旋转一个点
	Vector3D toEuler_zyx(void); //转换为欧拉角(航空欧拉角)
	Vector3D toEuler_zxy(void); //转换为欧拉角
	void fromEuler_zyx(Vector3D &p); //欧拉角转四元数(航空欧拉角)
	void fromAxis(Vector3D &axis,double angle); //从转轴和转角构造

#ifdef USECPP11 //4.9.2以上
	void fromEuler_zyx(Vector3D &&p); //欧拉角转四元数(航空欧拉角)
	void fromAxis(Vector3D &&axis,double angle); //从转轴和转角构造
#endif
};

void operator*=(QuatVec &q1,QuatVec &q2);
QuatVec operator*(QuatVec &q1,QuatVec &q2);

#ifdef USECPP11 //4.9.2以上
void operator*=(QuatVec &q1,QuatVec &&q2);
QuatVec operator*(QuatVec &&q1,QuatVec &&q2);
QuatVec operator*(QuatVec &q1,QuatVec &&q2);
QuatVec operator*(QuatVec &&q1,QuatVec &q2);
#endif

Vector2D matrix_trans2D(Vector2D &k,double t[][3]); //2D矩阵变换
Vector3D matrix_trans3D(Vector3D &k,double t[][4]); //3D矩阵变换
Vector2D translate2D(Vector2D &k,double tx,double ty); //2D向量平移变换
Vector2D scale2D(Vector2D &k,double a,double d); //2D相对于(0,0)比例变换
Vector2D scaleToVector2D(Vector2D &k,Vector2D &c,double a,double d); //2D相对于C(cx,cy)比例变换

Vector2D rotate2D(Vector2D &k,double r); //2D相对于(0,0)旋转变换 //弧度
#ifdef USECPP11 //4.9.2以上
Vector2D rotate2D(Vector2D &&k,double r); //2D相对于(0,0)旋转变换 //弧度
#endif

Vector2D rotateToVector2D(Vector2D &k,Vector2D &c,double r); //2D相对于C(cx,cy)旋转变换
//3D旋转:y-x-z输入点和欧拉角,原位操作
//Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &euler);//弧度制
//Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &&euler);//弧度制
//Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &&euler);//弧度制
//Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &euler);//弧度制
//3D旋转:z-x-y输入点和欧拉角,原位操作
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &euler);//弧度制
//3D旋转:z-y-x输入点和欧拉角,原位操作(航空欧拉角)
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &euler);//弧度制
//3D旋转:x-y-z输入点和欧拉角,原位操作
Vector3D rotate3D_euler_xyz(Vector3D &p,Vector3D &euler);//弧度制
//载荷旋转->载体欧拉角(航空欧拉角)输入当前欧拉角，载荷坐标系下的微小转动
void Loader2Carrier(Vector3D &cur,Vector3D &dt);//占用当前欧拉角作为输出

#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &euler);//弧度制
Vector3D rotate3D_euler_xyz(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_xyz(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_xyz(Vector3D &&p,Vector3D &euler);//弧度制
#endif

////////////////////////////////////////////////////////////////
//地理算法
////////////////////////////////////////////////////////////////
const double EARTH_R=6378137.0; //地球半径
const double EARTH_R_SHORT=6356752.0; //地球短半轴

#define wgs2xy(x)	((x)*EARTH_R*PI/180) //经纬度转距离
#define xy2wgs(x)	((x)/EARTH_R/PI*180) //距离转经纬度

//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz_ellipse(Vector3D blh); //椭圆法，输入经纬度高程单位：度、米
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh_ellipse(Vector3D xyz); //椭圆法，输出经纬度高程单位：度、米
//简化版
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz(Vector3D blh); //输入经纬度高程单位：度、米
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh(Vector3D xyz); //输出经纬度高程单位：度、米

# endif

