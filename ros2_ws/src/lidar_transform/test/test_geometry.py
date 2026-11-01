import math


def rotate_z(x, y, angle_degrees):
    angle = math.radians(angle_degrees)
    return (
        math.cos(angle) * x - math.sin(angle) * y,
        math.sin(angle) * x + math.cos(angle) * y,
    )


def test_ninety_degree_rotation():
    x, y = rotate_z(1.0, 0.0, 90.0)
    assert math.isclose(x, 0.0, abs_tol=1e-6)
    assert math.isclose(y, 1.0, abs_tol=1e-6)


def test_rotation_preserves_xy_distance():
    before = math.hypot(2.0, -3.0)
    after = math.hypot(*rotate_z(2.0, -3.0, 30.0))
    assert math.isclose(before, after, rel_tol=1e-9)
