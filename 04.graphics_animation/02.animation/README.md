# 21.animation -- 动画与状态机

## 概述
本示例演示 Qt5 动画框架与状态机的使用，包括属性动画(QPropertyAnimation)、动画组(QSequentialAnimationGroup/QParallelAnimationGroup)、窗口动画效果、QStateMachine 状态机，以及综合示例：用状态机驱动动画实现红绿灯演示。

## 知识点
- QPropertyAnimation 属性动画（位置、大小、透明度、颜色）
- 动画循环(Loop)、方向(Forward/Backward/PingPong)
- QEasingCurve 缓动曲线各类型
- QSequentialAnimationGroup 顺序动画组
- QParallelAnimationGroup 并行动画组
- 嵌套动画组、暂停/恢复/停止控制
- QGraphicsOpacityEffect 窗口淡入淡出
- 窗口几何动画（移动/缩放）
- QStateMachine 状态机框架
- QState、QFinalState 状态定义
- QSignalTransition、QKeyEventTransition 状态转换
- assignProperty、invokeMethod 状态动作
- 状态机 + 动画综合应用（交通灯演示）

## 构建与运行
```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```

## 关键文件
| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h | MainWindow类声明 |
| src/mainwindow.cpp | 动画与状态机UI及逻辑实现 |
| CMakeLists.txt | CMake构建配置 |
