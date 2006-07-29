/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Nicolas Di C�sar�

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file criteria.hpp
    \brief Optimization criteria class
*/

#ifndef quantlib_optimization_criteria_hpp
#define quantlib_optimization_criteria_hpp

#include <ql/types.hpp>
#include <iostream>

namespace QuantLib {


    //! Criteria to end optimization process
    /*!   - stationary point
          - stationary gradient
          - maximum number of iterations
          ....
    */
    class EndCriteria {
      public:
        enum Type { none, maxIter, statPt, statGd };

		//! default constructor
        EndCriteria();
        //! initialization constructor
        EndCriteria(Size maxIteration,
                    Real epsilon);
        void setPositiveOptimization(bool);
        /*! test if the number of iteration is not too big and if we don't
            raise a stationary point */
        bool operator()(Size iteration,
                        Real fold,
                        Real normgold,
                        Real fnew,
                        Real normgnew,
                        Real);
        //! return the end criteria type
        Type criteria() const;

        bool checkIterationNumber(Size iteration);
        bool checkStationaryValue(Real fold,
                                  Real fnew);
        bool checkAccuracyValue(Real f);
        bool checkStationaryGradientNorm(Real normDiff);
        bool checkAccuracyGradientNorm(Real norm);

      protected:
        //! Maximum number of iterations
        Size maxIteration_;
        //! function and gradient epsilons
        Real functionEpsilon_, gradientEpsilon_;
        //! Maximun number of iterations in stationary state
        Size maxIterStatPt_, statState_;
        Type endCriteria_;
        bool positiveOptimization_;
    };

	std::ostream& operator<<(std::ostream& out,
                             EndCriteria::Type ec);

    // inline 
    inline void EndCriteria::setPositiveOptimization(bool positiveOptimization) {
        positiveOptimization_ = positiveOptimization;
    }

    inline bool EndCriteria::checkIterationNumber(Size iteration) {
        bool test = (iteration >= maxIteration_);
        if (test)
            endCriteria_ = maxIter;
        return test;
    }

    inline bool EndCriteria::checkStationaryValue(Real fold,
                                                  Real fnew) {
        bool test = (std::fabs(fold - fnew) < functionEpsilon_);
        if (test) {
            statState_++;
            if (statState_ > maxIterStatPt_) {
                endCriteria_ = statPt;
            }
        } else {
            if (statState_ != 0)
                statState_ = 0;
        }
        return (test && (statState_ > maxIterStatPt_));
    }

    inline bool EndCriteria::checkAccuracyValue(Real f) {
        bool test = (f < functionEpsilon_ && positiveOptimization_);
        if (test) {
            endCriteria_ = statPt;
        }
        return test;
    }

    inline bool EndCriteria::checkStationaryGradientNorm(Real normDiff) {
        bool test = (normDiff < gradientEpsilon_);
        if (test)
            endCriteria_ = statGd;
        return test;
    }

    inline bool EndCriteria::checkAccuracyGradientNorm(Real norm) {
        bool test = (norm < gradientEpsilon_);
        if (test)
            endCriteria_ = statGd;
        return test;
    }

    inline bool EndCriteria::operator()(Size iteration,
                                        Real fold,
                                        Real normgold,
                                        Real fnew,
                                        Real normgnew,
                                        Real)
    {
        return
            checkIterationNumber(iteration) ||
            checkStationaryValue(fold, fnew) ||
            checkAccuracyValue(fnew) ||
            checkAccuracyValue(fold) ||
            checkAccuracyGradientNorm(normgnew) ||
            checkAccuracyGradientNorm(normgold);
    }

    inline EndCriteria::Type EndCriteria::criteria() const {
        return endCriteria_;
    }

}

#endif
