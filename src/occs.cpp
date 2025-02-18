#include "internal.hpp"

namespace CaDiCaL {

/*------------------------------------------------------------------------*/

// Occurrence lists.

void Internal::init_occs () {
  if (otab.size () < 2*vsize)
    otab.resize (2*vsize, Occs ());
  LOG ("initialized occurrence lists");
}

void Internal::reset_occs () {
  assert (occurring ());
  erase_vector (otab);
  LOG ("reset occurrence lists");
}

/*------------------------------------------------------------------------*/

// One-sided occurrence counter (each literal has its own counter).

void Internal::init_noccs () {
  assert (ntab.empty ());
  if (ntab.size () < 2*vsize)
    ntab.resize (2*vsize, 0);
  LOG ("initialized two-sided occurrence counters");
}

void Internal::reset_noccs () {
  assert (!max_var || !ntab.empty ());
  erase_vector (ntab);
  LOG ("reset two-sided occurrence counters");

  makeMergeDecisions = false;
  mergeDecisions.clear();
}

}
