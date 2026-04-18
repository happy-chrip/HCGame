from setuptools import setup, Extension
from pathlib import Path

BASE_DIR = Path(__file__).parent
module = Extension(
    "HCGame",  # 模块名
    sources=["src/HCGame.c", "src/position.c", "src/display.c", "src/surface.c", "src/event.c", "src/role.c",
             "src/key.c", "src/log.c"],  # C 文件路径
    include_dirs=[str(BASE_DIR / "third_include/SDL")],
    library_dirs=[str(BASE_DIR / "third_lib")],
    libraries=["SDL3"],
    define_macros=[
        ("SDL", None),
    ],
    extra_compile_args=[
        "/utf-8",
    ],
    extra_link_args=[
        "user32.lib",
        "gdi32.lib",
        "winmm.lib",
        "shell32.lib",
        "advapi32.lib",
        "ole32.lib",
        "oleaut32.lib",
        "imm32.lib",
        "comctl32.lib",
        "version.lib",
        "uuid.lib",
        "setupapi.lib",
        "dinput8.lib",
        "dxguid.lib",
    ]
)

setup(
    name="HCGame",
    version="0.0.0.1",
    description="这是一个测试！",
    ext_modules=[module],
)
