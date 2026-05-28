下面按你截图里的 **40 天最短路径**，给每一段配上“优先教程/博客链接 + 没有现成教程时的学习方法”。优先看官方文档，其次看博客/示例。

---

## 第 1 天：跑 OCC Bottle Tutorial

**目标**：先不要纠结数学和内核细节，先把 OCC 工程跑起来，生成一个完整模型。

推荐链接：

1. **OCC 官方 Bottle Tutorial**
   这是最应该先跑的。官方说明它的目标是教你用 OCCT 服务构造一个 3D 对象，而不是完整解释所有类。([Open CASCADE][1])

2. **OCC Tutorials and Samples**
   官方教程入口，里面列了 Bottle Tutorial、Novice Guide、Draw Demo 等。([Open CASCADE][2])

3. **OCJS Bottle Example**
   如果你想看一个更短、更现代的 JavaScript 版本，可以辅助理解 Bottle 示例的建模流程。([ocjs.org][3])

学习方法：

```text
只做三件事：
1. 编译 / 运行成功
2. 看懂它用了哪些 OCC 类
3. 把最终模型导出成 STEP 或 BREP
```

不要第一天就深入 `TopoDS_Shape` 的内部实现。

---

## 第 2-3 天：写 Shape Explorer，打印 Solid / Shell / Face / Wire / Edge / Vertex

**目标**：理解 OCC 的拓扑层级。

推荐链接：

1. **OCC Modeling Data User Guide**
   这是学习 OCC 拓扑和几何数据结构的主文档。官方说明 Modeling Data 提供 2D/3D 几何模型的数据结构。([Open CASCADE][4])

2. **OCC BREP Format 文档**
   这个文档明确提到 BREP 文件可以存 vertices、edges、wires、faces、shells、solids、compsolids、compounds、location、orientation 等信息，很适合对照 OCC 的拓扑层级。([文档帮助][5])

3. **OCC Novice Guide**
   官方新手入口，适合快速熟悉基础 workflow。([Open CASCADE][6])

学习方法：

写一个工具函数：

```cpp
void DumpShape(const TopoDS_Shape& shape) {
    // 统计并打印：
    // TopAbs_SOLID
    // TopAbs_SHELL
    // TopAbs_FACE
    // TopAbs_WIRE
    // TopAbs_EDGE
    // TopAbs_VERTEX
}
```

你要输出类似：

```text
Solid count  : 1
Shell count  : 1
Face count   : 6
Wire count   : 6
Edge count   : 24
Vertex count : 48
```

注意：这里统计出来的 Edge / Vertex 可能包含重复“使用实例”，后面再学去重。

---

## 第 4-5 天：从 Edge / Face 取 Curve / Surface / p-curve

**目标**：把 “Topology + Geometry” 绑定关系吃透。

推荐链接：

1. **BRep_Tool Class Reference**
   `BRep_Tool` 是从 BRep 拓扑对象访问几何数据的核心类。官方说明它提供访问 BRep shapes 几何的方法。([Open CASCADE][7])

2. **BRep_Tool 旧版参考：CurveOnPlane / CurveOnSurface**
   可以重点看 `Curve`、`Surface`、`CurveOnSurface` 相关方法。([Open CASCADE][8])

3. **Modeling Data User Guide**
   继续配合看，因为里面包含几何工具、曲线曲面、BRep 结构。([Open CASCADE][4])

学习方法：

你要写三个函数：

```cpp
void PrintEdgeCurve(const TopoDS_Edge& edge);
void PrintFaceSurface(const TopoDS_Face& face);
void PrintEdgePCurveOnFace(const TopoDS_Edge& edge, const TopoDS_Face& face);
```

重点观察：

```text
Edge -> Geom_Curve + first/last 参数范围
Face -> Geom_Surface
Edge + Face -> Geom2d_Curve，也就是 p-curve
```

这一步非常关键。你要形成这个直觉：

```text
TopoDS_Edge 不是曲线本身
TopoDS_Face 不是曲面本身

Edge 通过 BRep_Tool 关联 Geom_Curve
Face 通过 BRep_Tool 关联 Geom_Surface
Edge 在某个 Face 上还可能有 Geom2d_Curve
```

---

## 第 6-10 天：学 Bezier + 参数曲线，用代码采样画图

**目标**：理解 `C(t) -> Point` 这个思想，不要一开始就啃 NURBS。

推荐链接：

1. **Pomax: A Primer on Bézier Curves**
   免费在线书，覆盖 Bezier 的数学和编程实现，非常适合入门。([Pomax][9])

2. **Indiscripts: The Definitive Tutorial on Bézier Curves**
   偏图形化解释，适合辅助理解 De Casteljau 算法。([indiscripts.com][10])

学习方法：

先手写，不要马上用 OCC。

你要自己实现：

```text
1. 线性 Bezier
2. 二次 Bezier
3. 三次 Bezier
4. De Casteljau 算法
5. 采样 100 个点
6. 输出 CSV 或画图
```

最小公式先记这个：

```text
参数曲线：
C(t) = (x(t), y(t), z(t))

直线：
C(t) = P0 + t(P1 - P0)
```

然后回到 OCC：

```cpp
Handle(Geom_BezierCurve) curve = ...;
gp_Pnt p = curve->Value(u);
```

你要对比：**自己算出来的点** 和 **OCC `Value(u)` 采样点**。

---

## 第 11-15 天：学 B-Spline / NURBS 基本概念

**目标**：不用完全推导公式，但要知道 Degree、Knots、Poles、Weights 是什么。

推荐链接：

1. **MIT Hyperbook: B-Spline Curves and Surfaces**
   系统讲 B-spline 曲线曲面，适合建立严肃数学概念。([Open CASCADE][4])
   这里也可以从 MIT Hyperbook 的曲线曲面章节入口开始看。([McNeel Forum][11])

2. **MIT OCW Computational Geometry**
   这门课覆盖 B-spline、NURBS、sweep、offset、fillet、BRep、鲁棒几何计算等，和 CAD Kernel 很相关。([YouTube][12])

3. **Pomax Bezier Primer**
   继续用它打基础，因为理解 Bezier 后再理解 B-Spline 会容易很多。([Pomax][9])

学习方法：

这阶段不要急着完整实现 NURBS evaluator。你先搞清楚这些词：

```text
Degree：次数
Pole / Control Point：控制点
Knot Vector：节点向量
Multiplicity：节点重复度
Weight：权重
Continuity：连续性
```

OCC 里重点看这些类：

```cpp
Geom_BSplineCurve
Geom_BSplineSurface
Geom_BezierCurve
Geom_BezierSurface
```

建议做 4 个小实验：

```text
1. 创建一条 BSpline 曲线
2. 打印 Degree / Knots / Poles
3. 修改一个 Pole，看曲线怎么变
4. 修改 Knot Multiplicity，看连续性怎么变
```

---

## 第 16-25 天：跑 Boolean / Fillet / Offset / Sweep

**目标**：理解真正的 Geometry Kernel 难点不只是存数据，而是修改 BRep。

推荐链接：

1. **OCC Modeling Algorithms User Guide**
   官方说明 Modeling Algorithms 模块包含大量拓扑建模算法，并会调用几何算法。([Open CASCADE][13])

2. **OCC Modeling Algorithms：Topology API 包列表**
   里面列了 `BRepAlgoAPI`、`BRepBuilderAPI`、`BRepFilletAPI`、`BRepFeat`、`BRepOffsetAPI`、`BRepPrimAPI`，正好对应你要学的建模算法。([Open CASCADE][14])

3. **OCC Boolean Operations Wiki**
   这篇讲 OCC Boolean 组件，包括 General Fuse、Boolean Operator、Section、Splitter。([GitHub][15])

4. **NEWe OpenCascade: Let's talk about fillets**
   这篇是 Fillet 实战博客，适合看倒角为什么复杂，以及如何处理 Boolean 后的边。([OpenCASCADE's recipies from NEWe][16])

5. **Roman Lygin: Surface modeling / Skinning and Lofting**
   Roman Lygin 的博客对 OCC 曲面建模很有价值，适合看 Loft / Skinning 思路。([Open CASCADE notes][17])

学习方法：

按这个顺序写 demo：

```text
1. BRepPrimAPI_MakeBox
2. BRepPrimAPI_MakeCylinder
3. BRepAlgoAPI_Cut：box cut cylinder，做一个孔
4. BRepAlgoAPI_Fuse：两个实体合并
5. BRepAlgoAPI_Common：求交
6. BRepFilletAPI_MakeFillet：选边倒圆
7. BRepOffsetAPI_MakePipe：沿路径扫掠
8. BRepOffsetAPI_MakeThickSolid：抽壳 / 厚壳
```

每个 demo 都要做两件事：

```text
成功后：
打印 Face / Edge 数量变化

失败后：
记录失败输入、半径、容差、模型截图
```

尤其要刻意制造失败案例：

```text
小倒角半径
过大倒角半径
相切实体 Boolean
薄壁 Offset
很短的小边
几乎重合的面
```

这是学习工业内核最有价值的部分。

---

## 第 26-40 天：对照公司 Kernel API 重写一遍这些练习

**目标**：把 OCC 知识迁移到你们公司的 Geometry Kernel。

这一段通常没有公开链接，因为是公司内部内核。但学习方法很明确。

你要做一张对照表：

| 通用概念         | OCC                         | 公司 Kernel |
| ------------ | --------------------------- | --------- |
| Shape / Body | `TopoDS_Shape`              | ?         |
| Solid        | `TopoDS_Solid`              | ?         |
| Shell        | `TopoDS_Shell`              | ?         |
| Face         | `TopoDS_Face`               | ?         |
| Loop         | `TopoDS_Wire`               | ?         |
| Edge         | `TopoDS_Edge`               | ?         |
| Vertex       | `TopoDS_Vertex`             | ?         |
| Curve        | `Geom_Curve`                | ?         |
| Surface      | `Geom_Surface`              | ?         |
| p-curve      | `BRep_Tool::CurveOnSurface` | ?         |
| Orientation  | `TopAbs_Orientation`        | ?         |
| Tolerance    | `BRep_Tool::Tolerance`      | ?         |
| Boolean      | `BRepAlgoAPI_*`             | ?         |
| Fillet       | `BRepFilletAPI_MakeFillet`  | ?         |
| Offset       | `BRepOffsetAPI_*`           | ?         |
| Sweep        | `BRepOffsetAPI_MakePipe` 等  | ?         |

然后重写前面的练习：

```text
1. 创建 box / cylinder
2. 遍历拓扑
3. 从 Edge 取 Curve
4. 从 Face 取 Surface
5. 取 p-curve
6. 做 Boolean Cut
7. 做 Fillet
8. 做 Offset
9. 做 Sweep
10. 导入 STEP 后遍历并打印结构
```

最终产出一份内部笔记：

```text
OCC vs 公司 Kernel：
- 拓扑结构对照
- 几何对象对照
- p-curve 处理方式
- tolerance 处理方式
- orientation / coedge / edgeuse 对照
- Boolean / Fillet / Offset API 对照
- 常见失败案例对照
```

这份笔记比单纯看文档更有价值。

---

## 我建议你收藏的最少链接清单

1. OCC Bottle Tutorial：([Open CASCADE][1])
2. OCC Modeling Data：([Open CASCADE][4])
3. BRep_Tool Reference：([Open CASCADE][7])
4. OCC Modeling Algorithms：([Open CASCADE][13])
5. OCC Boolean Operations Wiki：([GitHub][15])
6. Pomax Bezier Primer：([Pomax][9])
7. MIT Computational Geometry：([YouTube][12])
8. Roman Lygin Surface Modeling：([Open CASCADE notes][17])

按这个顺序学：

```text
OCC Bottle
→ Shape Explorer
→ BRep_Tool
→ Bezier
→ BSpline / NURBS
→ Boolean / Fillet / Offset / Sweep
→ 公司 Kernel API 对照
```

这条路线最快能让你从“知道概念”进入“能看懂和调试 Geometry Kernel”。

[1]: https://dev.opencascade.org/doc/overview/html/occt__tutorial.html?utm_source=chatgpt.com "Open CASCADE Technology: Modeling: Bottle Tutorial"
[2]: https://dev.opencascade.org/doc/overview/html/samples.html?utm_source=chatgpt.com "Tutorials and Samples"
[3]: https://ocjs.org/docs/examples/bottle?utm_source=chatgpt.com "Bottle Example"
[4]: https://dev.opencascade.org/doc/overview/html/occt_user_guides__modeling_data.html?utm_source=chatgpt.com "Modeling Data"
[5]: https://documentation.help/Open-Cascade/occt_user_guides__brep_wp.html?utm_source=chatgpt.com "Open CASCADE Technology: BRep Format"
[6]: https://dev.opencascade.org/doc/overview/html/samples__novice_guide.html?utm_source=chatgpt.com "Novice Guide"
[7]: https://dev.opencascade.org/doc/refman/html/class_b_rep___tool.html?utm_source=chatgpt.com "BRep_Tool Class Reference"
[8]: https://dev.opencascade.org/doc/occt-7.5.0/refman/html/class_b_rep___tool.html?utm_source=chatgpt.com "BRep_Tool Class Reference"
[9]: https://pomax.github.io/bezierinfo/?utm_source=chatgpt.com "A Primer on Bézier Curves"
[10]: https://indiscripts.com/post/2021/11/the-definitive-tutorial-on-bezier-curves?utm_source=chatgpt.com "The Definitive Tutorial on Bézier Curves"
[11]: https://discourse.mcneel.com/t/mathematically-and-graphically-describe-a-curve-bezier-curve-spline-curve-nurbs/114563?utm_source=chatgpt.com "Mathematically and Graphically describe a CURVE | Bezier ..."
[12]: https://www.youtube.com/watch?v=-aiErrvLRfE&utm_source=chatgpt.com "Exploring Bezier And Spline Curves"
[13]: https://dev.opencascade.org/doc/overview/html/occt_user_guides__modeling_algos.html?utm_source=chatgpt.com "Open CASCADE Technology: Modeling Algorithms"
[14]: https://dev.opencascade.org/doc/occt-6.7.0/overview/html/user_guides__modeling_algos.html?utm_source=chatgpt.com "Modeling Algorithms"
[15]: https://github.com/Open-Cascade-SAS/OCCT/wiki/boolean_operations?utm_source=chatgpt.com "boolean_operations · Open-Cascade-SAS/OCCT Wiki"
[16]: https://neweopencascade.wordpress.com/2018/10/17/lets-talk-about-fillets/?utm_source=chatgpt.com "Let's talk about fillets - OpenCASCADE's recipies from NEWe"
[17]: https://opencascade.blogspot.com/2010/01/surface-modeling-part5.html?utm_source=chatgpt.com "Surface modeling. Part5"
