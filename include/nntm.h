#ifndef _NNTM_CLASS_H_
#define _NNTM_CLASS_H_

#include "nndb_constants.h"
#include "pmfe_types.h"
#include <gmpxx.h>
#include <vector>

namespace pmfe{
    class NNTM {
    public:
        const NNDBConstants constants;
        const dangle_mode dangles;

        NNTM(NNDBConstants constants, dangle_mode dangles);

        RNASequenceWithTables energy_tables(const RNASequence& seq) const;
        mpq_class minimum_energy(const RNASequenceWithTables& seq) const;
        RNAStructureWithScore mfe_structure(const RNASequenceWithTables& seq) const;

        const mpq_class energy(const RNAStructure& structure) const;

    protected:
        // MFE helpers
        mpq_class Ed3(int i, int j, const RNASequence& seq, bool inside = false) const;
        mpq_class Ed5(int i, int j, const RNASequence& seq, bool inside = false) const;
        mpq_class auPenalty(int i, int j, const RNASequence& seq) const;
        mpq_class eL(int i, int j, int ip, int jp, const RNASequence& seq) const;
        mpq_class eH(int i, int j, const RNASequence& seq) const;
        mpq_class eS(int i, int j, const RNASequence& seq) const;
        mpq_class calcVBI(int i, int j, const RNASequenceWithTables& seq) const;

        // Traceback helpers
        void traceW(int i, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;
        mpq_class traceV(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;
        mpq_class traceVM(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;
        mpq_class traceVBI(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;
        mpq_class traceWM(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;
        mpq_class traceWMPrime(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const;

        // Scoring helpers
        const mpq_class scoreTree(const RNAStructureTree& tree) const; // Score a whole structure tree
        const mpq_class scoreInternalNodeRecursively(const RNAStructureTree& tree, const IntervalTreeNode& node) const; // Score a given internal node of a structure tree, descending to its children
        const mpq_class eMUnpairedRegion(const RNAStructure& structure, int i1, int i2, int j1, int j2, bool is_external = false) const; // Compute the energy associated to an unpaired region in a multiloop or external loop
        const mpq_class eM(const RNAStructureTree& tree, const IntervalTreeNode& node) const; // Compute the energy associated to a multiloop node
        const mpq_class eE(const RNAStructureTree& tree) const; // Compute the energy associated to the external loop node

        const static int MAXLOOP = 30; /* The maximum loop size. */
        const static int TURN = 3; /* Minimum size of a hairpin loop. */
    };
};
#endif
