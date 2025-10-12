import struct
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors


#=========================
# MapData 구조체
#=========================
class MapData:
    def __init__(self, mapId, minX, minZ, width, height, cellSize, grid):
        self.mapId = mapId
        self.minX = minX
        self.minZ = minZ
        self.width = width
        self.height = height
        self.cellSize = cellSize
        self.grid = grid

    def is_walable(self, worldX, worldZ):
        gridX = int((worldX - self.minX) / self.cellSize)
        gridZ = int((worldZ - self.minZ) / self.cellSize)
        if 0 <= gridX < self.width and 0 <= gridZ < self.height:
            return self.grid[gridZ, gridX] == 1
        return False

#=========================
# .bin 맵 파일 로드
#=========================
def load_map(path: str) -> MapData:
    with open(path, "rb") as f:
        header = struct.unpack("6i", f.read(24))
        mapId, minX, minZ, width, height, cellSize = header

        grid = np.frombuffer(f.read(width * height), dtype=np.uint8)
        grid = grid.reshape((height, width))

    print(f"✅ Loaded Map: ID={mapId}, Size={width}x{height}, Cell={cellSize}, Min=({minX},{minZ})")
    return MapData(mapId, minX, minZ, width, height, cellSize, grid)

#=========================
# 맵 시각화
#=========================
def show_map(map_data: MapData):
    plt.figure(figsize=(10, 10))
    cmap = mcolors.ListedColormap(['black', 'green'])
    bounds = [0, 0.5, 1]  # 0~0.5 → 빨강, 0.5~1 → 초록
    norm = mcolors.BoundaryNorm(bounds, cmap.N)
    
    plt.imshow(map_data.grid, cmap=cmap, norm=norm, origin="lower")
    plt.title(f"MapViewer - MapID={map_data.mapId}\nSize={map_data.width}x{map_data.height}, CellSize={map_data.cellSize}")
    plt.xlabel("X (width)")
    plt.ylabel("Z (height)")
    plt.colorbar(label="Walkable (1=Green, 0=Black)")

    # 클릭 시 좌표 정보 출력
    def onclick(event):
        if event.xdata is None or event.ydata is None:
            return
        gridX = int(event.xdata)
        gridZ = int(event.ydata)
        worldX = map_data.minX + gridX * map_data.cellSize
        worldZ = map_data.minZ + gridZ * map_data.cellSize
        walkable = map_data.grid[gridZ, gridX] == 1
        print(f"🧭 Click → Grid({gridX},{gridZ}) | World({worldX},{worldZ}) | {'WALKABLE ✅' if walkable else 'BLOCKED ❌'}")

    plt.gcf().canvas.mpl_connect('button_press_event', onclick)
    plt.show()

# =============================
# 실행부
# =============================
if __name__ == "__main__":
    path = "../mapData/NavMeshGrid.bin"  # 같은 폴더에 있는 파일
    map_data = load_map(path)
    show_map(map_data)