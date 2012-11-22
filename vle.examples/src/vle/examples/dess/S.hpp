/*
 * @file vle/examples/dess/S.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_EXAMPLES_S_HPP
#define VLE_EXAMPLES_S_HPP

#include <vle/extension/differential-equation/DESS.hpp>

namespace vle { namespace examples { namespace dess {

class S : public extension::DifferentialEquation::DESS
{
public:
    S(const devs::DynamicsInit& model,
      const devs::InitEventList& events);
    virtual ~S()
        { }

    virtual double compute(const devs::Time& time) const;

private:
    double r;

    Var _S;
    Ext I;
};

}}} // namespace vle examples dess

#endif