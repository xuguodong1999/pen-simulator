#include "az/core/geometry_def.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>

using namespace az;

BOOST_GEOMETRY_REGISTER_POINT_2D(Vec2, ScalarType, cs::cartesian, coeffRef(0), coeffRef(1))
BOOST_GEOMETRY_REGISTER_LINESTRING(Vec2Array)
BOOST_GEOMETRY_REGISTER_BOX(Box2, Vec2, min(), max());
