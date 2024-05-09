//
// Created by hiccup on 2020/5/5.
//

#pragma once
#include <boost/geometry/algorithms/make.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry.hpp>
#include <boost/operators.hpp>
#include <nlohmann/json.hpp>
#include <boost/format.hpp>
#include <string>


namespace TestingFramework{ namespace MAPF {

        namespace trans = boost::geometry::strategy::transform;
        using std::ostream;

        template <class T>
        class Point2D    // note: private inheritance is OK here!
                : boost::addable< Point2D<T>          // point + point
                , boost::subtractable< Point2D<T>     // point - point
                , boost::dividable2< Point2D<T>, T    // point / T
                , boost::multipliable2< Point2D<T>, T // point * T, T * point
                , boost::equality_comparable<Point2D<T> // point equal;
                , boost::less_than_comparable< Point2D<T> // point less equal
                > > > > > >
        {
        public:
            Point2D(){x = 0 ; y=0;};
            Point2D(T x, T y): x(x), y(y){};

            T x;
            T y;

            inline std::string print()
            {
                return "[" + std::to_string(x) + "," + std::to_string(y) + "]";
            }

            friend bool operator == (const Point2D& lhs,const Point2D& rhs)
            {
                if(std::is_integral<T>::value)
                    return lhs.x == rhs.x && lhs.y==rhs.y;
                else
                {
                    double eps = std::numeric_limits<T>::epsilon();
                    return (std::abs(rhs.x-lhs.x) < eps) && (std::abs(rhs.y - lhs.y) < eps);
                }

            }



            Point2D& operator+=(const Point2D& rhs){
                // point operator+(point, const point&) automatically
                // generated by addable.
                x+=rhs.x;
                y+=rhs.y;
                return *this;

            }

            Point2D& operator-=(const Point2D& rhs){
                // point operator-(point, const point&) automatically
                // generated by subtractable.
                x-=rhs.x;
                y-=rhs.y;
                return *this;
            }

            Point2D& operator*=(T &t)
            {
                // point operator*(point, const T&) and
                // point operator*(const T&, point) auto-generated
                // by multipliable.
                x*=t;
                y*=t;
                return *this;
            }


            Point2D& operator/=(T &rhs)
            {
                // point operator/(point, const T&) auto-generated
                // by dividable.
                x = x/ rhs;
                y = y/ rhs;
                return *this;
            }

            bool operator< (const Point2D<T> &rhs) const
            {
                //auto tmp = Point2D(this->x,this->y);
                //return (norm(*this) < norm(rhs));
                if(std::is_integral<T>::value)
                {
                    if(this->y == rhs.y)
                        return this->x < rhs.x;
                    else
                        return this->y < rhs.y ;
                }
                else
                {
                    double eps = std::numeric_limits<T>::epsilon();
                    if(std::abs(rhs.y-this->y) < eps)
                        return this->x < rhs.x;
                    else
                        return this->y < rhs.y;
                }

            }

            T norm(const Point2D<T> &p) const
            {
                return sqrt(p.x*p.x + p.y*p.y);
            }

            //clock(radian)
            //这样写的好处是可以连续操作，void的话就没法链式操作了。
            Point2D& rotate(const double angle, const Point2D<T> &center)
            {
                Point2D p(this->x,this->y);
                p -= center;
                trans::rotate_transformer<boost::geometry::radian, T, 2, 2> rotate(angle);
                boost::geometry::transform(p, *this, rotate);
                *this += center;
                return *this;
            }

            friend ostream& operator<<(ostream& out,const Point2D& point){
                out<< "["<<point.x<<","<<point.y<<"]";
                return out;
            }


        };

        using nlohmann::json;
        template <class T>
        void to_json(json& j, const Point2D<T>& p) {
            auto my_format = boost::format{"[%1%,%2%]"} % p.x % p.y;
            j = json::parse(my_format.str());
        }

        template <class T>
        void from_json(const json& j, Point2D<T>& p) {

            p.x = j[0].get<T>();
            p.y = j[1].get<T>();
        }

}
}

using FPoint2 = TestingFramework::MAPF::Point2D<double>;
//using DPoint2 = TestingFramework::MAPF::Point2D<int>;
BOOST_GEOMETRY_REGISTER_POINT_2D(FPoint2, double, cs::cartesian, x, y)
//BOOST_GEOMETRY_REGISTER_POINT_2D(DPoint2, int, cs::cartesian, x, y)



/*
 * Learning Demo
 *
// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Custom point Example

#include <iostream>

#include <boost/geometry/algorithms/distance_func.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/strategies/strategies.hpp>
#include <boost/geometry/io/dsv/write.hpp>

// Sample point, defining three color values
struct my_color_point
{
    double red, green, blue;
};

// Sample point, having an int array defined
struct my_array_point
{
    int c[3];
};

// Sample point, having x/y
struct my_2d
{
    float x,y;
};

// Sample class, protected and construction-time-only x/y,
// Can (of course) only used in algorithms which take const& points
class my_class_ro
{
public:
    my_class_ro(double x, double y) : m_x(x), m_y(y) {}
    double x() const { return m_x; }
    double y() const { return m_y; }
private:
    double m_x, m_y;
};

// Sample class using references for read/write
class my_class_rw
{
public:
    const double& x() const { return m_x; }
    const double& y() const { return m_y; }
    double& x() { return m_x; }
    double& y() { return m_y; }
private:
    double m_x, m_y;
};

// Sample class using getters / setters
class my_class_gs
{
public:
    const double get_x() const { return m_x; }
    const double get_y() const { return m_y; }
    void set_x(double v) { m_x = v; }
    void set_y(double v) { m_y = v; }
private:
    double m_x, m_y;
};

// Sample point within a namespace
namespace my
{
    struct my_namespaced_point
    {
        double x, y;
    };
}



BOOST_GEOMETRY_REGISTER_POINT_3D(my_color_point, double, cs::cartesian, red, green, blue)
BOOST_GEOMETRY_REGISTER_POINT_3D(my_array_point, int, cs::cartesian, c[0], c[1], c[2])
BOOST_GEOMETRY_REGISTER_POINT_2D(my_2d, float, cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_CONST(my_class_ro, double, cs::cartesian, x(), y())
BOOST_GEOMETRY_REGISTER_POINT_2D(my_class_rw, double, cs::cartesian, x(), y())
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(my_class_gs, double, cs::cartesian, get_x, get_y, set_x, set_y)

BOOST_GEOMETRY_REGISTER_POINT_2D(my::my_namespaced_point, double, cs::cartesian, x, y)


int main()
{
    // Create 2 instances of our custom color point
    my_color_point c1 = boost::geometry::make<my_color_point>(255, 3, 233);
    my_color_point c2 = boost::geometry::make<my_color_point>(0, 50, 200);

    // The distance_func between them can be calculated using the cartesian method (=pythagoras)
    // provided with the library, configured by the coordinate_system type of the point
    std::cout << "color distance_func "
        << boost::geometry::dsv(c1) << " to "
        << boost::geometry::dsv(c2) << " is "
        << boost::geometry::distance_func(c1,c2) << std::endl;

    my_array_point a1 = {{0}};
    my_array_point a2 = {{0}};
    boost::geometry::assign_values(a1, 1, 2, 3);
    boost::geometry::assign_values(a2, 3, 2, 1);

    std::cout << "color distance_func "
        << boost::geometry::dsv(a1) << " to "
        << boost::geometry::dsv(a2) << " is "
        << boost::geometry::distance_func(a1,a2) << std::endl;

    my_2d p1 = {1, 5};
    my_2d p2 = {3, 4};
    std::cout << "float distance_func "
        << boost::geometry::dsv(p1) << " to "
        << boost::geometry::dsv(p2) << " is "
        << boost::geometry::distance_func(p1,p2) << std::endl;

    my_class_ro cro1(1, 2);
    my_class_ro cro2(3, 4);
    std::cout << "class ro distance_func "
        << boost::geometry::dsv(cro1) << " to "
        << boost::geometry::dsv(cro2) << " is "
        << boost::geometry::distance_func(cro1,cro2) << std::endl;

    my_class_rw crw1;
    my_class_rw crw2;
    boost::geometry::assign_values(crw1, 1, 2);
    boost::geometry::assign_values(crw2, 3, 4);
    std::cout << "class r/w distance_func "
        << boost::geometry::dsv(crw1) << " to "
        << boost::geometry::dsv(crw2) << " is "
        << boost::geometry::distance_func(crw1,crw2) << std::endl;

    my_class_gs cgs1;
    my_class_gs cgs2;
    boost::geometry::assign_values(cgs1, 1, 2);
    boost::geometry::assign_values(cgs2, 3, 4);
    std::cout << "class g/s distance_func "
        << boost::geometry::dsv(crw1) << " to "
        << boost::geometry::dsv(crw2) << " is "
        << boost::geometry::distance_func(cgs1,cgs2) << std::endl;

    my::my_namespaced_point nsp1 = boost::geometry::make<my::my_namespaced_point>(1, 2);
    my::my_namespaced_point nsp2 = boost::geometry::make<my::my_namespaced_point>(3, 4);
    std::cout << "namespaced distance_func "
        << boost::geometry::dsv(nsp1) << " to "
        << boost::geometry::dsv(nsp2) << " is "
        << boost::geometry::distance_func(nsp1,nsp2) << std::endl;


    return 0;
}
 */