#include "dumpshape.h"
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <map>
#include <iostream>
#include <string>

using namespace std;

void dumpShape(const TopoDS_Shape& shape) {
	// 统计并打印：
	// TopAbs_SOLID
	// TopAbs_SHELL
	// TopAbs_FACE
	// TopAbs_WIRE
	// TopAbs_EDGE
	// TopAbs_VERTEX

	map<TopAbs_ShapeEnum, string> shapes = {
		{TopAbs_COMPOUND, "TopAbs_COMPOUND"},
		{TopAbs_COMPSOLID, "TopAbs_COMPSOLID"},
		{TopAbs_EDGE, "TopAbs_EDGE"},
		{TopAbs_SHELL, "TopAbs_SHELL"},
		{ TopAbs_FACE, "TopAbs_FACE"},
		{TopAbs_WIRE, "TopAbs_WIRE"},
		{TopAbs_VERTEX, "TopAbs_VERTEX"},
		{TopAbs_SHAPE, "TopAbs_SHAPE"}
	};

	for (const auto& item : shapes)
	{
		int count = 0;
		for (TopExp_Explorer exp(shape, item.first); exp.More(); exp.Next()) {
			count++;
		}

		cout << item.second << ": " << count << endl;
	}
}
