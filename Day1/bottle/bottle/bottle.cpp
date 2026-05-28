// bottle.cpp : Defines the entry point for the application.
//

#include "bottle.h"

// MSVC 下使用 M_PI 需要这个
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

// gp 基础几何
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

// Geom 3D geometry
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>

// Geom2d
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>

// geometry construction
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <GCE2d_MakeSegment.hxx>

// TopoDS topology
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>

// topology explorer
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

// BRep tools/builders
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>

#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

// collections
#include <TopTools_ListOfShape.hxx>
#include <NCollection_List.hxx>

using namespace std;

TopoDS_Shape MakeBottle(double theWidth, double theHeight,
                        double theThickness)
{
    // Profile : Define Support Points
    gp_Pnt aPnt1(-theWidth / 2., 0, 0);        
    gp_Pnt aPnt2(-theWidth / 2., -theThickness / 4., 0);
    gp_Pnt aPnt3(0, -theThickness / 2., 0);
    gp_Pnt aPnt4(theWidth / 2., -theThickness / 4., 0);
    gp_Pnt aPnt5(theWidth / 2., 0, 0);
 
    // Profile : Define the Geometry
    occ::handle<Geom_TrimmedCurve> anArcOfCircle = GC_MakeArcOfCircle(aPnt2,aPnt3,aPnt4);
    occ::handle<Geom_TrimmedCurve> aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    occ::handle<Geom_TrimmedCurve> aSegment2 = GC_MakeSegment(aPnt4, aPnt5);
 
    // Profile : Define the Topology
    TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
    TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Wire aWire  = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);
 
    // Complete Profile
    gp_Ax1 xAxis = gp::OX();
    gp_Trsf aTrsf;
 
    aTrsf.SetMirror(xAxis);
    BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
    TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
    TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);
 
    BRepBuilderAPI_MakeWire aMkWire;
    aMkWire.Add(aWire);
    aMkWire.Add(aMirroredWire);
    TopoDS_Wire aWireProfile = aMkWire.Wire();
 
    // Body : Prism the Profile
    TopoDS_Face aFaceProfile = BRepBuilderAPI_MakeFace(aWireProfile);
    gp_Vec aPrismVec(0, 0, theHeight);
    TopoDS_Shape aBody = BRepPrimAPI_MakePrism(aFaceProfile, aPrismVec);
 
    // Body : Apply Fillets
    BRepFilletAPI_MakeFillet aMkFillet(aBody);
    TopExp_Explorer anEdgeExplorer(aBody, TopAbs_EDGE);
    while(anEdgeExplorer.More()){
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
        //Add edge to fillet algorithm
        aMkFillet.Add(theThickness / 12., anEdge);
        anEdgeExplorer.Next();
    }
 
    aBody = aMkFillet.Shape();
 
    // Body : Add the Neck
    gp_Pnt aNeckLocation(0, 0, theHeight);
    gp_Dir aNeckAxis = gp::DZ();
    gp_Ax2 neckAx2(aNeckLocation, aNeckAxis);
 
    double aNeckRadius = theThickness / 4.;
    double aNeckHeight = theHeight / 10.;
 
    BRepPrimAPI_MakeCylinder aMkCylinder(neckAx2, aNeckRadius, aNeckHeight);
    TopoDS_Shape aNeck = aMkCylinder.Shape();
 
    BRepAlgoAPI_Fuse aFuser(aBody, aNeck);
    if (aFuser.IsDone())
    {
      aBody = aFuser.Shape();
    }
    else
    {
      // Fuse failed; proceed with unmodified aBody.
      // In production code, report the error and handle the failure appropriately.
    }
 
    // Body : Create a Hollowed Solid
    TopoDS_Face   aFaceToRemove;
    double aZMax = -1;
 
    for(TopExp_Explorer aFaceExplorer(aBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()){
        TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
        // Check if <aFace> is the top face of the bottle's neck 
        occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(aFace);
        if(!aSurface.IsNull() && aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)){
            occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(aSurface);
            if (!aPlane.IsNull())
            {
              gp_Pnt aPnt = aPlane->Location();
              double aZ   = aPnt.Z();
              if(aZ > aZMax){
                  aZMax = aZ;
                  aFaceToRemove = aFace;
              }
            }
        }
    }
 
    NCollection_List<TopoDS_Shape> aFacesToRemove;
    aFacesToRemove.Append(aFaceToRemove);
    BRepOffsetAPI_MakeThickSolid aSolidMaker;
    aSolidMaker.MakeThickSolidByJoin(aBody, aFacesToRemove, -theThickness / 50, 1.e-3);
    aBody = aSolidMaker.Shape();
    // Threading : Create Surfaces
    occ::handle<Geom_CylindricalSurface> aCyl1 = new Geom_CylindricalSurface(neckAx2, aNeckRadius * 0.99);
    occ::handle<Geom_CylindricalSurface> aCyl2 = new Geom_CylindricalSurface(neckAx2, aNeckRadius * 1.05);
 
    // Threading : Define 2D Curves
    gp_Pnt2d aPnt(2. * M_PI, aNeckHeight / 2.);
    gp_Dir2d aDir(2. * M_PI, aNeckHeight / 4.);
    gp_Ax2d anAx2d(aPnt, aDir);
 
    double aMajor = 2. * M_PI;
    double aMinor = aNeckHeight / 10;
 
    occ::handle<Geom2d_Ellipse> anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
    occ::handle<Geom2d_Ellipse> anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
    occ::handle<Geom2d_TrimmedCurve> anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
    occ::handle<Geom2d_TrimmedCurve> anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
    gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
    gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);
 
    occ::handle<Geom2d_TrimmedCurve> aSegment = GC_MakeSegment2d(anEllipsePnt1, anEllipsePnt2);
    // Threading : Build Edges and Wires
    TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
    TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
    TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
    TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
    TopoDS_Wire aThreadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
    TopoDS_Wire aThreadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
    BRepLib::BuildCurves3d(aThreadingWire1);
    BRepLib::BuildCurves3d(aThreadingWire2);
 
    // Create Threading 
    BRepOffsetAPI_ThruSections aTool(true);
    aTool.AddWire(aThreadingWire1);
    aTool.AddWire(aThreadingWire2);
    aTool.CheckCompatibility(false);
 
    TopoDS_Shape aThreading = aTool.Shape();
 
    // Building the Resulting Compound 
    TopoDS_Compound aRes;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aRes);
    aBuilder.Add (aRes, aBody);
    aBuilder.Add (aRes, aThreading);
 
    return aRes;
}

