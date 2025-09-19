#!/bin/bash
set -e

# 配置
BUILD_DIR="build"
EXECUTABLE="cpplearn"        # CMake 构建出的可执行文件名
PERF_DATA="perf.data"
FLAME_GRAPH_DIR="$HOME/Flamegraph"
FLAME_SVG="flame.svg"

# 检查 FlameGraph 工具
if [ ! -d "$FLAME_GRAPH_DIR" ]; then
    echo "FlameGraph not found in $FLAME_GRAPH_DIR"
    echo "Please clone it: git clone https://github.com/brendangregg/Flamegraph.git $FLAME_GRAPH_DIR"
    exit 1
fi

# 创建 build 目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 运行 CMake
echo "Running CMake..."
cmake ..

# 构建项目
echo "Building project..."
cmake --build . -j$(nproc)

# 检查可执行文件
if [ ! -f "$EXECUTABLE" ]; then
    echo "Executable $EXECUTABLE not found in $BUILD_DIR"
    exit 1
fi

# 删除旧的 perf 数据
rm -f $PERF_DATA $FLAME_SVG out.folded

# 运行 perf record
echo "Running perf..."
perf record -F 99 -g -o $PERF_DATA ./$EXECUTABLE

# 生成火焰图
echo "Generating flamegraph..."
perf script -i $PERF_DATA | $FLAME_GRAPH_DIR/stackcollapse-perf.pl > out.folded
$FLAME_GRAPH_DIR/flamegraph.pl out.folded > $FLAME_SVG

echo "Done! Fire up $FLAME_SVG in a browser to view the flamegraph."
