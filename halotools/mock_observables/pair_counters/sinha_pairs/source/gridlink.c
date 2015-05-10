#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "defs.h"
#include "gridlink.h"
#include "utils.h"
#include "function_precision.h"
#include "cellarray.h"

#define MEMORY_INCREASE_FAC   1.2

double get_binsize(const double xmin,const double xmax, const double rmax, const int refine_factor, const int max_ncells, int *nlattice)  __attribute__((warn_unused_result));

double get_binsize(const double xmin,const double xmax, const double rmax, const int refine_factor, const int max_ncells, int *nlattice)
{
  double xdiff = xmax-xmin;
  int nmesh=(int)(refine_factor*xdiff/rmax) ;
//#ifdef PERIODIC
//  if (nmesh<(2*refine_factor+1))  {
//    fprintf(stderr,"linklist> ERROR:  nlattice = %d is so small that with periodic wrapping the same cells will be counted twice ....exiting\n",nmesh) ;
//    exit(EXIT_FAILURE) ;
//  }
//#endif
  
  if (nmesh>max_ncells)  nmesh=max_ncells;
  double xbinsize = xdiff/nmesh;
  *nlattice = nmesh;
  return xbinsize;
}

cellarray * gridlink(const int np,
           const DOUBLE *x,const DOUBLE *y,const DOUBLE *z,
           const DOUBLE xmin, const DOUBLE xmax,
           const DOUBLE ymin, const DOUBLE ymax,
           const DOUBLE zmin, const DOUBLE zmax,
           const DOUBLE rmax,
           const int xbin_refine_factor,
           const int ybin_refine_factor,
           const int zbin_refine_factor,
           int *nlattice_x,
           int *nlattice_y,
           int *nlattice_z)
{
  cellarray *lattice=NULL;
  int ix,iy,iz;
  int nmesh_x,nmesh_y,nmesh_z;
  int *nallocated=NULL;
  DOUBLE xdiff,ydiff,zdiff;
  DOUBLE cell_volume,box_volume;
  DOUBLE xbinsize,ybinsize,zbinsize;
  int expected_n=0;
  int64_t totncells;
  struct timeval t0,t1;
  gettimeofday(&t0,NULL);

  xbinsize = get_binsize(xmin,xmax,rmax,xbin_refine_factor, NLATMAX, &nmesh_x);
  ybinsize = get_binsize(ymin,ymax,rmax,ybin_refine_factor, NLATMAX, &nmesh_y);
  zbinsize = get_binsize(zmin,zmax,rmax,zbin_refine_factor, NLATMAX, &nmesh_z);
  
  totncells = (int64_t) nmesh_x * (int64_t) nmesh_y * (int64_t) nmesh_z;

  xdiff = xmax-xmin;
  ydiff = ymax-ymin;
  zdiff = zmax-zmin;
  
  cell_volume=xbinsize*ybinsize*zbinsize;
  box_volume=xdiff*ydiff*zdiff;
  expected_n=(int)(np*cell_volume/box_volume*MEMORY_INCREASE_FAC);
  fprintf(stderr,"In %s> Running with [nmesh_x, nmesh_y, nmesh_z]  = %d,%d,%d. ",__FUNCTION__,nmesh_x,nmesh_y,nmesh_z);
  lattice    = (cellarray *) my_malloc(sizeof(cellarray), totncells);
  nallocated = (int *)       my_malloc(sizeof(int)      , totncells);

  for (int i=0;i<nmesh_x;i++) {
    for (int j=0;j<nmesh_y;j++) {
      for (int k=0;k<nmesh_z;k++) {
    int64_t index = i*nmesh_y*nmesh_z + j*nmesh_z + k;
    lattice[index].x = my_malloc(sizeof(DOUBLE),expected_n);//This allocates extra and is wasteful
    lattice[index].y = my_malloc(sizeof(DOUBLE),expected_n);
    lattice[index].z = my_malloc(sizeof(DOUBLE),expected_n);
    lattice[index].nelements=0;
    nallocated[index] = expected_n;
      }
    }
  }

  DOUBLE xinv=1.0/xbinsize;
  DOUBLE yinv=1.0/ybinsize;
  DOUBLE zinv=1.0/zbinsize;
  
  for (int i=0;i<np;i++)  {
    ix=(int)((x[i]-xmin)*xinv) ;
    iy=(int)((y[i]-ymin)*yinv) ;
    iz=(int)((z[i]-zmin)*zinv) ;
    if (ix>nmesh_x-1)  ix--;    /* this shouldn't happen, but . . . */
    if (iy>nmesh_y-1)  iy--;
    if (iz>nmesh_z-1)  iz--;
  if(! ( ix >= 0 && ix < nmesh_x && iy >=0 && iy < nmesh_y && iz >= 0 && iz < nmesh_z)) {
    fprintf(stderr,"Problem with i = %d x = %lf y = %lf z = %lf \n",i,x[i],y[i],z[i]);
    fprintf(stderr,"ix = %d iy = %d iz = %d\n",ix,iy,iz);
  }
  assert(x[i] >= xmin && x[i] <= xmax && "x-position is within limits");
  assert(y[i] >= ymin && y[i] <= ymax && "y-position is within limits");
  assert(z[i] >= zmin && z[i] <= zmax && "z-position is within limits");
  
  assert(ix >= 0 && ix < nmesh_x && "ix is in range");
    assert(iy >= 0 && iy < nmesh_y && "iy is in range");
    assert(iz >= 0 && iz < nmesh_z && "iz is in range");

  int64_t index = ix*nmesh_y*nmesh_z + iy*nmesh_z + iz;

    if(lattice[index].nelements == nallocated[index]) {
      expected_n = nallocated[index]*MEMORY_INCREASE_FAC;

    //In case expected_n is 1 or MEMORY_INCREASE_FAC is 1. 
    //This way, we only increase by a very few particles 
    // at a time. Smaller memory footprint
      if(expected_n == nallocated[index])
    expected_n += 3;

      lattice[index].x = my_realloc(lattice[index].x ,sizeof(DOUBLE),expected_n,"lattice.x");
      lattice[index].y = my_realloc(lattice[index].y ,sizeof(DOUBLE),expected_n,"lattice.y");
      lattice[index].z = my_realloc(lattice[index].z ,sizeof(DOUBLE),expected_n,"lattice.z");
      
      nallocated[index] = expected_n;
    }
    assert(lattice[index].nelements < nallocated[index] && "Ensuring that number of particles in a cell doesn't corrupt memory");
    int ipos=lattice[index].nelements;
    lattice[index].x[ipos] = x[i];
    lattice[index].y[ipos] = y[i];
    lattice[index].z[ipos] = z[i];
    lattice[index].nelements++;
  }
  free(nallocated);
  
  *nlattice_x=nmesh_x;
  *nlattice_y=nmesh_y;
  *nlattice_z=nmesh_z;
  gettimeofday(&t1,NULL);
  fprintf(stderr," Time taken = %6.2lf sec\n",ADD_DIFF_TIME(t0,t1));
  
  return lattice;
}