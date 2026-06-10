# 22.concurrent -- 并发编程

## 概述
本示例演示 Qt5 并发编程的各种方式，包括 QtConcurrent::run 后台任务、map/filter/reduce 并行数据处理、QThreadPool 线程池管理，以及并发图像处理的综合对比。

## 知识点
- QtConcurrent::run 在后台线程执行耗时任务
- QFuture 获取异步结果、QFutureWatcher 监听完成信号
- QFuture::cancel 取消任务
- QtConcurrent::mapped 并行转换数据
- QtConcurrent::filtered 并行过滤数据
- QtConcurrent::reduced 并行聚合数据
- QRunnable 自定义任务
- QThreadPool::globalInstance() 全局线程池
- 线程池最大线程数、任务队列、任务优先级
- 单线程 vs 多线程处理时间对比

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
| src/mainwindow.cpp | 并发编程UI及逻辑实现 |
| CMakeLists.txt | CMake构建配置 |
