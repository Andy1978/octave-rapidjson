// Copyright (C) 2017 Andreas Weber <andy@josoansi.de>
//
// GNU Octave wrapper around RapidJSON (http://rapidjson.org/)
//
// This file is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see
// <http://www.gnu.org/licenses/>.

#ifndef _DEBUG_H_
#define _DEBUG_H_
#ifdef DEBUG
#include <iomanip>
#include <iostream>
#define DBG_MSG2(d, a, b) for (int __k__ = 0; __k__ < d; ++__k__) std::cout << "-";\
                   std::cout << std::setw (15 - d) << std::left\
                             << __FILE__ << ":"\
                             << std::setw (14) << __FUNCTION__ << ":"\
                             << std::setw (4) << __LINE__ << " ";\
                   std::cout << a << " "\
                             << b << std::endl
#define DBG_MSG1(d, a) DBG_MSG2(d, a, "")
#else
#define DBG_MSG2(d, a, b)
#define DBG_MSG1(d, a)
#endif
#endif
