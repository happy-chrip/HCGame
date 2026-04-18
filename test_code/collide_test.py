import random
from pathlib import Path
import time
import sys
import os

sys.path.append(str(Path(__file__).parent.parent))
# os.environ["HCGame_DEBUG_MODE"] = "1"

import HCGame

print(f"测试函数add结果:{HCGame.add(10, 15)}")
HCGame.display.create_window()

group: list[HCGame.Role] = list()
temp_surface = HCGame.load_image_as_surface(str(Path(__file__).parent / "小可走路2.png"))
for i in range(1000):
    a = HCGame.Role()
    a.load_image_from_surface(temp_surface)
    # a.buffer()
    group.append(a)
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
    for i in group:
        i.position.x = random.randint(0, 800)
        i.position.y = random.randint(0, 500)
    
    random.choice(group).collide(random.choice(group))


    HCGame.display.blits_all()
    HCGame.display.update()
    # time.sleep(0.75)
    now_fps, max_fps = next(fps)
    print(f"\rnow_fps={now_fps: <10.2f} max_fps={max_fps: <15.2f}", end="")
print("\n程序跑到这啦! ")
