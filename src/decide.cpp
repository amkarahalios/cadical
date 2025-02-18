#include "internal.hpp"

namespace CaDiCaL {

// This function determines the next decision variable on the queue, without
// actually removing it from the decision queue, e.g., calling it multiple
// times without any assignment will return the same result.  This is of
// course used below in 'decide' but also in 'reuse_trail' to determine the
// largest decision level to backtrack to during 'restart' without changing
// the assigned variables (if 'opts.restartreusetrail' is non-zero).

int Internal::next_decision_variable_on_queue () {
  int64_t searched = 0;
  int res = queue.unassigned;

  // keep making these decisions until done
  bool doNotUseDecision = true;
  if (makeMergeDecisions)
  {
    while (doNotUseDecision && (!mergeDecisions.empty()))
    {
      int newRes = mergeDecisions.back();
      mergeDecisions.pop_back();
 
      if ((vals[vidx(newRes)] != 0) || (vals[-vidx(newRes)] != 0))
      {
        LOG("skip change because already set: %d", newRes);
      }
      else
      {
        res = newRes;
        doNotUseDecision = false;
      }
    }

    if (doNotUseDecision)
    {
      LOG("no more merge decisions required");
      makeMergeDecisions = false;
    }
  }

  if (makeMergeDecisions)
  {
    LOG("change decision lit: %d", res);
    if (mergeDecisions.empty())
    {
      makeMergeDecisions = false;
    }
  }
  else
  {
    // will need to decide on the merge literals at some point?
    // needs appropriate mapping
    int anyRes = res;
    int searchedAfter = 0;
    while (val(anyRes))
    {
      anyRes = link (anyRes).prev, searched++;
    }
    int aRes = anyRes;
    if (i2e[std::abs(anyRes)] > numColors*numLiterals)
    {
      while (val(anyRes) || i2e[std::abs(anyRes)] > numColors * numLiterals)
      {
        anyRes = link (anyRes).prev;
        searched++;
        searchedAfter++;
      }
    }

    if (anyRes)
    {
      res = anyRes;
    }
    else
    {
      res = aRes;
      searched = searched - searchedAfter;
    }
  }

  if (searched) {
    stats.searched += searched;
    update_queue_unassigned (res);
  }
  LOG ("next queue decision variable %d bumped %" PRId64 "", res, bumped (res));
  return res;
}

// This function determines the best decision with respect to score.
//
int Internal::next_decision_variable_with_best_score () {
  int res = 0;

  // keep making these decisions until done
  bool doNotUseDecision = true;
  if (makeMergeDecisions)
  {
    while (doNotUseDecision && (!mergeDecisions.empty()))
    {
      int newRes = mergeDecisions.back();
      mergeDecisions.pop_back();
 
      if ((vals[vidx(newRes)] != 0) || (vals[-vidx(newRes)] != 0))
      {
        LOG("skip change because already set: %d", newRes);
      }
      else
      {
        res = newRes;
        doNotUseDecision = false;
      }
    }

    if (doNotUseDecision)
    {
      LOG("no more merge decisions required");
      makeMergeDecisions = false;
    }
  }

  if (makeMergeDecisions)
  {
    LOG("change decision lit: %d", res);
    if (mergeDecisions.empty())
    {
      makeMergeDecisions = false;
    }
  }
  else
  {
    for (;;) {
      res = scores.front ();
      if (!val (res)) break;
      (void) scores.pop_front ();
    }
  }
  LOG ("next decision variable %d with score %g", res, score (res));
  return res;
}

int Internal::next_decision_variable () {
  if (use_scores ()) return next_decision_variable_with_best_score ();
  else               return next_decision_variable_on_queue ();
}

/*------------------------------------------------------------------------*/

// Implements phase saving as well using a target phase during
// stabilization unless decision phase is forced to the initial value
// of a phase is forced through the 'phase' option.

int Internal::decide_phase (int idx, bool target) {
  const int initial_phase = opts.phase ? 1 : -1;
  int phase = 0;
  if (force_saved_phase) phase = phases.saved[idx];
  if (!phase && opts.forcephase) phase = initial_phase;
  if (!phase) phase = phases.forced[idx];               // TODO swap?
  if (!phase && target) phase = phases.target[idx];
  if (!phase) phase = phases.saved[idx];

  // The following should not be necessary and in some version we had even
  // a hard 'COVER' assertion here to check for this.   Unfortunately it
  // triggered for some users and we could not get to the root cause of
  // 'phase' still not being set here.  The logic for phase and target
  // saving is pretty complex, particularly in combination with local
  // search, and to avoid running in such an issue in the future again, we
  // now use this 'defensive' code here, even though such defensive code is
  // considered bad programming practice.
  //
  if (!phase) phase = initial_phase;

  return phase * idx;
}

// The likely phase of an variable used in 'collect' for optimizing
// co-location of clauses likely accessed together during search.

int Internal::likely_phase (int idx) { return decide_phase (idx, false); }

/*------------------------------------------------------------------------*/

bool Internal::satisfied () {
  size_t assigned = trail.size ();
  if (propagated < assigned) return false;
  if ((size_t) level < assumptions.size () + (!!constraint.size ())) return false;
  return (assigned == (size_t) max_var);
}

// Search for the next decision and assign it to the saved phase.  Requires
// that not all variables are assigned.

int Internal::decide () {
  assert (!satisfied ());
  START (decide);
  int res = 0;
  if ((size_t) level < assumptions.size ()) {
    const int lit = assumptions[level];
    assert (assumed (lit));
    const signed char tmp = val (lit);
    if (tmp < 0) {
      LOG ("assumption %d falsified", lit);
      res = 20;
    } else if (tmp > 0) {
      LOG ("assumption %d already satisfied", lit);
      level++;
      control.push_back (Level (0, trail.size ()));
      LOG ("added pseudo decision level");
    } else {
      LOG ("deciding assumption %d", lit);
      search_assume_decision (lit);
    }
  } else if ((size_t) level == assumptions.size () && constraint.size ()) {
    bool satisfied_constraint = false;
    int unassigned_lit = 0;
    for (const auto lit : constraint) {
      const signed char tmp = val (lit);
      if (tmp < 0) {
        LOG ("constraint lit %d falsified", lit);
      } else if (tmp > 0) {
        LOG ("literal %d satisfies constraint and is implied by assumptions", lit);
        level++;
        control.push_back (Level (0, trail.size ()));
        LOG ("added pseudo decision level for constraint");
        satisfied_constraint = true;
        break;
      } else if (!unassigned_lit)
        unassigned_lit = lit;
    }
    if (!satisfied_constraint) {
      if (unassigned_lit) {
        LOG ("deciding %d to satisfy constraint", unassigned_lit);
        search_assume_decision (unassigned_lit);
      } else {
        LOG ("failing constraint");
        unsat_constraint = true;
        res = 20;
      }
    }
  } else {
    stats.decisions++;
    int idx = next_decision_variable ();
    const bool target = (opts.target > 1 || (stable && opts.target));
    int decision = decide_phase (idx, target);
    search_assume_decision (decision);
  }
  if (res) marked_failed = false;
  STOP (decide);
  return res;
}

}
