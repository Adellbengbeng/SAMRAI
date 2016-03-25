//
// File:	CartesianPatchGeometry.h
// Package:	SAMRAI geometry package
// Copyright:	(c) 1997-2005 The Regents of the University of California
// Revision:	$Revision: 601 $
// Modified:	$Date: 2005-09-06 11:23:15 -0700 (Tue, 06 Sep 2005) $
// Description: Simple Cartesian grid geometry for an AMR hierarchy.
//

#ifndef included_geom_CartesianPatchGeometry
#define included_geom_CartesianPatchGeometry

#ifndef included_SAMRAI_config
#include "SAMRAI_config.h"
#endif
#ifndef included_hier_PatchGeometry
#include "PatchGeometry.h"
#endif
#ifndef included_hier_BoxArray
#include "BoxArray.h"
#endif
#ifndef included_hier_IntVector
#include "IntVector.h"
#endif

namespace SAMRAI {
    namespace geom {

/**
 * Class CartesianPatchGeometry implements simple Cartesian mes 
 * geometry management for a single patch in an AMR hierarchy.  The geometry is 
 * limited to the mesh increments being specified by the DIM-tuple 
 * (dx[0],...,dx[DIM-1]) associated with the patch, and the spatial 
 * coordinates of the lower and upper corners of the patch within the 
 * computational domain.  The grid data is set by CartesianGridGeometry<DIM>
 * class.  This patch geometry class is derived from hier::PatchGeometry<DIM>
 * base class.
 *
 * @see hier::BoundaryBox
 * @see hier::PatchGeometry
 * @see geom::CartesianGridGeometry
 */

template<int DIM> class CartesianPatchGeometry 
: public hier::PatchGeometry<DIM>
{
public:

   /**
    * Constructor for CartesianPatchGeometry class.  It simply passes 
    * patch boundary information to hier::PatchGeometry base class constructor
    * and allocates storage for spatial coordinates on patch.
    */
   CartesianPatchGeometry<DIM>(
      const hier::IntVector<DIM>& ratio_to_level_zero,
      const tbox::Array< tbox::Array<bool> >& touches_regular_bdry,
      const tbox::Array< tbox::Array<bool> >& touches_periodic_bdry,
      const double* dx,
      const double* x_lo,
      const double* x_hi);

   /**
    * Destructor for CartesianPatchGeometry deallocates the
    * storage for spatial coordinates on patch.
    */
   ~CartesianPatchGeometry<DIM>();

   /**
    * Return const pointer to dx array for patch. 
    */
   const double* getDx() const; 

   /**
    * Return const pointer to lower spatial coordinate for patch.
    */
   const double* getXLower() const;
 
   /**
    * Return const pointer to upper spatial coordinate for patch.
    */
   const double* getXUpper() const;

   /**
    * Print CartesianPatchGeometry class data.
    */
   void printClassData(ostream& os) const;

private:
   // These are not implemented. 
   CartesianPatchGeometry(const CartesianPatchGeometry<DIM>&); 
   void operator=(const CartesianPatchGeometry<DIM>&);

   double d_dx[DIM];           // mesh increments for patch.
   double d_x_lo[DIM];         // spatial coords of lower end of patch.
   double d_x_up[DIM];         // spatial coords of upper end of patch.

};

}
}

#ifndef DEBUG_NO_INLINE
#include "CartesianPatchGeometry.I"
#endif
#endif

#ifdef INCLUDE_TEMPLATE_IMPLEMENTATION
#include "CartesianPatchGeometry.C"
#endif