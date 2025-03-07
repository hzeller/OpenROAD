// *****************************************************************************
// *****************************************************************************
// ATTENTION: THIS IS AN AUTO-GENERATED FILE. DO NOT CHANGE IT!
// *****************************************************************************
// *****************************************************************************
// Copyright 2012, Cadence Design Systems
//
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8.
//
// Licensed under the Apache License, Version 2.0 (the \"License\");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an \"AS IS\" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
//
//  $Author: xxx $
//  $Revision: xxx $
//  $Date: xxx $
//  $State: xxx $
// *****************************************************************************
// *****************************************************************************

#define EXTERN extern "C"

#include "defiProp.h"
#include "defiProp.hpp"

// Wrappers definitions.
const char* defiProp_string(const ::defiProp* obj)
{
  return ((const DefParser::defiProp*) obj)->string();
}

const char* defiProp_propType(const ::defiProp* obj)
{
  return ((const DefParser::defiProp*) obj)->propType();
}

const char* defiProp_propName(const ::defiProp* obj)
{
  return ((const DefParser::defiProp*) obj)->propName();
}

char defiProp_dataType(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->dataType();
}

int defiProp_hasNumber(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->hasNumber();
}

int defiProp_hasRange(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->hasRange();
}

int defiProp_hasString(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->hasString();
}

int defiProp_hasNameMapString(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->hasNameMapString();
}

double defiProp_number(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->number();
}

double defiProp_left(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->left();
}

double defiProp_right(const ::defiProp* obj)
{
  return ((DefParser::defiProp*) obj)->right();
}

void defiProp_print(const ::defiProp* obj, FILE* f)
{
  ((DefParser::defiProp*) obj)->print(f);
}
