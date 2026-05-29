## 目标

会用 TopExp_Explorer 遍历 TopoDS_Shape，并理解 OCC 的拓扑层级。

## 方法

根据 [OCC Modeling Data](https://dev.opencascade.org/doc/overview/html/occt_user_guides__modeling_data.html?utm_source=chatgpt.com) 学习。

重点关注：

- Topology
- Shape content
- Topological types
- Classes inheriting TopoDS_Shape
- Exploration of Topological Data Structures

学习 OCC 的拓扑层级定义：

```
Solid
  Shell
    Face
      Wire
        Edge
          Vertex
```