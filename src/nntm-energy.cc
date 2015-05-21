// Copyright (c) 2015 Andrew Gainer-Dewar

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "nntm.h"
#include "nndb_constants.h"
#include "thread_pool.h"

#include <gmpxx.h>
#include <boost/bind.hpp>

#include <deque>

namespace pmfe {
    NNTM::NNTM(const NNDBConstants& constants, dangle_mode dangles, SimpleThreadPool& thread_pool):
        constants(constants),
        dangles(dangles),
        thread_pool(thread_pool)
    {};

    RNASequenceWithTables NNTM::energy_tables(const RNASequence& inseq) const {
        /*
          Construct the energy tables for the DP algorithm
         */
        RNASequenceWithTables seq(inseq, constants.INFINITY_);

        // Populate V, VM, VBI, WM, and WMPrime
        for (int b = TURN+1; b <= seq.len() - 1; ++b) {
            SimpleJobGroup job_group(thread_pool);
            for (int i = 0; i <= seq.len() - 1 - b; ++i) {
                job_group.post(boost::bind(&NNTM::populate_internal_tables, this, i, i+b, std::ref(seq)));
            }

            job_group.wait_for_all_jobs();
        }

        // Populate W
        for (int j = TURN+1; j <= seq.len() - 1; ++j) {
            std::deque<mpq_class> w_vals;
            w_vals.push_back(constants.INFINITY_);

            for (int i = 0; i < j-TURN; i++) {
                mpq_class Wim1;
                if (i > 0) {
                    Wim1 = seq.W[i-1];
                } else {
                    Wim1 = 0;
                }

                if (dangles == BOTH_DANGLE) { // -d2 option
                    mpq_class Widjd = seq.V[i][j] + auPenalty(i, j, seq) + Wim1;
                    if (i > 0) {
                        Widjd += Ed5(i, j, seq);
                    }

                    if (j < seq.len() - 1) {
                        Widjd += Ed3(i, j, seq);
                    }

                    w_vals.push_back(Widjd);
                } else if (dangles == NO_DANGLE) { // -d0 option
                    w_vals.push_back(seq.V[i][j] + auPenalty(i, j, seq) + Wim1);
                } else { // default
                    w_vals.push_back(seq.V[i][j] + auPenalty(i, j, seq) + Wim1);
                    w_vals.push_back(seq.V[i+1][j] + auPenalty(i+1, j, seq) + Ed5(i+1, j, seq) + Wim1);
                    w_vals.push_back(seq.V[i][j-1] + auPenalty(i, j-1, seq) + Ed3(i, j-1, seq) + Wim1);
                    w_vals.push_back(seq.V[i+1][j-1] + auPenalty(i+1, j-1, seq) + Ed5(i+1, j-1, seq) + Ed3(i+1, j-1, seq) + Wim1);
                }
            }

            w_vals.push_back(seq.W[j-1]); // Base j is free
            w_vals.push_back(0); // All bases up to j are free

            seq.W[j] = *std::min_element(w_vals.begin(), w_vals.end());
        }

        return seq;
    };

    void NNTM::populate_internal_tables(int i, int j, RNASequenceWithTables& seq) const {
        // Input specification
        assert (0 <= i);
        assert (j < seq.len());
        assert (i < j);

        if (seq.can_pair(i, j)) {
            std::deque<mpq_class> vm_vals;
            vm_vals.push_back(constants.INFINITY_);

            mpq_class d3, d5;
            d3 = Ed3(i, j, seq, true);
            d5 = Ed5(i, j, seq, true);

            if (dangles == BOTH_DANGLE) { // -d2
                vm_vals.push_back(seq.WMPrime[i+1][j-1] + d3 + d5 + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2]);
            } else if (dangles == NO_DANGLE) { // -d0
                vm_vals.push_back(seq.WMPrime[i+1][j-1] + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2]);
            } else { // default
                vm_vals.push_back(seq.WMPrime[i+1][j-1] + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2]);
                vm_vals.push_back(seq.WMPrime[i+2][j-1] + d5 + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2] + constants.multConst[1]);
                vm_vals.push_back(seq.WMPrime[i+1][j-2] + d3 + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2] + constants.multConst[1]);
                vm_vals.push_back(seq.WMPrime[i+2][j-2] + d3 + d5 + auPenalty(i, j, seq) + constants.multConst[0] + constants.multConst[2] + 2*constants.multConst[1]);
            }

            seq.VM[i][j] = *std::min_element(vm_vals.begin(), vm_vals.end());

            std::deque<mpq_class> v_vals;
            v_vals.push_back(constants.INFINITY_);
            v_vals.push_back(seq.VM[i][j]);

            v_vals.push_back(eH(i, j, seq));
            v_vals.push_back(eS(i, j, seq) + seq.V[i+1][j-1]);

            seq.VBI[i][j] = calcVBI(i, j, seq);
            v_vals.push_back(seq.VBI[i][j]);

            seq.V[i][j] = *std::min_element(v_vals.begin(), v_vals.end());
        } else {
            seq.V[i][j] = constants.INFINITY_;
        }

        std::deque<mpq_class> wmp_vals;
        wmp_vals.push_back(constants.INFINITY_);

        for (int h = i+TURN+1 ; h <= j-TURN-2; ++h) {
            wmp_vals.push_back(seq.WM[i][h] + seq.WM[h+1][j]);
        }

        seq.WMPrime[i][j] = *std::min_element(wmp_vals.begin(), wmp_vals.end());

        // WM begin
        std::deque<mpq_class> wm_vals;
        wm_vals.push_back(constants.INFINITY_);
        wm_vals.push_back(seq.WMPrime[i][j]);

        if (dangles == BOTH_DANGLE) {
            mpq_class energy = seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2];

            if (i > 0) {
                energy += Ed5(i, j, seq);
            }

            if (j < seq.len() - 1) {
                energy += Ed3(i, j, seq);
            }

            wm_vals.push_back(energy);
        } else if (dangles == NO_DANGLE) {
            wm_vals.push_back(seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2]);
        } else { // default
            wm_vals.push_back(seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2]); // no dangle

            wm_vals.push_back(seq.V[i+1][j] + Ed5(i+1, j, seq) + auPenalty(i+1, j, seq) + constants.multConst[2] + constants.multConst[1]); //i dangle

            wm_vals.push_back(seq.V[i][j-1] + Ed3(i, j-1, seq) + auPenalty(i, j-1, seq) + constants.multConst[2] + constants.multConst[1]);  //j dangle

            wm_vals.push_back(seq.V[i+1][j-1] + Ed5(i+1, j-1, seq) + Ed3(i+1, j-1, seq) + auPenalty(i+1, j-1, seq) + constants.multConst[2] + 2*constants.multConst[1]); //i,j dangle
        }

        wm_vals.push_back(seq.WM[i+1][j] + constants.multConst[1]); //i dangle

        wm_vals.push_back(seq.WM[i][j-1] + constants.multConst[1]); //j dangle

        seq.WM[i][j] = *std::min_element(wm_vals.begin(), wm_vals.end());
        // WM end

        // FM begin
        std::deque<mpq_class> fm1_vals;
        fm1_vals.push_back(constants.INFINITY_);

        int minl = i+TURN+1;
        for (int l = minl; l <= j; ++l) {
            switch (dangles) {
            case NO_DANGLE:
            {
                fm1_vals.push_back(seq.V[i][i] + auPenalty(i, l, seq) + constants.multConst[1]*(j-l) + constants.multConst[2]);
                break;
            }

            case CHOOSE_DANGLE:
            {
                mpq_class d5 = Ed5(i+1, l, seq);
                mpq_class d3 = Ed3(i, l-1, seq);
                mpq_class d53 = Ed5(i+1, l-1, seq) + Ed3(i+1, l-1, seq);

                fm1_vals.push_back(seq.V[i][l] + auPenalty(i, l, seq) + constants.multConst[1] * (j-l) + constants.multConst[2]);

                if (l >= minl + 1)
                    fm1_vals.push_back(seq.V[i+1][l] + auPenalty(i+1, l, seq) + d5 + constants.multConst[1] * (j-l+1) + constants.multConst[2]);

                if (l >= minl + 1)
                    fm1_vals.push_back(seq.V[i][l-1] + auPenalty(i, l-1, seq) + d3 + constants.multConst[1] * (j-(l-1)) + constants.multConst[2]);

                if (l >= minl + 2)
                    fm1_vals.push_back(seq.V[i+1][l-1] + auPenalty(i+1, l-1, seq) + d53 + constants.multConst[1]* (j-(l-1)+1) + constants.multConst[2]);

                break;
            }

            case BOTH_DANGLE:
            {
                mpq_class d5 = Ed5(i, l, seq);
                mpq_class d3 = Ed3(i, l, seq);
                fm1_vals.push_back(seq.V[i][l] + auPenalty(i, l, seq) + d5 + d3 + constants.multConst[1] * (j-l) + constants.multConst[2]);
                break;
            }

            default:
            {
                exit(EXIT_FAILURE);
                break;
            }
            }
        }
        seq.FM1[i][j] = *std::min_element(fm1_vals.begin(), fm1_vals.end());

        std::deque<mpq_class> fm_vals;
        fm_vals.push_back(constants.INFINITY_);

        for (int k = i+TURN+1; k <= j-TURN-1; ++k) {
            fm_vals.push_back(seq.FM[i][k-1] + seq.FM1[k][j]);
        }

        for (int k = i; k <= j-TURN-1; ++k) {
            fm_vals.push_back(seq.FM1[k][j] + constants.multConst[1]*(k-i));
        }
        seq.FM[i][j] = *std::min_element(fm_vals.begin(), fm_vals.end());
    }

    mpq_class NNTM::minimum_energy(const RNASequenceWithTables& seq) const {
        /*
          Return the minimum energy of a structure on this sequence
         */
        return seq.W[seq.len()-1];
    };


    // dangle on the 5' end of (i, j)
    // if inside==true, dangle i+1 instead of i-1
    mpq_class NNTM::Ed5(int i, int j, const RNASequence& seq, bool inside) const {
        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());

        mpq_class penalty = 0;

        if (i > 0) {
            if (inside)
                penalty = constants.dangle[seq.base(i)][seq.base(j)][seq.base(i+1)][0];
            else
                penalty = constants.dangle[seq.base(j)][seq.base(i)][seq.base(i-1)][1];
        } else {
            if (inside)
                penalty = constants.dangle[seq.base(i)][seq.base(j)][seq.base(i+1)][0];
            else
                penalty = constants.dangle[seq.base(j)][seq.base(i)][seq.base(seq.len()-1)][1];
        }

        return penalty;
    }

    // dangle on the 3' end of (i, j)
    // if inside==true, dangle j-1 instead of j+1
    mpq_class NNTM::Ed3(int i, int j, const RNASequence& seq, bool inside) const {
        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());

        mpq_class penalty = 0;

        if (j < seq.len()-1) {
            if (inside)
                penalty = constants.dangle[seq.base(i)][seq.base(j)][seq.base(j-1)][1];
            else
                penalty = constants.dangle[seq.base(j)][seq.base(i)][seq.base(j+1)][0];
        } else {
            if (inside)
                penalty = constants.dangle[seq.base(i)][seq.base(j)][seq.base(j-1)][1];
            else
                penalty = constants.dangle[seq.base(j)][seq.base(i)][seq.base(0)][0];
        }

        return penalty;
    }

    mpq_class NNTM::auPenalty(int i, int j, const RNASequence& seq) const {
        /*
          Return the pairing penalty for (i, j); this is nonzero unless it is a GC pair
         */
        int base_i = seq.base(i);
        int base_j = seq.base(j);
        if (
            (base_i == BASE_U and (base_j == BASE_A or base_j == BASE_G)) or
            (base_j == BASE_U and (base_i == BASE_A or base_i == BASE_G))
            ) {
            return constants.auend;
        } else {
            return 0;
        }
    }

    mpq_class NNTM::eL(int i, int j, int ip, int jp, const RNASequence& seq) const {
        /*
          Compute the energy of an internal loop between pairs (i, j) and (ip, jp)
        */
        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());
        assert (i < ip && ip < jp && jp < j);

        mpq_class energy = constants.INFINITY_;
        mpq_class loginc = 0;

        /*SH: These calculations used to incorrectly be within the bulge loop code, moved out here. */
        int size1 = ip - i - 1;
        int size2 = j - jp - 1;
        int size = size1 + size2;

        int lopsided = abs(size1 - size2); /* define the asymmetry of an interior loop */

        int pindex = std::min(2, std::min(size1, size2));
        mpq_class lvalue = lopsided * constants.poppen[pindex];
        mpq_class penterm;
        if (constants.params.dummy_scaling >= 0) {
            penterm = std::min(constants.maxpen, lvalue);
        } else {
            penterm = std::max(constants.maxpen, lvalue);
        }

        if (size1 == 0 || size2 == 0) {
            if (size > 30) {
                /* AM: Does not depend upon i and j and ip and jp - Stacking Energies */
                loginc = constants.prelog * log((double) size / 30.0); // Taking a log and then casting to rational--gross! But we gots ta do what we gots ta do
                energy = constants.bulge[30] + constants.eparam[2] + loginc + auPenalty(i, j, seq)
                    + auPenalty(ip, jp, seq);
            } else if (size <= 30 && size != 1) {
                /* Does not depend upon i and j and ip and jp - Stacking Energies  */
                energy = constants.bulge[size] + constants.eparam[2];
                energy += auPenalty(i, j, seq) + auPenalty(ip, jp, seq);
            } else if (size == 1) {
                energy = constants.stack[seq.base(i)][seq.base(j)][seq.base(ip)][seq.base(jp)]
                    + constants.bulge[size] + constants.eparam[2];
            }
        } else {
            /* Internal loop */

            if (size > 30) {
                loginc = constants.prelog * log((double) size / 30.0);

                if (!((size1 == 1 || size2 == 1) && constants.gail)) { /* normal internal loop with size > 30*/
                    energy = constants.tstki[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] +
                        constants.tstki[seq.base(jp)][seq.base(ip)][seq.base(jp + 1)][seq.base(ip - 1)] + constants.inter[30] + loginc +
                        constants.eparam[3] + penterm;
                } else { /* if size is more than 30 and it is a grossely asymmetric internal loop and gail is not 0*/
                    energy = constants.tstki[seq.base(i)][seq.base(j)][BASE_A][BASE_A] + constants.tstki[seq.base(jp)][seq.base(ip)][BASE_A][BASE_A]
                        + constants.inter[30] + loginc + constants.eparam[3] + penterm;
                }
            }
            else if (size1 == 2 && size2 == 2) { /* 2x2 internal loop */
                energy = constants.iloop22[seq.base(i)][seq.base(ip)][seq.base(j)][seq.base(jp)][seq.base(i+1)][seq.base(j-1)][seq.base(i+2)][seq.base(j-2)];
            } else if (size1 == 1 && size2 == 2) {
                energy = constants.iloop21[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)][seq.base(j - 2)][seq.base(ip)][seq.base(jp)];
            } else if (size1 == 2 && size2 == 1) { /* 1x2 internal loop */
                energy = constants.iloop21[seq.base(jp)][seq.base(ip)][seq.base(j - 1)][seq.base(i + 2)][seq.base(i + 1)][seq.base(j)][seq.base(i)];
            } else if (size == 2) { /* 1*1 internal loops */
                energy = constants.iloop11[seq.base(i)][seq.base(i + 1)][seq.base(ip)][seq.base(j)][seq.base(j - 1)][seq.base(jp)];
            }
            //else if ((size1 == 2 && size2 == 3) || (size1 == 3 && size2 == 2)) {
            //	return constants.tstacki23[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] +
            //		constants.tstacki23[seq.base(jp)][seq.base(ip)][seq.base(jp + 1)][seq.base(ip - 1)];
            //}
            else if ((size1 == 1 || size2 == 1) && constants.gail) { /* gail = (Grossly Asymmetric Interior Loop Rule) (on/off <-> 1/0)  */
                energy = constants.tstki[seq.base(i)][seq.base(j)][BASE_A][BASE_A] + constants.tstki[seq.base(jp)][seq.base(ip)][BASE_A][BASE_A]
                    + constants.inter[size] + loginc + constants.eparam[3] + penterm;
            } else { /* General Internal loops */
                energy = constants.tstki[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] + constants.tstki[seq.base(jp)][seq.base(ip)][seq.base(jp + 1)][seq.base(ip - 1)] + constants.inter[size]
                    + loginc + constants.eparam[3] + penterm;
            }
        }

        return energy;
    }

    mpq_class NNTM::eH(int i, int j, const RNASequence& seq) const {
        /*
          Compute the energy of a hairpin loop based at pair (i, j)
         */

        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());
        assert (i < j);

        mpq_class energy = constants.INFINITY_;

        int size = j - i - 1; /*  size is the number of bases in the loop, when the closing pair is excluded */

        /*  look in hairpin, and be careful that there is only 30 values */

        if (size > 30) {
            mpq_class loginc = constants.prelog * log(((double) size) / 30.0);
            energy = constants.hairpin[30] + loginc + constants.tstkh[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] + constants.eparam[4]; /* size penalty + terminal mismatch stacking energy*/
        }

        else if (size <= 30 && size > 4) {
            energy = constants.hairpin[size] + constants.tstkh[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] + constants.eparam[4]; /* size penalty + terminal mismatch stacking energy*/
        }

        else if (size == 4) {
            std::string loopkey = seq.subsequence(i, j);
            mpq_class tlink = 0; // Loop contribution is typically 0
            if (constants.tloop.count(loopkey) != 0) {
                tlink = constants.tloop.find(loopkey)->second; // But some loops have special contributions, stored in this table
            }
            energy = tlink + constants.hairpin[size] + constants.tstkh[seq.base(i)][seq.base(j)][seq.base(i + 1)][seq.base(j - 1)] + constants.eparam[4];
        }

        else if (size == 3) {
            /*  triloop... For the moment, the file triloop.dat is empty */
            /*  else, should have a treatment like the one if size==4 */
            energy = constants.hairpin[size];
            /* AM: Don't include stacking energy terms for triloopls */
            /* + constants.tstkh[seq.base(i)][seq.base(j)][seq.base(i+1)][seq.base(j-1)]  */
            /* + constants.eparam[4]; */
            /*  Must be another penalty for terminal AU... Not sure of this */
            energy += auPenalty(i, j, seq);
        }

        else if (size < 3 && size != 0) {
            /*  no terminal mismatch */
            energy = constants.hairpin[size] + constants.eparam[4];
        } else if (size == 0)
            energy = constants.INFINITY_;

        /*  GGG Bonus => GU closure preceded by GG */
        /*  i-2 = i-1 = i = G, and j = U; i < j */
        if (i >= 2) {
            if (seq.base(i - 2) == BASE_G && seq.base(i - 1) == BASE_G && seq.base(i) == BASE_G
                && seq.base(j) == BASE_U) {
                energy += constants.gubonus;
                /*  printf ("\n GGG bonus for i %d j %d ", i, j); */
            }
        }

        /*  Poly-C loop => How many C are needed for being a poly-C loop */
        mpq_class tlink = 1;
        for (int index = 1; (index <= size) && (tlink == 1); ++index) {
            if (seq.base(i + index) != BASE_C)
                tlink = 0;
        }

        if (tlink == 1) {
            if (size == 3) {
                energy += constants.c3;
            } else {
                energy += constants.cint + size * constants.cslope;
            }
        }

        return energy;
    }

    mpq_class NNTM::eS(int i, int j, const RNASequence& seq) const {
        /*
          Compute the energy of a stack of pairs (i, j) and (i+1, j-1)
        */
        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());
        assert (i < j);

        return constants.stack[seq.base(i)][seq.base(j)][seq.base(i+1)][seq.base(j-1)] + constants.eparam[1];
    }

    mpq_class NNTM::calcVBI(int i, int j, const RNASequenceWithTables& seq) const {
        /*
          Helper method to populate the VBI array
         */

        // Input specification
        assert (i >= 0 && i < seq.len());
        assert (j >= 0 && j < seq.len());
        assert (i < j);

        std::deque<mpq_class> vals;
        vals.push_back(constants.INFINITY_);

        for (int p = i+1; p <= std::min(j-2-TURN, i+MAXLOOP+1) ; ++p) {
            int minq = j-i+p-MAXLOOP-2;
            if (minq < p+1+TURN)
                minq = p+1+TURN;

            int maxq;
            if (p == i+1) {
                maxq = j-2;
            } else {
                maxq = j-1;
            }

            for (int q = minq; q <= maxq; q++) {
                if (q - p > TURN && seq.can_pair(p, q)) {
                    vals.push_back(eL(i, j, p, q, seq) + seq.V[p][q]);
                }
            }
        }

        mpq_class VBIij = *std::min_element(vals.begin(), vals.end());
        return VBIij;
    }
};
