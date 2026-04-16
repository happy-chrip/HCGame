# Created by ABai on 2026/3/22.
def add(a: int, b: int) -> int:
    ...

def has_quit_event() -> bool:
    ...

class Role:
    def __init__(self):
        self.costume_list: list = []    # 只读
        self.costume_number: int = -1
        self.scale: float = 1.0
        self.width_scale: float = 1.0
        self.height_scale: float = 1.0
        self.alpha: float = 1.0
        self.flip_x: bool = 0
        self.flip_y: bool = 0
        self.facing_angle: float = 0
        self.position: Position = Position(0, 0)
        ...
    def load_image(self, path: str):
        ...

    def buffer(self):
        ...

    def load_image_from_surface(self, surface: __Surface):
        ...

    def collide(self, target_role: Role):


class __Display:
    def create_window(self, width: int=800, height: int=600, title: str = "Welcome to use HCGame"):
        ...
    def blits_all(self) -> bool:
        ...
    def update(self) -> bool:
        ...

display: __Display

class __Surface:
    ...

class Position:
    def __init__(self,x=0, y=0):
        self.x = x
        self.y = y
    ...

def load_image_as_surface(path: str) -> __Surface:
    ...