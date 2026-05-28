// bottle.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

#include <TopoDS_Shape.hxx>
#include <Standard_TypeDef.hxx>

TopoDS_Shape MakeBottle(double theWidth, double theHeight,
    double theThickness);

// TODO: Reference additional headers your program requires here.
