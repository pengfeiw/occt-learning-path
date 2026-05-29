# 从架构、Modeling Data 到 B-Rep 拓扑结构

## 1. 为什么学习 OCCT 要先理解 Modeling Data？

Open CASCADE Technology，简称 OCCT，是一个开源的几何建模内核。它不仅能创建点、线、面、体，还提供了拓扑结构、B-Rep 表达、几何算法、布尔运算、文件读写、可视化等一系列能力。

如果刚开始学习 OCCT，最容易犯的错误是直接去看建模算法，比如拉伸、倒角、布尔、STEP 读写。但这些算法背后都依赖同一套核心数据结构：**Modeling Data**。

Modeling Data 可以理解为 OCCT 的建模数据基础层。它回答几个最核心的问题：

* 几何对象怎么表示？
* 拓扑对象怎么表示？
* 一个实体是如何由 Vertex、Edge、Wire、Face、Shell、Solid 组成的？
* 几何和拓扑如何绑定？
* 一个 Shape 为什么可以被共享？
* 同一个 Edge 为什么可以在不同 Face 中以不同方向使用？

在继续学习 OCCT 的建模算法之前，先理解 Modeling Data，是非常必要的。

---

## 2. OCCT 的大体架构

从学习角度，可以先把 OCCT 分成几层：

```text
Foundation Classes
    ↓
Modeling Data
    ↓
Modeling Algorithms
    ↓
Data Exchange / Visualization / Application Framework
```

### 2.1 Foundation Classes

这一层提供基础设施，比如：

```cpp
Standard_Transient
Handle(...)
NCollection_*
TCollection_*
```

其中最重要的是 `Handle` 和 `Standard_Transient`。

OCCT 中很多几何对象不是直接按值复制，而是通过 `Handle` 引用管理。可以把 `Handle(Geom_Curve)` 理解成类似 `std::shared_ptr<Geom_Curve>` 的引用计数智能指针。

例如：

```cpp
Handle(Geom_Curve) curve;
Handle(Geom_Surface) surface;
```

它们背后的对象通常继承自 `Standard_Transient`，由 `Handle` 负责生命周期管理。

---

### 2.2 Modeling Data

这一层是 Day1 / Day2 的重点。

它主要包括：

```text
gp       基础几何数学对象
Geom     3D 几何对象
Geom2d   2D 几何对象
TopAbs   拓扑枚举
TopLoc   拓扑位置
TopoDS   拓扑数据结构
TopExp   拓扑遍历工具
TopTools 拓扑容器工具
BRep     geometry + topology 的结合
```

Modeling Data 的核心思想是：

```text
Geometry 描述数学形状；
Topology 描述这些形状如何被使用、连接、限制和组织。
```

---

### 2.3 Modeling Algorithms

这一层负责真正的建模操作，比如：

```text
创建基本体
拉伸
旋转
布尔运算
倒角
圆角
偏移
曲线曲面构造
```

常见包包括：

```cpp
BRepPrimAPI
BRepBuilderAPI
BRepAlgoAPI
BRepFilletAPI
BRepOffsetAPI
```

例如：

```cpp
BRepPrimAPI_MakeBox
BRepBuilderAPI_MakeEdge
BRepBuilderAPI_MakeFace
BRepAlgoAPI_Cut
```

---

### 2.4 Data Exchange

这一层负责文件交换，例如：

```text
STEP
IGES
BREP
STL
OBJ
```

常见包包括：

```cpp
STEPControl
IGESControl
BRepTools
StlAPI
```

其中 `.brep` 是 OCCT 自己的 B-Rep 文件格式，适合保存 OCCT 内部的拓扑和几何结构。

---

## 3. OCCT 中的主要 Package

### 3.1 `gp`：基础几何数学对象

`gp` 可以理解为 geometry primitives，也就是基础几何对象。

常见类型有：

```cpp
gp_Pnt      // 3D 点
gp_Pnt2d    // 2D 点
gp_Vec      // 3D 向量
gp_Dir      // 3D 方向
gp_Lin      // 直线
gp_Circ     // 圆
gp_Ax1      // 轴
gp_Ax2      // 坐标系
gp_Trsf     // 变换
```

`gp` 对象通常是轻量值对象。

例如：

```cpp
gp_Pnt p(0, 0, 0);
gp_Vec v(1, 0, 0);
gp_Dir d(0, 0, 1);
```

它们通常直接按值创建、复制和传递。

---

### 3.2 `Geom` / `Geom2d`：完整几何对象

`Geom` 表示 3D 几何对象，`Geom2d` 表示 2D 几何对象。

例如：

```cpp
Handle(Geom_Curve)
Handle(Geom_Line)
Handle(Geom_Circle)
Handle(Geom_BSplineCurve)

Handle(Geom_Surface)
Handle(Geom_Plane)
Handle(Geom_CylindricalSurface)
Handle(Geom_BSplineSurface)
```

和 `gp` 不同，`Geom` / `Geom2d` 对象通常通过 `Handle` 管理。

可以这样理解：

```text
gp_xxx       小型数学对象，通常按值使用
Geom_xxx     完整几何对象，通常用 Handle 引用
TopoDS_xxx   拓扑对象，通常按值传递，但内部共享 TShape
```

---

### 3.3 `TopAbs`：拓扑抽象枚举

`TopAbs` 中的 `Abs` 可以理解为 Abstract。

它提供拓扑相关的枚举，比如：

```cpp
TopAbs_ShapeEnum
TopAbs_Orientation
TopAbs_State
```

其中最常见的是 `TopAbs_ShapeEnum`：

```cpp
TopAbs_COMPOUND
TopAbs_COMPSOLID
TopAbs_SOLID
TopAbs_SHELL
TopAbs_FACE
TopAbs_WIRE
TopAbs_EDGE
TopAbs_VERTEX
TopAbs_SHAPE
```

它们表示拓扑对象的类型。

例如：

```cpp
if (shape.ShapeType() == TopAbs_FACE)
{
    std::cout << "This is a face" << std::endl;
}
```

`TopAbs` 不是具体拓扑对象，而是拓扑系统里的“概念词典”。

---

### 3.4 `TopLoc`：拓扑位置

`TopLoc` 用来描述 Shape 的位置变换。

最重要的是：

```cpp
TopLoc_Location
TopLoc_Datum3D
```

`TopLoc_Datum3D` 可以理解为一个基础 3D 位置基准，内部对应一个基础变换。

`TopLoc_Location` 则是多个基础变换组合出来的复合位置。

一个 `TopoDS_Shape` 不一定真的修改自己的几何坐标。它可以通过 `TopLoc_Location` 表示：

```text
这个 shape 被放到了某个位置。
```

这在装配体中非常重要。

比如同一个螺丝模型可以被放到多个位置：

```text
same screw TShape
    + Location A
    + Location B
    + Location C
```

这样不需要复制多份几何和拓扑数据。

---

### 3.5 `TopoDS`：拓扑数据结构

`TopoDS` 可以理解为 Topological Data Structure。

它是 OCCT 拓扑系统的核心。

常见类包括：

```cpp
TopoDS_Shape
TopoDS_Vertex
TopoDS_Edge
TopoDS_Wire
TopoDS_Face
TopoDS_Shell
TopoDS_Solid
TopoDS_Compound
```

其中 `TopoDS_Shape` 是所有拓扑对象的通用基类。

可以把 `TopoDS_Shape` 简化理解为：

```cpp
class TopoDS_Shape
{
    Handle(TopoDS_TShape) myTShape;
    TopLoc_Location       myLocation;
    TopAbs_Orientation    myOrientation;
};
```

也就是说：

```text
TopoDS_Shape = 指向底层 TShape 的引用 + Location + Orientation
```

这也是 OCCT 拓扑结构最关键的设计之一。

---

## 4. Geometry 和 Topology 的区别

OCCT 中一定要区分 Geometry 和 Topology。

### 4.1 Geometry 描述数学对象

例如：

```text
一条无限直线
一个圆
一个无限平面
一个圆柱面
一个 B-Spline 曲面
```

这些属于几何。

例如：

```cpp
Handle(Geom_Curve) curve;
Handle(Geom_Surface) surface;
```

几何对象回答的问题是：

```text
这个东西在数学上是什么？
它的参数方程是什么？
它的坐标如何计算？
```

---

### 4.2 Topology 描述几何对象如何被使用

拓扑对象回答的问题是：

```text
我用了这条曲线的哪一段？
我用了这个曲面的哪一块？
这些 Edge 如何组成 Wire？
这些 Wire 如何限制 Face？
这些 Face 如何组成 Shell？
这些 Shell 如何包围 Solid？
```

例如：

```text
Geom_Line    = 一条无限直线
TopoDS_Edge  = 这条直线上的一段
```

再比如：

```text
Geom_Plane   = 一个无限平面
TopoDS_Face  = 这个平面上被 Wire 限制出来的一块区域
```

所以可以记成：

```text
Geometry 负责“数学上是什么”；
Topology 负责“模型里怎么用它”。
```

---

## 5. OCCT 的拓扑层级

OCCT 中常见的拓扑层级是：

```text
Compound
  CompSolid
    Solid
      Shell
        Face
          Wire
            Edge
              Vertex
```

更常用的理解是：

```text
Solid
└── Shell
    └── Face
        └── Wire
            └── Edge
                └── Vertex
```

### 5.1 Vertex

`Vertex` 是零维拓扑对象，对应几何中的点。

```text
Vertex = 拓扑点
```

它可以限制一条 Edge 的端点。

---

### 5.2 Edge

`Edge` 是一维拓扑对象，对应几何中的曲线的一段。

```text
Edge = 曲线的一段 + 端点 + 参数范围 + 方向 + 容差
```

注意：

```text
Edge 不是 Geom_Curve；
Edge 是对 Geom_Curve 的拓扑使用。
```

---

### 5.3 Wire

`Wire` 是一组连接起来的 Edge。

```text
Wire = connected edges
```

Wire 可以是开放的，也可以是闭合的。

Face 的边界通常由一个或多个 Wire 组成：

```text
outer wire
inner wire for holes
```

---

### 5.4 Face

`Face` 是二维拓扑对象，对应一个曲面上的一块区域。

```text
Face = Surface 的一块被 Wire 限制出来的区域
```

例如，一个平面 Face 可能只是无限平面上的一个矩形区域。

一个带孔 Face 可能有：

```text
一个外边界 Wire
一个或多个内孔 Wire
```

---

### 5.5 Shell

`Shell` 是一组连接起来的 Face。

它表示一个壳，也就是一些面组成的边界。

```text
Shell = 一组 Face 组成的表皮
```

Shell 可以是开放的，也可以是闭合的。

例如，没有盖子的盒子就是一个 open shell，但它不是普通意义上的 solid。

---

### 5.6 Solid

`Solid` 表示三维空间中的一个区域。

```text
Solid = 被 Shell 限制出来的三维区域
```

Shell 和 Solid 很容易混淆。

可以这样记：

```text
Shell = 边界壳
Solid = 壳包围出的体积区域
```

如果只是显示，Shell 看起来确实像 Solid；但如果要做布尔运算、体积计算、inside/outside 判断，就必须有 Solid 的语义。

---

## 6. Orientation 的作用

`TopAbs_Orientation` 用来表示一个拓扑对象作为边界时的使用方向。

常见值有：

```cpp
TopAbs_FORWARD
TopAbs_REVERSED
TopAbs_INTERNAL
TopAbs_EXTERNAL
```

其中最常用的是：

```text
FORWARD
REVERSED
```

Orientation 的核心作用是：

```text
说明这个边界对象的哪一侧是 interior。
```

OCCT 先定义一个 default region，然后 orientation 决定真正的 interior 和 default region 的关系：

```text
FORWARD   interior = default region
REVERSED  interior = default region 的相反侧
INTERNAL  两侧都是 interior
EXTERNAL  两侧都不是 interior
```

### 6.1 Edge 在 Wire / Face 中的方向

同一条 Edge 在不同 Face 中可能方向不同。

例如两个相邻 Face 共享一条 Edge：

```text
Face A 使用 Edge：FORWARD
Face B 使用 Edge：REVERSED
```

底层 Edge 可以是同一个，但它在不同上下文中的 Orientation 不同。

这就是为什么 Orientation 存在 `TopoDS_Shape` wrapper 上，而不是存在底层 `TopoDS_TShape` 中。

---

## 7. Location 的作用

`TopLoc_Location` 表示一个 Shape 的位置变换。

一个 Shape 可以共享同一个底层 TShape，但有不同的位置：

```text
same TShape
    + Location A
    + Location B
    + Location C
```

这在装配体中非常常见。

例如多个相同螺丝放在不同位置，如果每个都复制完整几何，会非常浪费。更好的方式是：

```text
共享同一个底层拓扑和几何
每个实例带一个不同的 Location
```

所以：

```text
Location 解决“同一个 shape 放在哪里”的问题；
Orientation 解决“同一个 shape 怎么被使用”的问题。
```

---

## 8. TopoDS_Shape 与 TopoDS_TShape

这是 Day2 最重要的知识点之一。

`TopoDS_Shape` 和 `TopoDS_TShape` 的关系可以这样理解：

```text
TopoDS_TShape = 真实拓扑数据本体
TopoDS_Shape  = 指向 TShape 的轻量引用，并带有 Location 和 Orientation
```

也就是说：

```cpp
TopoDS_Edge e1 = edge;
TopoDS_Edge e2 = edge;
```

这里并不会深拷贝底层 Edge 数据。

它只是复制了：

```text
TShape 引用
Location
Orientation
```

所以可以说：

```text
TopoDS_Shape 按值传递；
TopoDS_TShape 通过引用共享。
```

这和 `Geom_Curve` 这种几何对象通常通过 `Handle` 引用传递并不矛盾。

更准确地说：

```text
Geometry 对象本身常用 Handle 管理；
Topology 的外层 Shape 是值对象；
Topology 的底层 TShape 也是共享引用。
```

---

## 9. 为什么需要 TopoDS_TShape 这一层？

如果没有 `TopoDS_TShape`，`TopoDS_Edge` 就必须直接保存全部底层数据。

这样会有几个问题：

### 9.1 难以共享

两个 Face 共享同一条 Edge 时，如果没有共享层，可能就要复制两份 Edge。

这样它们在拓扑意义上就不是同一条边了。

### 9.2 难以表达不同 Orientation

同一条 Edge 在 Face A 中可能是 `FORWARD`，在 Face B 中可能是 `REVERSED`。

如果 Orientation 存在底层 Edge 中，就只能有一个方向。

所以 OCCT 把 Orientation 放在外层 `TopoDS_Shape` 上：

```text
same TShape
    + TopoDS_Edge FORWARD
    + TopoDS_Edge REVERSED
```

### 9.3 难以表达不同 Location

装配体中同一个零件可能出现多次，每次位置不同。

如果没有 `Location` wrapper，就需要复制多份底层数据。

### 9.4 不利于局部修改和版本共享

当一个 solid 修改后，很多未变化的 Face / Edge 可以继续共享旧的 TShape，只有变化部分创建新的 TShape。

这让 OCCT 的拓扑结构更像一张可共享的图，而不是一棵必须完整复制的树。

---

## 10. TopExp_Explorer：拓扑遍历工具

`TopExp_Explorer` 是 Day2 最常用的工具。

它可以从一个 `TopoDS_Shape` 中递归查找指定类型的 sub-shape。

例如查找所有 Face：

```cpp
for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
{
    TopoDS_Face face = TopoDS::Face(exp.Current());
}
```

查找所有 Edge：

```cpp
for (TopExp_Explorer exp(shape, TopAbs_EDGE); exp.More(); exp.Next())
{
    TopoDS_Edge edge = TopoDS::Edge(exp.Current());
}
```

注意：

```text
TopExp_Explorer 不是只遍历下一层；
它会递归查找指定类型。
```

例如从 `Solid` 查 `Vertex`，它会沿着：

```text
Solid -> Shell -> Face -> Wire -> Edge -> Vertex
```

递归找到所有 Vertex。

如果只想遍历直接子节点，可以使用：

```cpp
TopoDS_Iterator
```

区别是：

```text
TopExp_Explorer = 递归查找某种类型
TopoDS_Iterator = 只遍历直接子节点
```

---

## 11. TopTools：拓扑容器与去重

用 `TopExp_Explorer` 遍历 Edge 或 Vertex 时，常常会遇到重复。

例如一个 box 直觉上只有 8 个 Vertex，但直接遍历 Vertex 时可能会得到更多，因为同一个 Vertex 可能被多个 Edge 引用。

这时可以用：

```cpp
TopTools_MapOfShape
```

它可以保存一组 Shape，并自动去重。

示例：

```cpp
TopTools_MapOfShape map;

for (TopExp_Explorer exp(shape, TopAbs_VERTEX); exp.More(); exp.Next())
{
    map.Add(exp.Current());
}

std::cout << "Unique vertices: " << map.Extent() << std::endl;
```

常见容器还有：

```cpp
TopTools_MapOfShape
TopTools_IndexedMapOfShape
TopTools_DataMapOfShapeShape
TopTools_IndexedDataMapOfShapeShape
```

可以简单理解为 OCCT 为 `TopoDS_Shape` 准备的专用 set / map。

---

## 12. OCCT 的 B-Rep 是什么？

B-Rep 是 Boundary Representation，也就是边界表示。

它的核心思想是：

```text
一个实体不是直接用体素表示；
而是用边界来表示。
```

例如一个 box：

```text
Solid
  Shell
    6 个 Face
      每个 Face 有 Wire
        Wire 由 Edge 组成
          Edge 由 Vertex 限定
```

B-Rep 中同时包含：

```text
Geometry
Topology
```

Geometry 描述数学形状：

```text
点、曲线、曲面
```

Topology 描述连接关系和边界关系：

```text
Vertex、Edge、Wire、Face、Shell、Solid
```

所以 OCCT 的 B-Rep 可以理解成：

```text
B-Rep = Geometry + Topology + Location + Orientation + Tolerance
```

其中：

```text
Geometry 负责精确数学描述
Topology 负责组织结构和边界关系
Location 负责位置实例化
Orientation 负责边界使用方向
Tolerance 负责处理 CAD 中不可避免的数值误差
```

---

## 13. OCCT B-Rep 和其他几何内核 B-Rep 的差别

市面上的几何内核很多，比如：

```text
Parasolid
ACIS
CGAL
OpenNURBS
OCCT
```

它们都可能支持某种形式的 B-Rep，但侧重点不同。

### 13.1 OCCT 的特点

OCCT 的特点是：

```text
开源
C++ 实现
B-Rep 数据结构相对透明
TopoDS / BRep / Geom 等层次比较清楚
可以直接访问底层拓扑结构
适合学习、研究、自研 CAD / CAM / CAE 工具
```

你可以直接看到并操作：

```cpp
TopoDS_Shape
TopoDS_Face
TopoDS_Edge
TopExp_Explorer
TopTools_MapOfShape
BRep_Tool::Curve
BRep_Tool::Surface
```

这对学习几何内核非常有帮助。

---

### 13.2 Parasolid / ACIS 的特点

Parasolid 和 ACIS 是商业几何内核，通常用于成熟 CAD / CAM / CAE 软件中。

它们的优势通常在于：

```text
成熟度高
鲁棒性强
商业支持完善
大量工业软件验证
建模算法能力强
```

但它们的内部实现通常不是公开的。

对于学习者来说，你很难像研究 OCCT 一样直接阅读其内部拓扑数据结构。

---

### 13.3 Mesh / Polyhedral B-Rep 与精确 B-Rep

OCCT 的 Face 可以有精确曲面，也可以有用于显示或近似的三角网格。

例如：

```text
精确几何：
Geom_Surface

显示网格：
Poly_Triangulation
```

如果一个模型只有三角形，没有底层精确曲面，就更接近 polyhedral model。

这类模型类似 STL：

```text
只有三角面片
没有精确圆柱面、球面、B-Spline 曲面
```

而真正的 CAD B-Rep 通常需要精确曲线和曲面。

例如一个圆柱：

```text
精确 B-Rep：
一个圆柱面 + 两个平面

Mesh：
很多三角形拼出来，看起来像圆柱
```

从显示角度，两者可能看起来差不多；但从建模和加工角度，差别非常大。

---

## 14. Day1 / Day2 的最小实践目标

学习完这两天，最小目标是写一个拓扑统计工具：

```cpp
void dumpShape(const TopoDS_Shape& shape)
{
    Count(shape, TopAbs_SOLID);
    Count(shape, TopAbs_SHELL);
    Count(shape, TopAbs_FACE);
    Count(shape, TopAbs_WIRE);
    Count(shape, TopAbs_EDGE);
    Count(shape, TopAbs_VERTEX);
}
```

其中：

```cpp
int Count(const TopoDS_Shape& shape, TopAbs_ShapeEnum type)
{
    int count = 0;

    for (TopExp_Explorer exp(shape, type); exp.More(); exp.Next())
    {
        ++count;
    }

    return count;
}
```

如果想去重，可以用：

```cpp
TopTools_MapOfShape
```

例如：

```cpp
int CountUnique(const TopoDS_Shape& shape, TopAbs_ShapeEnum type)
{
    TopTools_MapOfShape map;

    for (TopExp_Explorer exp(shape, type); exp.More(); exp.Next())
    {
        map.Add(exp.Current());
    }

    return map.Extent();
}
```

---

## 15. 最终总结

Day1 / Day2 的核心不是学会多少 API，而是建立 OCCT 的基本心智模型：

```text
gp 是基础数学几何对象；
Geom / Geom2d 是完整几何对象，通常用 Handle 管理；
TopoDS 是拓扑对象体系；
TopAbs 定义拓扑枚举；
TopLoc 定义位置；
TopExp 用来遍历拓扑；
TopTools 用来保存和去重拓扑对象；
BRep 是几何与拓扑的结合。
```

最关键的一句话是：

```text
Geometry 描述对象的数学形状；
Topology 描述这些几何对象如何被连接、限制、共享和使用。
```

再进一步：

```text
TopoDS_TShape 是真实拓扑数据；
TopoDS_Shape 是带 Location 和 Orientation 的轻量引用。
```

理解了这几个概念，后面学习建模算法、布尔运算、STEP 读写、曲面建模时，就不会只是机械调用 API，而是能理解 OCCT 背后的数据结构设计。