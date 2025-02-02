// *****************************************************************************
// *****************************************************************************
// Copyright 2013, Cadence Design Systems
//
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8.
//
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
//
//  $Author: dell $
//  $Revision: #1 $
//  $Date: 2020/09/29 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#ifndef defiDebug_h
#define defiDebug_h

#include "defiKRDefs.hpp"

BEGIN_DEF_PARSER_NAMESPACE

class defrData;

/* Set flag */
extern void defiSetDebug(int num, int value);

/* Read flag */
extern int defiDebug(int num);

/* Error loggin function */
extern void defiError(int check,
                      int msgNum,
                      const char* message,
                      defrData* defData = nullptr);

/* for auto upshifting names in case insensitive files */
extern const char* upperCase(const char* c, defrData* defData = nullptr);
extern const char* DEFCASE(const char* ch, defrData* defData = nullptr);

END_DEF_PARSER_NAMESPACE

#endif
