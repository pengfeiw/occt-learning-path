#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include "dumpshape.h"

int main() {
    const char* filePath = "bottle.brep";

    TopoDS_Shape shape;
    BRep_Builder builder;

    bool ok = BRepTools::Read(shape, filePath, builder);

    if (!ok || shape.IsNull())
    {
        std::cerr << "Failed to read BREP file: " << filePath << std::endl;
        return 1;
    }

    std::cout << "BREP file loaded successfully: " << filePath << std::endl;

    dumpShape(shape);
	return 0;
}