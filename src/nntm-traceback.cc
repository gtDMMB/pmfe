// Copyright (c) 2015 Andrew Gainer-Dewar.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdexcept>
#include <vector>

#include "nntm.h"
#include "nndb_constants.h"
#include "rational.h"

#include <boost/multi_array.hpp>

//#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>


namespace pmfe {
    RNAStructureWithScore NNTM::mfe_structure(const RNASequenceWithTables& seq) const {
        RNAStructure structure(seq);
        ScoreVector score;

        BOOST_LOG_TRIVIAL(debug) << "Starting structure traceback.";
        traceW(seq.len()-1, seq, structure, score);

        ScoreVector newscore = this->score(structure);

        if (newscore.energy != score.energy) {
            BOOST_LOG_TRIVIAL(error) << "Inconsistent energy: " << score.energy.get_d() << " ≅ " << newscore.energy.get_d();
            BOOST_LOG_TRIVIAL(error) << constants.params;
            throw std::logic_error("Energy calculation was inconsistent!");
        }

        RNAStructureWithScore result (structure, newscore);
        return result;
    }

    bool NNTM::traceW(int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        bool found_something = false;
        Rational wim1;

        if (j <= 0) {
            return score.energy == seq.W[seq.len() - 1];
        }

        for (int i = 0; i < j and not found_something; i++) {
            if (j-i < TURN) continue;

            if (i > 0) {
                wim1 = std::min(Rational(0), seq.W[i-1]);
            } else {
                wim1 = 0;
            }

            switch (dangles) {
            case BOTH_DANGLE:
            {
                Rational e_dangles = 0;
                if (i > 0) {
                    e_dangles += Ed5(i, j, seq);
                }

                if (j < seq.len() - 1) {
                    e_dangles += Ed3(i, j, seq);
                }

                if (seq.W[j] == seq.V[i][j] + auPenalty(i, j, seq) + e_dangles + wim1) {
                    found_something = true;
                    Rational loop = auPenalty(i, j, seq) + e_dangles;
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    traceV(i, j, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                };
                break;
            }

            case NO_DANGLE:
            {
                if (seq.W[j] == seq.V[i][j] + auPenalty(i, j, seq) + wim1) {
                    found_something = true;
                    Rational loop = auPenalty(i, j, seq);
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    traceV(i, j, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                };
                break;
            }

            case CHOOSE_DANGLE:
            {
                if (seq.W[j] == seq.V[i][j] + auPenalty(i, j, seq) + wim1) {
                    found_something = true;
                    Rational loop = auPenalty(i, j, seq);
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    traceV(i, j, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                } else if (seq.W[j] ==  seq.V[i][j-1] + auPenalty(i, j-1, seq) + Ed3(i, j-1, seq) + wim1) {
                    found_something = true;
                    Rational loop = auPenalty(i, j-1, seq) + Ed3(i, j-1, seq);
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    structure.mark_d3(j);
                    traceV(i, j-1, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                } else if (seq.W[j] == seq.V[i+1][j] + auPenalty(i+1, j, seq) + Ed5(i+1, j, seq) + wim1){
                    found_something = true;
                    Rational loop = auPenalty(i+1, j, seq) + Ed5(i+1, j, seq);
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    structure.mark_d5(i);
                    traceV(i + 1, j, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                } else if (seq.W[j] == seq.V[i+1][j-1] + auPenalty(i+1, j-1, seq) + Ed5(i+1, j-1, seq) + Ed3(i+1, j-1, seq) + wim1) {
                    found_something = true;
                    Rational loop = auPenalty(i+1, j-1, seq) + Ed5(i+1, j-1, seq) + Ed3(i+1, j-1, seq);
                    score.energy += loop;
                    BOOST_LOG_TRIVIAL(debug) << "ExtLoop (" << i << ", " << j << ") with energy " << loop.get_d();
                    structure.mark_d3(j);
                    structure.mark_d5(i);
                    traceV(i+1, j-1, seq, structure, score);
                    traceW(i-1, seq, structure, score);
                }

                break;
            }

            default:
                throw std::logic_error("Invalid dangle mode.");
                break;
            }
        }

        if (seq.W[j] == seq.W[j-1] and not found_something) {
            found_something = traceW(j-1, seq, structure, score);
        }

        if (not found_something) {
            BOOST_LOG_TRIVIAL(error) << "W traceback did not finish for j = " << j;
            BOOST_LOG_TRIVIAL(error) << constants.params;
            throw std::logic_error("W traceback did not finish!");
        }

        return found_something;
    }

    Rational NNTM::traceV(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        Rational a, b, c, d;
        Rational Vij;
        if (j-i < TURN)  return Rational::infinity();

        // TODO: Eliminate silly intermediate variables
        a = eH(i, j, seq);

        b = eS(i, j, seq) + seq.V[i + 1][j - 1];
        c = seq.VBI[i][j];
        d = seq.VM[i][j];

        Vij = seq.V[i][j];
        structure.mark_pair(i, j);

        if (Vij == a ) {
            Rational loop = eH(i, j, seq);
            score.energy += loop;
            BOOST_LOG_TRIVIAL(debug) << "Hairpin (" << i << ", " << j << ") with energy " << loop.get_d();
            return Vij;
        } else if (Vij == b) {
            Rational loop = eS(i, j, seq);
            score.energy += loop;
            BOOST_LOG_TRIVIAL(debug) << "Stack (" << i << ", " << j << ") with energy " << loop.get_d();
            traceV(i+1, j-1, seq, structure, score);
            return Vij;
        } else if (Vij == c) {
            traceVBI(i, j, seq, structure, score);
            return Vij;
        } else if (Vij == d) {
            Rational loop = Vij - traceVM(i, j, seq, structure, score);
            score.energy += loop;
            BOOST_LOG_TRIVIAL(debug) << "Multiloop (" << i << ", " << j << ") with energy " << loop.get_d();
            return Vij;
        }

        return 0;
    }

    Rational NNTM::traceVBI(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        Rational VBIij;
        int ip, jp;
        int ifinal, jfinal;

        ifinal = 0;
        jfinal = 0;

        for (ip = i + 1; ip < j - 1; ip++) {
            for (jp = ip + 1; jp < j; jp++) {
                VBIij = eL(i, j, ip, jp, seq) + seq.V[ip][jp];
                if (VBIij == seq.VBI[i][j]){
                    ifinal = ip;
                    jfinal = jp;
                    break;
                }
            }
            if (jp != j) break;
        }

        Rational loop = eL(i, j, ifinal, jfinal, seq);
        score.energy += loop;

        BOOST_LOG_TRIVIAL(debug) << "IntLoop (" << i << ", " << j << ") with energy " << loop.get_d();

        return traceV(ifinal, jfinal, seq, structure, score);
    }

    Rational NNTM::traceVM(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        Rational eVM = 0;

        switch (dangles) {
        case BOTH_DANGLE:
        {
            if (seq.VM[i][j] == seq.WMPrime[i+1][j-1] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) + Ed5(i, j, seq, true) + Ed3(i, j, seq, true)) {
                eVM += traceWMPrime(i+1, j-1, seq, structure, score);
                score.multiloops++;
                score.branches++;
            }
            break;
        }

        case NO_DANGLE:
        {
            if (seq.VM[i][j] == seq.WMPrime[i+1][j-1] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) ) {
                eVM += traceWMPrime(i+1, j-1, seq, structure, score);
                score.multiloops++;
                score.branches++;
            }
            break;
        }

        case CHOOSE_DANGLE: {
            if (seq.VM[i][j] == seq.WMPrime[i+1][j-1] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) ) {
                eVM += traceWMPrime(i+1, j-1, seq, structure, score);
                score.multiloops++;
                score.branches++;
            } else if (seq.VM[i][j] == seq.WMPrime[i+2][j-1] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) + Ed5(i, j, seq, true) + constants.multConst[1]) {
                eVM += traceWMPrime(i+2, j-1, seq, structure, score);
                structure.mark_d3(i+1);
                score.multiloops++;
                score.branches++;
                score.unpaired++;
            } else if (seq.VM[i][j] == seq.WMPrime[i+1][j-2] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) + Ed3(i, j, seq, true) + constants.multConst[1]) {
                eVM += traceWMPrime(i+1, j-2, seq, structure, score);
                structure.mark_d5(j-1);
                score.multiloops++;
                score.branches++;
                score.unpaired++;
            } else if (seq.V[i][j] ==  seq.WMPrime[i+2][j-2] + constants.multConst[0] + constants.multConst[2] + auPenalty(i, j, seq) + Ed5(i, j, seq, true) + Ed3(i, j, seq, true) + constants.multConst[1]*2) {
                eVM += traceWMPrime(i+2, j-2, seq, structure, score);
                structure.mark_d3(i+1);
                structure.mark_d5(j-1);
                score.multiloops++;
                score.branches++;
                score.unpaired += 2;
            }

            break;
        }

        default:
            throw std::logic_error("Invalid dangle mode.");
            break;
        }

        return eVM;
    }

    Rational NNTM::traceWMPrime(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        int done=0, h;
        Rational energy = 0;

        for (h = i; h < j and not done; h++) {
            if (seq.WM[i][h] + seq.WM[h+1][j] == seq.WMPrime[i][j]) {
                energy += traceWM(i, h, seq, structure, score);
                energy += traceWM(h+1, j, seq, structure, score);
                done = 1;
                break;
            }
        }
        return energy;
    }

    Rational NNTM::traceWM(int i, int j, const RNASequenceWithTables& seq, RNAStructure& structure, ScoreVector& score) const {
        assert(i < j);
        int done = 0;
        Rational eWM = 0;

        if (not done and seq.WM[i][j] == seq.WMPrime[i][j]) {
            eWM += traceWMPrime(i, j, seq, structure, score);
            done = 1;
        }

        if (not done){
            switch (dangles) {
            case BOTH_DANGLE:
            {
                if (seq.WM[i][j] == seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2] + Ed5(i, j, seq) + Ed3(i, j, seq)) {
                    eWM += traceV(i, j, seq, structure, score);
                    score.branches++;
                    done = 1;
                }
                break;
            }

            case NO_DANGLE:
            {
                if (seq.WM[i][j] == seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2]) {
                    eWM += traceV(i, j, seq, structure, score);
                    score.branches++;
                    done = 1;
                }
                break;
            }

            case CHOOSE_DANGLE:
            {
                if (seq.WM[i][j] == seq.V[i][j] + auPenalty(i, j, seq) + constants.multConst[2]) {
                    eWM += traceV(i, j, seq, structure, score);
                    score.branches++;
                    done = 1;
                } else if (seq.WM[i][j] == seq.V[i+1][j] + Ed5(i+1, j, seq) + auPenalty(i+1, j, seq) + constants.multConst[2] + constants.multConst[1]) {
                    eWM += traceV(i+1, j, seq, structure, score);
                    structure.mark_d5(i);
                    score.branches++;
                    score.unpaired++;
                    done = 1;
                } else if (seq.WM[i][j] == seq.V[i][j-1] + Ed3(i, j-1, seq) + auPenalty(i, j-1, seq) + constants.multConst[2] + constants.multConst[1]) {
                    eWM += traceV(i, j-1, seq, structure, score);
                    structure.mark_d3(j);
                    score.branches++;
                    score.unpaired++;
                    done = 1;
                } else if (seq.WM[i][j] == seq.V[i+1][j-1] + Ed5(i+1, j-1, seq) + Ed3(i+1, j-1, seq) + auPenalty(i+1, j-1, seq) + constants.multConst[2] + constants.multConst[1]*2) {
                    eWM += traceV(i+1, j-1, seq, structure, score);
                    structure.mark_d5(i);
                    structure.mark_d3(j);
                    score.branches++;
                    score.unpaired += 2;
                    done = 1;
                }
                break;
            }

            default:
                throw std::logic_error("Invalid dangle mode.");
                break;
            }

            if (not done){
                if (seq.WM[i][j] == seq.WM[i+1][j] + constants.multConst[1]) {
                    done = 1;
                    eWM += traceWM(i+1, j, seq, structure, score);
                    score.unpaired++;
                } else if (seq.WM[i][j] == seq.WM[i][j-1] + constants.multConst[1]) {
                    done = 1;
                    eWM += traceWM(i, j-1, seq, structure, score);
                    score.unpaired++;
                }
            }
        }

        return eWM;
    }
}
