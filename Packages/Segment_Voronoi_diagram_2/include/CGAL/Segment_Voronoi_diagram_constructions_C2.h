#ifndef CGAL_SEGMENT_VORONOI_DIAGRAM_CONSTRUCTIONS_C2_H
#define CGAL_SEGMENT_VORONOI_DIAGRAM_CONSTRUCTIONS_C2_H

#include <CGAL/basic.h>
#include <CGAL/enum.h>

#include <CGAL/predicates/Segment_Voronoi_diagram_vertex_C2.h>

#include <CGAL/Parabola_2.h>
#include <CGAL/Parabola_segment_2.h>


CGAL_BEGIN_NAMESPACE


//***********************************************************************
//***********************************************************************
//                            CONSTRUCTIONS
//***********************************************************************
//***********************************************************************


//-----------------------------------------------------------------------
//                  Segment Voronoi diagram vertex
//-----------------------------------------------------------------------

template<class K, class M>
class Construct_svd_vertex_2
{
public:
  typedef typename K::Site_2                Site_2;
  typedef CGAL::Svd_voronoi_vertex_2<K,M>   Voronoi_vertex_2;
  typedef typename K::Point_2               Point_2;
  
public:
  Point_2 operator()(const Site_2& s1, const Site_2& s2,
		     const Site_2& s3) const
  {
    Voronoi_vertex_2 v(s1, s2, s3);
    return v.point();
  }
};


//-----------------------------------------------------------------------
//                  Segment Voronoi diagram circle
//-----------------------------------------------------------------------


template<class Gt, class M>
class Construct_svd_circle_2
{
public:
  typedef typename Gt::Site_2                 Site_2;
  typedef Svd_voronoi_vertex_2<Gt,M>          Voronoi_vertex_2;
  typedef typename Gt::Circle_2               Circle_2;

  Circle_2 operator() (const Site_2& s1, const Site_2& s2,
		       const Site_2& s3) const
  {
    Voronoi_vertex_2 v(s1, s2, s3);
    return v.circle();
  }
};



//-----------------------------------------------------------------------
//                    Segment Voronoi diagram bisector
//-----------------------------------------------------------------------


template<class Gt, class M>
class Construct_svd_bisector_2
{
public:
  typedef typename Gt::Site_2        Site_2;
  typedef typename Gt::Point_2       Point_2;
  typedef typename Gt::Line_2        Line_2;

  Line_2 operator()(const Site_2& p, const Site_2& q) const
  {
    CGAL_assertion( !(p.is_segment() && q.is_segment()) );

    if ( p.is_point() && q.is_point() ) {
      Point_2 mid = midpoint(p.point(), q.point());
      Line_2 l(p.point(), q.point());
      return l.perpendicular(mid);
    }
    if ( p.is_segment() && q.is_point() ) {
      // in this case q has to be one of the two endpoints of the
      // segment p...
      Line_2 l = p.segment().supporting_line();
      return l.perpendicular(q.point());
    }
    // in this case p has to be one of the two endpoints of the
    // segment q...
    Line_2 l = q.segment().supporting_line();
    return l.perpendicular(p.point());
  }
};

//-----------------------------------------------------------------------
//                 Segment Voronoi diagram bisector ray
//-----------------------------------------------------------------------

template<class Gt, class M >
class Construct_svd_bisector_ray_2
{
public:
  typedef typename Gt::Site_2             Site_2;
  typedef typename Gt::Point_2            Point_2;
  typedef typename Gt::Line_2             Line_2;
  typedef typename Gt::Ray_2              Ray_2;
  typedef Construct_svd_vertex_2<Gt,M>    Construct_svd_vertex_2;

  Ray_2 operator()(const Site_2& p, const Site_2& q,
		   const Site_2& r) const
  {
    CGAL_assertion( !(p.is_segment() && q.is_segment()) );

    Point_2 v = Construct_svd_vertex_2()(p, q, r);
    Point_2 p1, p2;
    if ( p.is_point() && q.is_point() ) {
      p1 = q.point();
      p2 = p.point();
    } else if ( p.is_point() && q.is_segment() ) {
      CGAL_assertion( p.point() == q.source() ||
		      p.point() == q.target() );
      p1 = (p.point() == q.source()) ? q.target() : q.source();
      p2 = p.point();
    } else {
      // p is a segment and q a point
      p1 = q.point();
      p2 = (q.point() == p.source()) ? p.target() : p.source();
    }
    Line_2 l(p1, p2);
    //    Point base = p.is_point() ? p.point() : q.point();
    Line_2 lperp = l.perpendicular( v );
    return Ray_2(v, lperp.direction());
  }
};


//-----------------------------------------------------------------------
//              Segment Voronoi diagram bisector segment
//-----------------------------------------------------------------------


template<class Gt, class M>
class Construct_svd_bisector_segment_2
{
public:
  typedef typename Gt::Site_2                 Site_2;
  typedef typename Gt::Point_2                Point_2;
  typedef typename Gt::Line_2                 Line_2;
  typedef typename Gt::Ray_2                  Ray_2;
  typedef typename Gt::Segment_2              Segment_2;
  typedef CGAL::Parabola_segment_2<Gt>        Parabola_segment_2;

  typedef Construct_svd_vertex_2<Gt,M>        Construct_svd_vertex_2;
  typedef typename Gt::Are_same_points_2      Are_same_points_2;

  Object operator()(const Site_2& p, const Site_2& q,
		    const Site_2& r, const Site_2& s) const
  {
    Construct_svd_vertex_2 circumcenter;
    Point_2 vpqr = circumcenter(p, q, r);
    Point_2 vqps = circumcenter(q, p, s);


    Are_same_points_2 are_same_points;

    if ( (p.is_point() && q.is_point()) ||
	 (p.is_segment() && q.is_segment()) ) {
      Segment_2 vorseg(vpqr, vqps);
      return CGAL::make_object(vorseg);
    }
    if ( p.is_point() ) {
      // check is p is an endpoint of q
      if (  are_same_points( p.point(), q.segment().source() ) ||
	    are_same_points( p.point(), q.segment().target() )  ) {
	Segment_2 vorseg(vpqr, vqps);
	return CGAL::make_object(vorseg);
      }
      Line_2 l = q.segment().supporting_line();
      Parabola_segment_2 vorseg(p.point(), l, vpqr, vqps);
      return CGAL::make_object(vorseg);
    }
    // check is q is an endpoint of p
    if ( q.point() == p.segment().source() ||
	 q.point() == p.segment().target() ) {
      Segment_2 vorseg(vpqr, vqps);
      return CGAL::make_object(vorseg);
    }
    Line_2 l = p.segment().supporting_line();
    Parabola_segment_2 vorseg(q.point(), l, vpqr, vqps);
    return CGAL::make_object(vorseg);
  }
};

//-----------------------------------------------------------------------


CGAL_END_NAMESPACE



#endif // CGAL_SEGMENT_VORONOI_DIAGRAM_CONSTRUCTIONS_C2_H
