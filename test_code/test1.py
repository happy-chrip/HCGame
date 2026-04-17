GROUP_LENGTH: int = 245

from pathlib import Path
import time
import sys
import os
from typing import Optional

os.system("cls")

sys.path.append(str(Path(__file__).parent.parent))
# os.environ["HCGame_DEBUG_MODE"] = "1"

import HCGame

print(f"测试函数add结果:{HCGame.add(10, 15)}")

HCGame.display.create_window()


group: list[Optional[HCGame.Role]] = [None] * GROUP_LENGTH
surface = HCGame.load_image_as_surface(str(Path(__file__).parent / "test1.png"))

for i in range(GROUP_LENGTH):
    temp = HCGame.Role()
    temp.load_image_from_surface(surface)
    temp.buffer()
    # group.append(temp)
    print(temp.costume_number)
    group[i] = temp


def get_fps():
    _last_time = time.time()
    _max_fps = 0
    _fps_list = []
    _last_second = _last_time
    while True:
        _fps = 1 / max(0.00001, time.time() - _last_time)
        if time.time() - _last_second > 1:
            _fps_list.clear()
            _max_fps = 0
            _last_second = time.time()
        _fps_list.append(_fps)
        _max_fps = max(_max_fps, _fps)
        _last_time = time.time()
        yield sum(_fps_list) / len(_fps_list), _max_fps


fps = get_fps()
while not HCGame.has_quit_event():
    HCGame.display.blits_all()
    HCGame.display.update()

    now_fps, max_fps = next(fps)
    print(f"\rnow_fps={now_fps: <10.2f} max_fps={max_fps: <15.2f}", end="")
    # time.sleep(0.02)
print("\n程序跑到这啦! ")
