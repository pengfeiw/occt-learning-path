#include "bottle.h"
#include <BRepTools.hxx>
#include <STEPControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Failure.hxx>

int main()
{
    try
    {
        TopoDS_Shape bottle = MakeBottle(50.0, 70.0, 30.0);

        if (bottle.IsNull())
        {
            std::cerr << "MakeBottle failed: result shape is null." << std::endl;
            return 1;
        }

        // 1. 导出 OCC 原生 BREP
        const char* brepFile = "bottle.brep";

        if (!BRepTools::Write(bottle, brepFile))
        {
            std::cerr << "Failed to write " << brepFile << std::endl;
            return 1;
        }

        std::cout << "Wrote " << brepFile << std::endl;

        // 2. 导出 STEP
        const char* stepFile = "bottle.step";

        STEPControl_Writer writer;

        IFSelect_ReturnStatus transferStatus =
            writer.Transfer(bottle, STEPControl_AsIs);

        if (transferStatus != IFSelect_RetDone)
        {
            std::cerr << "Failed to transfer shape to STEP writer." << std::endl;
            return 1;
        }

#include <BRepTools.hxx>
        IFSelect_ReturnStatus writeStatus = writer.Write(stepFile);

        if (writeStatus != IFSelect_RetDone)
        {
            std::cerr << "Failed to write " << stepFile << std::endl;
            return 1;
        }

        std::cout << "Wrote " << stepFile << std::endl;
#include <BRepTools.hxx>

        return 0;
    }
    catch (const Standard_Failure& e)
    {
        std::cerr << "OCC exception: " << e.GetMessageString() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "std exception: " << e.what() << std::endl;
        return 1;
    }
}
